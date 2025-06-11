/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 17:00:08 by jocuni-p          #+#    #+#             */
/*   Updated: 2025/06/10 18:02:39 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"
#include "minishell_executor.h"

/*
 * cleanup_final_data
 * Libera los recursos persistentes de la estructura 'data' al final.
 */
static void	cleanup_final_data(t_data_env_exe *data)
{
	if (data->pwd)
		free(data->pwd);
	if (data->old_pwd)
		free(data->old_pwd);
	if (data->env_for_execve)
		free_arr2d(data->env_for_execve);
}

/*
** process_command_line
** Ahora recibe un puntero a la estructura 'data' persistente.
*/
static void	process_command_line(char *line, t_data_env_exe *data)
{
	t_cmd	*cmd_list;
	t_list	*heredoc_tmp_f;

	heredoc_tmp_f = NULL;
	cmd_list = tokenize_parse_expand(line, *(data->shell_env_list_ptr), \
									&heredoc_tmp_f);
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
	execute_pipeline_logic(cmd_list, data);
	if (cmd_list)
		cmd_free(&cmd_list);
	if (heredoc_tmp_f)
		ft_lstclear(&heredoc_tmp_f, del_heredoc_tmp_f);
}

/*
* control_and_d
* Modificado para señalar al bucle principal que debe terminar.
* Retorna 1 para salir, 0 para continuar.
*/
static int	control_and_d(char *line_input)
{
	if (!line_input)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "exit\n", 5);
		return (1);
	}
	return (0);
}

/*
* generate_line
* Lee una línea de la entrada del usuario usando readline. Si la línea es
* válida (no nula y no vacía), la añade al historial de comandos.
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
* minishell
* Declara 'data' aquí para que sea persistente.
* El bucle ahora puede terminar para permitir la limpieza final.
*/
void	minishell(t_env *envlist_main)
{
	t_data_env_exe	data;
	char			*line_input;

	ft_memset(&data, 0, sizeof(t_data_env_exe));
	data.shell_env_list_ptr = &envlist_main;
	while (1)
	{
		signals_interactive();
		line_input = generate_line();
		if (control_and_d(line_input))
			break ;
		if (ft_is_all_space(line_input))
		{
			if (line_input)
				free(line_input);
			continue ;
		}
		process_command_line(line_input, &data);
		free(line_input);
		line_input = NULL;
	}
	free(line_input);
	cleanup_final_data(&data);
}
