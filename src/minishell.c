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
**	process_command_line
**	Puente entre el parseo y la ejecución. Valida que el comando resultante
**	de la expansión no sea nulo o vacío antes de pasarlo al ejecutor.
**	Gestiona la creación y liberación de todos los recursos del ciclo.
*/
static void	process_command_line(char *line, t_env **envlist_ptr)
{
	t_cmd			*cmd_list;
	t_data_env_exe	data;
	t_list			*heredoc_tmp_f;

	heredoc_tmp_f = NULL;
	cmd_list = tokenize_parse_expand(line, *envlist_ptr, &heredoc_tmp_f);
	if (cmd_list == NULL || cmd_list->commands == NULL
		|| cmd_list->commands[0] == NULL
		|| *(cmd_list->commands[0]) == '\0')
	{
		if (cmd_list)
			cmd_free(&cmd_list);
		if (heredoc_tmp_f)
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
*	control_and_d
*	Maneja la señal Ctrl+D (EOF) de readline. Si la entrada es nula,
*	imprime "exit" y termina el shell con el último estado de salida.
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
*	generate_line
*	Lee una línea de la entrada del usuario usando readline. Si la línea es
*	válida (no nula y no vacía), la añade al historial de comandos.
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
*	minishell
*	Función principal que maneja el bucle interactivo de minishell.
*	Orquesta la lectura, el procesamiento y la ejecución de comandos.
*/
void	minishell(t_env *envlist_main)
{
	t_env	*current_env_list_head;
	char	*line_input;

	current_env_list_head = envlist_main;
	while (1)
	{
		signals_interactive();
		line_input = generate_line();
		control_and_d(line_input);
		if (ft_is_all_space(line_input))
		{
			if (line_input)
				free(line_input);
			continue ;
		}
		process_command_line(line_input, &current_env_list_head);
		free(line_input);
		line_input = NULL;
	}
}
