/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 17:00:08 by jocuni-p          #+#    #+#             */
/*   Updated: 2025/06/04 17:54:48 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"
#include "minishell_executor.h" 

/*
 * process_command_line
 * Esta función actúa como el puente entre el módulo de parseo
 * y el de ejecución. Tokeniza, parsea, expande, maneja los heredocs,
 * y luego ejecuta una única línea de comando.
 */
static void	process_command_line(char *line, t_env **envlist_ptr)
{
	t_cmd			*cmd_list;
	t_data_env_exe	data;
	t_list			*heredoc_tmp_f;

	heredoc_tmp_f =NULL;
	cmd_list = NULL;
	if (!line || !envlist_ptr)
		return ;
	cmd_list = tokenize_parse_expand(line, *envlist_ptr, &heredoc_tmp_f);
	if (cmd_list == NULL)
	{
		if(heredoc_tmp_f)
			ft_lstclear(&heredoc_tmp_f, del_heredoc_tmp_f);
		return ;
	}
	execute_pipeline_logic(cmd_list, envlist_ptr, &data);
	if (cmd_list)
		cmd_free(&cmd_list);
	if (heredoc_tmp_f)
		ft_lstclear(&heredoc_tmp_f, del_heredoc_tmp_f);
}

/*
 * control_and_d
 * Maneja la señal Ctrl+D (EOF) de readline.
 * Si line_input es NULL, imprime "exit" (si es un TTY) y
 * termina el shell usando el último estado de salida conocido (g_get_signal).
 */
static void	control_and_d(char *line_input)
{
	if (!line_input)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "exit\n", 5);
		exit(g_get_signal);
	}
}

/*
 * generate_line
 * Lee una línea de entrada del usuario usando readline, mostrando el prompt.
 * Si la línea es válida (no NULL y no vacía), la añade al historial.
 *
 * Retorna: La línea leída (alocada por readline), o NULL en EOF (Ctrl+D).
 */
static char	*generate_line(void)
{
	char	*line;

	line = readline("minishell$ ");
	if (line && *line)
		add_history(line);
	return (line);
}

/*
 * main_process_loop_step
 * Realiza un ciclo completo de procesamiento para una línea de comando:
 * lee la línea y luego la procesa a través de los módulos de parseo y ejecución.
 */
static void	main_process_loop_step(t_env **envlist_ptr)
{
	char	*line_input;

	line_input = generate_line();
	control_and_d(line_input);
	if (ft_is_all_space(line_input))
	{
		if (line_input)
			free(line_input);
		return ;
	}
	process_command_line(line_input, envlist_ptr);
	free(line_input);
	line_input = NULL;
}

/*
 * minishell
 * Función principal que maneja el bucle interactivo de minishell.
 * Es llamada desde main() después de la inicialización del entorno.
 *
 * envlist_main: Puntero a la cabeza de la lista de entorno principal.
 */
void	minishell(t_env *envlist_main)
{
	t_env	*current_env_list_head;

	current_env_list_head = envlist_main;
	while (1)
	{
		signals_interactive();
		main_process_loop_step(&current_env_list_head);
	}
}
