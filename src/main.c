/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 17:55:12 by castorga          #+#    #+#             */
/*   Updated: 2024/06/07 16:59:38 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
* Variable global para almacenar el último estado de salida. Se usa
* principalmente para el manejo de señales y para que el estado sea
* accesible entre diferentes partes del shell.
*/
int	g_get_signal = 0;

// --- Declaraciones adelantadas de funciones estáticas auxiliares ---
static t_cmd	*helper_tokenize_parse_expand(char *line, t_env *env_list_head);
static void		helper_execute_pipeline_logic(t_cmd *bonus_cmd_list, \
											t_env **envlist_bonus_ptr, \
											t_data_env_exe *data_flperez);
// NOTA: process_command_line, run_non_interactive_mode, run_interactive_mode
// ya están declaradas en tu archivo original.

/*
* static t_cmd  *helper_tokenize_parse_expand(char *line, t_env *env_list_head)
* -----------------------------------------------------------------------------
* Tokeniza, parsea, expande variables y maneja heredocs para una línea de input.
* Libera la lista de tokens internamente después de parsear.
*
* line: La línea de comando en bruto.
* env_list_head: Puntero a la cabeza de la lista de entorno (t_env *) de bonus,
* necesario para la expansión.
*
* Retorna: La cabeza de la lista de comandos (t_cmd *) de bonus procesada,
* o NULL si hay un error o no hay comandos.
*/
// Esta es la versión que proporcionaste en el paso 31, que ya incluye la lógica
// para manejar token_list_bonus == NULL cuando line no es NULL.
static t_cmd	*helper_tokenize_parse_expand(char *line, t_env *env_list_head)
{
	t_tok	*token_list_bonus;
	t_cmd	*cmd_list_bonus;

	token_list_bonus = NULL;
	cmd_list_bonus = NULL;
	tokenizer(&token_list_bonus, line);
	if (token_list_bonus == NULL && line != NULL)
	{
		msg_error_cmd("", NULL, "command not found", 127);
		return (NULL);
	}
	if (token_list_bonus == NULL) // Si line era NULL o tokenizer falló de otra forma
		return (NULL);
	if (parser(&cmd_list_bonus, token_list_bonus) == 1)
	{
		if (cmd_list_bonus)
			cmd_free(&cmd_list_bonus);
		tok_free(&token_list_bonus);
		return (NULL);
	}
	tok_free(&token_list_bonus);
	if (cmd_list_bonus == NULL)
		return (NULL);
	should_expand(cmd_list_bonus, env_list_head);
	if (heredoc(cmd_list_bonus) != 0) // Asumiendo que heredoc establece g_get_signal si falla
	{
		cmd_free(&cmd_list_bonus);
		return (NULL);
	}
	return (cmd_list_bonus);
}

/*
* static void   helper_execute_pipeline_logic(t_cmd *bonus_cmd_list, ...)
* --------------------------------------------------------------------------
* (Sin cambios respecto a tu versión del paso 31)
*/
static void	helper_execute_pipeline_logic(t_cmd *bonus_cmd_list, \
											t_env **envlist_bonus_ptr, \
											t_data_env_exe *data_flperez)
{
	ft_memset(data_flperez, 0, sizeof(t_data_env_exe));
	data_flperez->shell_env_list_ptr = envlist_bonus_ptr;
	data_flperez->last_exit_status = g_get_signal;
	data_flperez->cmds_head = \
		convert_cmd_list_to_cms_list_exec(bonus_cmd_list);
	if (data_flperez->shell_env_list_ptr && \
		*(data_flperez->shell_env_list_ptr))
		data_flperez->env_for_execve = \
		convert_envlist_to_envp_exe(*(data_flperez->shell_env_list_ptr));
	else
		data_flperez->env_for_execve = NULL;
	if (data_flperez->cmds_head == NULL && bonus_cmd_list != NULL)
		write(2, "minishell: Error during command translation\n", 44);
	else if (!data_flperez->env_for_execve && \
		data_flperez->shell_env_list_ptr && \
		*(data_flperez->shell_env_list_ptr) && data_flperez->cmds_head)
		write(2, "minishell: Error converting environment\n", 40);
	else if (data_flperez->cmds_head)
	{
		execute_pipeline(data_flperez->cmds_head, data_flperez);
		g_get_signal = data_flperez->last_exit_status;
	}
	free_flperez_cmd_list(data_flperez->cmds_head);
	data_flperez->cmds_head = NULL;
	free_arr2d(data_flperez->env_for_execve);
	data_flperez->env_for_execve = NULL;
}

/*
* static void   process_command_line(char *line, t_env **envlist_bonus_ptr)
* --------------------------------------------------------------------------
* (Ajuste menor: si helper_tokenize_parse_expand devuelve NULL porque ya
* manejó un error como 'comando vacío', g_get_signal ya está establecido)
*/
static void	process_command_line(char *line, t_env **envlist_bonus_ptr)
{
	t_cmd			*bonus_cmd_list;
	t_data_env_exe	data_flperez;

	bonus_cmd_list = NULL;
	if (!line || !envlist_bonus_ptr) // line puede ser "", !line es falso.
		return ;
	bonus_cmd_list = helper_tokenize_parse_expand(line, *envlist_bonus_ptr);
	if (bonus_cmd_list == NULL)
	{
		// Si bonus_cmd_list es NULL, helper_tokenize_parse_expand ya manejó
		// el error (ej. "" o error de parser) y estableció g_get_signal.
		return ;
	}
	helper_execute_pipeline_logic(bonus_cmd_list, envlist_bonus_ptr, \
									&data_flperez);
	if (bonus_cmd_list) // Debería ser siempre verdad si no se retornó antes
		cmd_free(&bonus_cmd_list);
}

/*
* static void   run_non_interactive_mode(char *av[], t_env **envlist_bonus)
* --------------------------------------------------------------------------
* (Modificado para manejar explícitamente av[2] == "" sin ft_split)
*/
static void	run_non_interactive_mode(char *av[], t_env **envlist_bonus)
{
	char	**arg_input_lines;
	int		i;

	// Si av[2] es una cadena vacía y no contiene ';',
	// se procesa como un único comando vacío.
	if (av[2][0] == '\0' && ft_strchr(av[2], ';') == NULL)
	{
		process_command_line(av[2], envlist_bonus);
		// g_get_signal es establecido por process_command_line
		// -> helper_tokenize_parse_expand -> msg_error_cmd
	}
	else
	{
		arg_input_lines = ft_split(av[2], ';');
		if (!arg_input_lines)
		{
			msg_error_cmd("minishell", NULL, \
							"memory allocation error for command split", EXIT_FAILURE);
			return ;
		}
		i = 0;
		while (arg_input_lines[i])
		{
			process_command_line(arg_input_lines[i], envlist_bonus);
			i++;
		}
		free_arr2d(arg_input_lines);
	}
}

/*
* static void   run_interactive_mode(t_env **envlist_bonus)
* -----------------------------------------------------------
* (Lógica para "solo Enter" ya presente y correcta; el caso de `""` como input
* con comillas es manejado por el flujo normal que llega a find_command_path)
*/
static void	run_interactive_mode(t_env **envlist_bonus)
{
	char	*line;

	line = NULL;
	while (1)
	{
		signals_interactive();
		line = readline("minishell> ");
		if (!line) // Ctrl+D (EOF)
		{
			// La función original de control_and_d no está aquí,
			// pero Bash imprime "exit" en TTY para EOF.
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "exit\n", 5);
			// g_get_signal no se modifica aquí, se usa el último valor
			// para salir en main. Esto es razonable.
			break ;
		}
		if (*line) // Si la línea no está vacía (ej. NO es solo Enter)
		{
			add_history(line);
			process_command_line(line, envlist_bonus);
		}
		// Si *line era '\0' (solo Enter), no se hace nada,
		// g_get_signal no cambia, se muestra nuevo prompt. Correcto.
		free(line);
		line = NULL;
	}
	if (line) // Por si se sale del bucle por otra vía con line alocado
		free(line);
}

/*
* int   main(int ac, char *av[], char *envp[])
* --------------------------------------------
* (Modificado para llamar a run_non_interactive_mode si av[2] es "")
*/
int	main(int ac, char *av[], char *envp[])
{
	t_env	*envlist_bonus;

	envlist_bonus = NULL;
	signals_interactive(); // Configurar señales iniciales
	env_init_list(envp, &envlist_bonus);
	if (ac == 3 && ft_strcmp(av[1], "-c") == 0 && av[2] != NULL) // CAMBIO: Permitir av[2] == ""
	{
		run_non_interactive_mode(av, &envlist_bonus);
	}
	else if (ac != 1) // Error si hay argumentos pero no son -c "algo"
	{
		// ft_msgs(10, NULL); // Asumiendo que esto imprime un error de uso
		write(STDERR_FILENO, "minishell: incorrect arguments\n", 31);
		set_exit_status(EXIT_FAILURE); // O un código de error de uso específico
		// cleaner_envlist se llama abajo
	}
	else // Modo interactivo
	{
		// init_msg(); // Si tienes un mensaje de bienvenida
		run_interactive_mode(&envlist_bonus);
	}
	cleaner_envlist(&envlist_bonus); // Limpiar al final
	return (g_get_signal); // Devolver el último estado de salida
}

/*
int	g_get_signal = 0;
//set_signals(PARENT) => signals_interactive();

int	main(int ac, char *av[], char *envp[])
{
	t_env	*envlist;

	(void)av;
	envlist = NULL;
	if (ac != 1)
	{
		ft_msgs(10, NULL);
		exit(EXIT_FAILURE);
	}
	env_init_list(envp, &envlist);
	while (1)
	{
		signals_interactive();
		minishell(envlist);
	}
	return (0);
}
*/
