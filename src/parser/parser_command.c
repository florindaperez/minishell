/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:02:44 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 17:02:47 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h"

static bool	initialize_command_io(t_cmd *cmd)
{
	cmd->io = malloc(sizeof(t_cmd_io));
	if (!cmd->io)
	{
		msg_error_cmd("parser", "malloc failed for command IO",
			strerror(errno), 1);
		g_exit_status = 1;
		return (false);
	}
	init_io_fds(cmd->io);
	return (true);
}

t_cmd	*new_command(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
	{
		msg_error_cmd("parser", "malloc failed to create command",
			strerror(errno), 1);
		g_exit_status = 1;
		return (NULL);
	}
	cmd->path = NULL;
	cmd->args = NULL;
	cmd->next = NULL;
	cmd->prev = NULL;
	cmd->is_builtin = false;
	cmd->redir_error = false;
	cmd->was_literal_empty_command = false;
	cmd->io = NULL;
	if (!initialize_command_io(cmd))
	{
		free(cmd);
		return (NULL);
	}
	return (cmd);
}

static t_arg_info	**reallocate_args_array(t_arg_info **old_args,
											int arg_count,
											t_arg_info *new_arg_struct)
{
	t_arg_info	**new_array;
	int			i;

	new_array = malloc(sizeof(t_arg_info *) * (arg_count + 2));
	if (!new_array)
	{
		msg_error_cmd("parser", "malloc failed for new args array",
			strerror(errno), 1);
		g_exit_status = 1;
		return (NULL);
	}
	i = 0;
	while (i < arg_count)
	{
		new_array[i] = old_args[i];
		i++;
	}
	new_array[arg_count] = new_arg_struct;
	new_array[arg_count + 1] = NULL;
	return (new_array);
}

/*
* Handles cleanup if adding an argument fails or inputs are invalid.
* new_arg_info_struct is the struct that might need freeing.
*/
static void	handle_add_arg_info_failure(t_arg_info *new_arg_info_struct)
{
	if (new_arg_info_struct)
	{
		free_ptr(new_arg_info_struct->value);
		free_ptr(new_arg_info_struct);
	}
}

void	add_arg_info_to_cmd(t_cmd *cmd, t_arg_info *new_arg_info_struct)
{
	int			arg_count;
	t_arg_info	**new_args_array;

	if (!cmd || !new_arg_info_struct)
	{
		handle_add_arg_info_failure(new_arg_info_struct);
		return ;
	}
	arg_count = 0;
	if (cmd->args)
	{
		while (cmd->args[arg_count])
			arg_count++;
	}
	new_args_array = reallocate_args_array(cmd->args, arg_count,
			new_arg_info_struct);
	if (!new_args_array)
	{
		handle_add_arg_info_failure(new_arg_info_struct);
		return ;
	}
	free(cmd->args);
	cmd->args = new_args_array;
}
