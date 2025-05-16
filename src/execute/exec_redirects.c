/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redirects.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 16:14:40 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/10 16:14:45 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* En un archivo como exec_redirects.c */
#include "minishell.h" // Incluir lo necesario

static void	redirect_fd(int old_fd, int new_fd, const char *error_msg_prefix)
{
	if (dup2(old_fd, new_fd) == -1)
		perror_exit(error_msg_prefix, EXIT_FAILURE);
}

/**
 * Configura las redirecciones de E/S para un proceso hijo.
 * Usa pipes y descriptores de archivo ya abiertos en cmd->io.
 */
void	setup_child_redirections(t_cmd *cmd, int pipe_in_fd,
									int pipe_out_fd)
{
	bool	stdin_redirected;
	bool	stdout_redirected;

	stdin_redirected = false;
	stdout_redirected = false;
	if (cmd && cmd->io && cmd->io->fd_in != -1)
	{
		redirect_fd(cmd->io->fd_in, STDIN_FILENO, "dup2 file input");
		safe_close(&cmd->io->fd_in);
		stdin_redirected = true;
	}
	if (cmd && cmd->io && cmd->io->fd_out != -1)
	{
		redirect_fd(cmd->io->fd_out, STDOUT_FILENO, "dup2 file output");
		safe_close(&cmd->io->fd_out);
		stdout_redirected = true;
	}
	if (!stdin_redirected && pipe_in_fd != -1)
		redirect_fd(pipe_in_fd, STDIN_FILENO, "dup2 pipe input");
	if (!stdout_redirected && pipe_out_fd != -1)
		redirect_fd(pipe_out_fd, STDOUT_FILENO, "dup2 pipe output");
	safe_close(&pipe_in_fd);
	safe_close(&pipe_out_fd);
}
