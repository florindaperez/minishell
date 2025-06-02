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

#include "minishell.h"
#include "minishell_executor.h"

// int g_get_signal; // Asegúrate que g_get_signal esté definida (probablemente en main.c)
// y sea accesible si es necesario aquí, o que el estado se pase de otra forma.

/*
* static void	control_and_d(char *line_input)
* -------------------------------------------------
* Maneja la señal Ctrl+D (EOF) proveniente de readline.
* Si line_input es NULL (lo que readline devuelve para EOF),
* imprime "exit" (si es un TTY) y termina el shell limpiamente
* usando el último estado de salida conocido (g_get_signal).
*
* line_input: La cadena retornada por readline.
*/
void	control_and_d(char *line_input)
{
	if (!line_input)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "exit\n", 5);
		exit(g_get_signal);
	}
}

/*
* static char	*generate_line(void)
* --------------------------------------
* Lee una línea de entrada del usuario utilizando readline, mostrando el prompt.
* Si la línea es válida (no NULL y no vacía), la añade al historial.
*
* Retorna: La línea leída (alocada por readline), o NULL si EOF (Ctrl+D).
*/
char	*generate_line(void)
{
	char	*line;

	line = readline("minishell$ ");
	if (line && *line)
		add_history(line);
	return (line);
}

/*
* static int	prepare_and_execute(t_cmd *b_cmd_list, \
* t_env **b_env_list_ptr)
* -------------------------------------------------------------------------
* Prepara la estructura de datos para el ejecutor (executor),
* traduce las estructuras de comando "parseo" a las de executor,
* invoca el pipeline de ejecución y actualiza el estado de salida global.
* Libera las estructuras de comando traducidas y el array de entorno temporal.
*
* b_cmd_list: Cabeza de la lista de comandos "parseo" (t_cmd *) parseados.
* b_env_list_ptr: Puntero al puntero de la cabeza de la lista de entorno
* principal "parseo" (t_env **).
*
* Retorna: 0 si la preparación y la llamada al ejecutor fueron exitosas
* (no implica que los comandos ejecutados tuvieran éxito),
* 1 si hubo un error durante la preparación/traducción.
*/
static int	prepare_and_execute(t_cmd *b_cmd_list, \
										t_env **b_env_list_ptr)
{
	t_data_env_exe	data_flp;
	int				previous_exit_status;

	previous_exit_status = g_get_signal;
	ft_memset(&data_flp, 0, sizeof(t_data_env_exe));
	data_flp.shell_env_list_ptr = b_env_list_ptr; /* Conexión al entorno principal */
	data_flp.cmds_head = convert_cmd_list_to_cms_list_exec(
			b_cmd_list);
	if (!data_flp.cmds_head && b_cmd_list)
	{
		return (1); /* Fallo en traducción de comandos */
	}
	if (data_flp.shell_env_list_ptr && *(data_flp.shell_env_list_ptr))
		data_flp.env_for_execve = \
			convert_env_list_to_exec_envp(*(data_flp.shell_env_list_ptr));
	else
		data_flp.env_for_execve = NULL;
	if (!data_flp.env_for_execve && data_flp.shell_env_list_ptr && \
		*(data_flp.shell_env_list_ptr))
	{
		free_cmd_list(data_flp.cmds_head);
		return (1); /* Fallo en conversión de entorno */
	}
	data_flp.last_exit_status = previous_exit_status;
	if (data_flp.cmds_head)
		execute_pipeline(data_flp.cmds_head, &data_flp);
	g_get_signal = data_flp.last_exit_status;
	free_cmd_list(data_flp.cmds_head);
	if (data_flp.env_for_execve)
		free_arr2d(data_flp.env_for_execve);
	return (0);
}

/*
* static void	main_process_loop_step(t_env **envlist_ptr)
* -----------------------------------------------------------------
* Realiza un ciclo completo de procesamiento para una línea de comando:
* leer línea, tokenizar, parsear, (expandir/heredoc si es necesario
* en la capa "parseo"), y luego preparar y ejecutar con executor.
*
* envlist_ptr: Puntero al puntero de la cabeza de la lista de entorno
* principal (t_env **).
*/
static void	main_process_loop_step(t_env **envlist_ptr)
{
	t_tok	*token_list;
	t_cmd	*cmd_list;
	char	*line_input;

	token_list = NULL;
	cmd_list = NULL;
	line_input = generate_line();
	control_and_d(line_input);
	if (!line_input || !*line_input)
	{
		if (line_input)
			free(line_input);
		return ;
	}
	tokenizer(&token_list, line_input);
	free(line_input);
	if (!token_list)
		return ;
	if (parser(&cmd_list, token_list) == 1)
	{
		if (cmd_list) /* El parser puede dejarla parcialmente alocada */
			cmd_free(&cmd_list);
		/* tok_free es usualmente responsabilidad del parser si falla */
		return ;
	}
	tok_free(&token_list); /* Si parser OK, tokens de parseo ya no se usan */
	if (!cmd_list)
		return ;
	/* --- Consideraciones para Expansión y Heredoc en "parseo" --- */
	/* Si executor hace toda la expansión de $VAR y maneja heredocs, estas */
	/* llamadas de parseo podrían ser para pre-procesamiento específico. */
	should_expand(cmd_list, *envlist_ptr);
	if (heredoc(cmd_list) != 0) /* Asumiendo que heredoc pre-procesa */
	{
		cmd_free(&cmd_list);
		return ;
	}
	prepare_and_execute(cmd_list, envlist_ptr);
	cmd_free(&cmd_list);
}

/*
* void	minishell(t_env *envlist_main)
* --------------------------------------------
* Función principal que maneja el bucle interactivo de minishell.
* Es llamada desde main() después de la inicialización del entorno.
*
* envlist_main: Puntero a la cabeza de la lista de entorno principal.
* Nota: Se pasa t_env *, pero las funciones internas
* a menudo necesitarán t_env ** si la cabeza puede cambiar.
*/
void	minishell(t_env *envlist_main)
{
	t_env	*current_env_list_head; // Copia local del puntero a la cabeza

	current_env_list_head = envlist_main;
	while (1)
	{
		signals_interactive();
		main_process_loop_step(&current_env_list_head);
		/*
		* Si main_process_loop_step (a través de builtins) modifica la cabeza
		* de la lista (ej. si la lista estaba vacía y export añade el primer
		* elemento, o unset elimina el último y la cabeza se vuelve NULL),
		* current_env_list_head (que es *envlist_ptr dentro de
		* main_process_loop_step) se actualizará.
		* El envlist_main original en main() también se actualizaría
		* porque main_process_loop_step recibe &current_env_list_head
		* (que sería equivalente a recibir &envlist_main si main
		* pasara la dirección de su variable local).
		* El diseño actual de main pasando &envlist a run_interactive_mode,
		* y si run_interactive_mode pasa ese mismo &envlist a esta función
		* minishell() (renombrando el parámetro), entonces está bien.
		* La clave es que main_process_loop_step reciba t_env **.
		*/
	}
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
