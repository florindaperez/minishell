/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_file_apply.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 21:55:06 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 21:55:09 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h"

/*
* Abre un archivo para redirección de salida ('>' o '>>').
* Gestiona la creación, truncado o apéndice según el tipo de redirección.
*/
int	handle_output_redir_open(t_cmd *cmd, t_token_type type,
										char *fname)
{
	int		temp_fd;
	int		open_flags;
	mode_t	mode;

	cleanup_previous_io(cmd->io, false);
	open_flags = O_WRONLY | O_CREAT;
	mode = 0644;
	if (type == TOKEN_APPEND)
		open_flags |= O_APPEND;
	else
		open_flags |= O_TRUNC;
	errno = 0;
	temp_fd = open(fname, open_flags, mode);
	if (temp_fd == -1)
	{
		msg_error_cmd(cmd_name_for_error_in_parser(cmd), fname,
			strerror(errno), 1);
		g_exit_status = 1;
		cmd->redir_error = true;
		return (-1);
	}
	cmd->io->outfile = fname;
	cmd->io->append_mode = (type == TOKEN_APPEND);
	cmd->io->fd_out = temp_fd;
	return (1);
}

/*
* Aplica la configuración de una redirección de archivo (entrada o salida)
* a la estructura del comando.
*/
int	apply_file_redirection(t_cmd *cmd, t_token *redir_tk,
									char *fname)
{
	int	result;

	result = 1;
	if (redir_tk->type == TOKEN_REDIRECT_IN)
	{
		cleanup_previous_io(cmd->io, true);
		cmd->io->infile = fname;
		cmd->io->fd_in = -1;
	}
	else if (redir_tk->type == TOKEN_REDIRECT_OUT
		|| redir_tk->type == TOKEN_APPEND)
	{
		result = handle_output_redir_open(cmd, redir_tk->type, fname);
		if (result == -1)
			free_ptr(fname);
	}
	else
	{
		free_ptr(fname);
	}
	return (result);
}
