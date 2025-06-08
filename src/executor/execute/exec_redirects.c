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
#include "minishell.h" // Incluir lo necesario
#include "minishell_executor.h"

/*
* redirect_fd
*
* Wrapper para dup2 que incluye manejo de errores.
* Duplica old_fd en new_fd.
*/
static void	redirect_fd(int old_fd, int new_fd)
{
	if (dup2(old_fd, new_fd) == -1)
		perror_exit("minishell: dup2 failed", EXIT_FAILURE);
}

/*
* setup_child_redirections
*
* Configura las redirecciones de E/S para un proceso hijo.
* Prioriza las redirecciones de fichero (ej: < file) sobre los pipes.
* Cierra los descriptores de fichero originales de los pipes después
* de la redirección para prevenir cuelgues y fugas de descriptores.
*/
void	setup_child_redirections(t_cmd_exe *cmd, int pipe_in_fd,
									int pipe_out_fd)
{
	if (cmd && cmd->io && cmd->io->fd_in != -1)
	{
		redirect_fd(cmd->io->fd_in, STDIN_FILENO);
		safe_close(&cmd->io->fd_in);
	}
	else if (pipe_in_fd != -1)
		redirect_fd(pipe_in_fd, STDIN_FILENO);
	if (cmd && cmd->io && cmd->io->fd_out != -1)
	{
		redirect_fd(cmd->io->fd_out, STDOUT_FILENO);
		safe_close(&cmd->io->fd_out);
	}
	else if (pipe_out_fd != -1)
		redirect_fd(pipe_out_fd, STDOUT_FILENO);
	if (pipe_in_fd != -1)
		safe_close(&pipe_in_fd);
	if (pipe_out_fd != -1)
		safe_close(&pipe_out_fd);
}
