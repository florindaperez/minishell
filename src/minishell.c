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

/* Variable global para el estado de salida (definida en main.c) */
extern int	g_get_signal;

/*
 * Lee una línea de input usando readline en modo interactivo.
 * Añade líneas no vacías al historial.
 * Retorna: La línea leída (necesita free), o NULL en EOF.
 * Esta función es estática y pertenece a la lógica del bucle principal.
 */
static char	*read_command_line_interactive(void)
{
	char	*line;

	line = readline("minishell$ ");
	if (line && *line)
		add_history(line);
	return (line);
}

/*
 * Función principal del ciclo interactivo de Minishell.
 * Es llamada en bucle por main().
 * Se encarga de:
 * 1. Leer una línea de entrada.
 * 2. Manejar EOF (Ctrl+D) para terminar el shell.
 * 3. Si hay entrada, llamar a las funciones de parseo.
 * 4. Si el parseo es exitoso y no hay señales, llamar a las de ejecución.
 * 5. Liberar recursos del ciclo actual.
 * envlist_head_ptr: Puntero al puntero de la cabeza de la lista de entorno.
 * Esto permite que builtins modifiquen el entorno.
 * Esta función es pública (declarada en minishell.h) y llamada por main().
 */
void	minishell(t_env **envlist_head_ptr)
{
	char			*line;
	t_cmd			*cmd_list_parsed;
	t_data_env_exe	execution_data_struct;

	line = read_command_line_interactive();
	if (!line)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "exit\n", 5);
		cleaner_envlist(envlist_head_ptr);
		exit(g_get_signal);
	}
	if (*line)
	{
		cmd_list_parsed = parse_and_prepare_line(line, *envlist_head_ptr);
		if (cmd_list_parsed != NULL && g_get_signal == 0)
		{
			execute_prepared_commands(cmd_list_parsed,
				envlist_head_ptr, &execution_data_struct);
		}
		if (cmd_list_parsed)
			cmd_free(&cmd_list_parsed);
	}
	free(line);
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
