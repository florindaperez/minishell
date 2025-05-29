/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 17:00:08 by jocuni-p          #+#    #+#             */
/*   Updated: 2024/06/10 10:51:57 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"         // Para t_cmd, t_env, y otras definiciones "bonus"
#include "minishell_executor.h" // Para t_data_env_exe, y los prototipos de funciones del executor y de la capa de traducción

/*
* Handles Ctrl+D (EOF) signal from readline.
* Exits the shell cleanly.
*/
void	control_and_d(char *line_input)
{
	if (!line_input)
	{
		// La primera condición !isatty es para cuando la entrada no es
		// una terminal, ej. `echo "ls" | ./minishell`. En ese caso,
		// readline devuelve NULL tras procesar la entrada.
		// El segundo write es para el caso interactivo (Ctrl+D).
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "exit\n", 5);
		// Aquí podrías querer limpiar envlist antes de salir si es el fin del programa
		exit(g_get_signal); // Salir con el último estado conocido
		//exit(EXIT_SUCCESS); // O siempre salir con SUCCESS en Ctrl+D interactivo
	}
}

/*
* Reads a line of input from the user using readline.
* Adds valid lines to history.
*/
char	*generate_line(void)
{
	char	*line;

	line = readline("minishell$ "); // Tu prompt
	if (line && *line) // Solo añadir a historial si la línea no es NULL y no está vacía
		add_history(line);
	return (line);
}

/*
* Prepara los datos para el ejecutor de flperez y lo invoca.
* Esta función es un candidato a ser dividida para Norminette.
*/
static int	prepare_and_execute_flperez(t_cmd *b_cmd_list,
										t_env *b_env_list)
{
	t_data_env_exe	data_flp; // Estructura para flperez_core
	int				previous_exit_status;

	previous_exit_status = g_get_signal; // Guardar antes de modificar
	ft_memset(&data_flp, 0, sizeof(t_data_env_exe));
	data_flp.cmds_head = convert_bonus_cmd_list_to_flperez_cmd_list(
			b_cmd_list);
	if (!data_flp.cmds_head && b_cmd_list)
	{
		// ft_putstr_fd("minishell: error during command translation\n", STDERR_FILENO);
		// La capa de traducción debería manejar la liberación de memoria parcial.
		// Podrías querer setear g_get_signal a un error específico aquí.
		return (1); // Indicar fallo en preparación
	}
	data_flp.env = convert_bonus_envlist_to_flperez_envp(b_env_list);
	if (!data_flp.env && b_env_list)
	{
		// ft_putstr_fd("minishell: error converting environment\n", STDERR_FILENO);
		free_flperez_cmd_list(data_flp.cmds_head); // Liberar lo ya traducido
		return (1); // Indicar fallo
	}
	data_flp.last_exit_status = previous_exit_status;
	if (data_flp.cmds_head) // Solo ejecutar si hay comandos
		execute_pipeline(data_flp.cmds_head, &data_flp); // LLAMADA A FLPEREZ_CORE
	g_get_signal = data_flp.last_exit_status; // Actualizar estado global
	free_flperez_cmd_list(data_flp.cmds_head);
	if (data_flp.env) // Solo liberar si se alocó
		free_arr2d(data_flp.env);
	return (0); // Indicar éxito en preparación y llamada (no necesariamente en ejecución)
}

/*
* Proceso principal para cada línea de comando en modo interactivo.
* Esta función ha sido refactorizada para el nuevo flujo.
* Podría necesitar dividirse más para Norminette.
*/
static void	main_process_loop_step(t_env *envlist_bonus)
{
	t_tok	*token_list_bonus;
	t_cmd	*cmd_list_bonus; // Salida de tu parser de bonus
	char	*line_input;

	token_list_bonus = NULL;
	cmd_list_bonus = NULL;
	line_input = generate_line();
	control_and_d(line_input); // Maneja Ctrl+D (EOF), sale si es necesario
	if (!line_input || !*line_input) // Línea vacía o solo espacios (readline devuelve "")
	{
		if (line_input) // Si es "" (no NULL), liberar
			free(line_input);
		return ; // Volver al prompt
	}
	tokenizer(&token_list_bonus, line_input);
	free(line_input); // La línea ya no es necesaria después de tokenizar
	if (!token_list_bonus) // Tokenizer no produjo tokens (ej. línea solo de espacios)
		return ;
	if (parser(&cmd_list_bonus, token_list_bonus) == 1) // Tu parser de bonus
	{
		// El parser de bonus ya maneja errores y libera token_list_bonus.
		// cmd_list_bonus puede estar parcialmente formado; cmd_free de bonus debe manejarlo.
		if (cmd_list_bonus)
			cmd_free(&cmd_list_bonus); // Tu cmd_free para estructuras de bonus
		return ;
	}
	tok_free(&token_list_bonus); // Si parser tuvo éxito, libera los tokens de bonus
	if (!cmd_list_bonus) // Si el parser no produjo comandos (caso raro si parser retorna 0)
		return ;
	// --- Lógica de Expansión y Heredoc (REVISAR PROFUNDAMENTE) ---
	// 1. Expansión (should_expand):
	//    La expansión de argumentos ($VAR, etc.) la hará flperez_core.
	//    Si `should_expand` hacía expansiones tempranas (ej. nombres de archivo
	//    en redirecciones ANTES de la traducción), esa lógica específica
	//    debe invocarse aquí o integrarse en el parser.
	//    Si era solo para argumentos de comando, esta llamada es obsoleta aquí.
	// should_expand(cmd_list_bonus, envlist_bonus); // COMENTADO PARA REVISIÓN

	// 2. Heredoc (heredoc):
	//    Si flperez_core crea el archivo temporal del heredoc (Opción 1),
	//    esta función de bonus ya NO debería crear ese archivo para la ejecución.
	//    Su rol sería asegurar que t_redir (bonus) tenga el DELIMITADOR
	//    y el flag `original_delimiter_had_quotes` listos para la capa de traducción.
	//    Si tu parser ya lo hace (ej. en redir_new_node), esta llamada podría no ser necesaria.
	//    Si `heredoc(cmd_list_bonus)` realiza este pre-procesamiento del delimitador, se mantiene.
	//    Si también realiza expansión DENTRO del heredoc, y quieres que flperez lo haga,
	//    entonces esta parte de `heredoc()` de bonus se vuelve obsoleta.
	if (heredoc(cmd_list_bonus) != 0) // Asumiendo que heredoc ahora retorna un estado
	{                                 // y solo pre-procesa para la capa de traducción.
		cmd_free(&cmd_list_bonus);    // Si falla el pre-procesamiento del heredoc.
		return ;
	}
	prepare_and_execute_flperez(cmd_list_bonus, envlist_bonus);
	cmd_free(&cmd_list_bonus); // Liberar la estructura de comando original de bonus
}

/*
* Función principal que maneja el bucle interactivo de minishell.
*/
void	minishell(t_env *envlist_bonus)
{
	// El main ya verifica isatty(STDIN_FILENO) para el modo -c.
	// Esta función minishell() se llama para el bucle interactivo.
	// Si se llama, se asume que es un TTY.
	// La comprobación original de isatty aquí podría ser redundante
	// si main asegura que solo se llama en modo interactivo.

	// if (isatty(STDIN_FILENO)) // Esta comprobación estaba en tu minishell()
	// {
	while (1) // Bucle infinito para el modo interactivo
	{
		signals_interactive();          // Configura señales para el prompt antes de leer la entrada.
		main_process_loop_step(envlist_bonus);
		// El estado de salida g_get_signal ya fue actualizado por prepare_and_execute
	}
	// }
	// else // Este else ya no sería alcanzado si main controla el modo
	// {
	//    write(STDERR_FILENO, "minishell: input is not a terminal\n", 36);
	//    exit(EXIT_FAILURE); // O un código de error específico
	// }
}
/*
void	control_and_d(char *line)
{
	if (!line)
	{
		if (!isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "exit\n", 5);
		write(STDOUT_FILENO, "exit\n", 5);
		exit(EXIT_SUCCESS);
	}
}

char	*generate_line(void)
{
	char	*line;

	line = readline("minishell$ ");
	if (line && *line)
		add_history(line);
	return (line);
}

static void	main_process(t_env *envlist)
{
	t_tok			*tok;
	t_cmd			*cmd;
	t_exe			exe;
	int				size_pipe;
	char			*line;

	tok = NULL;
	cmd = NULL;
	line = generate_line();
	control_and_d(line);
	if (!line || !*line)
		return (free(line));
	tokenizer(&tok, line);
	free(line);
	if (parser(&cmd, tok) == 1)
		return ;
	tok_free(&tok);
	should_expand(cmd, envlist);
	init_exe(&exe, cmd);
	heredoc(cmd);
	size_pipe = cmd_size(cmd);
	pre_executor(&envlist, cmd, &exe, size_pipe);
	cmd_free(&cmd);
}

void	minishell(t_env *envlist)
{
	if (isatty(STDIN_FILENO))
		main_process(envlist);
	else
	{
		write(2, "run ./minishell (without arguments or operators)\n", 49);
		exit(STDERR_FILENO);
	}
}
*/
