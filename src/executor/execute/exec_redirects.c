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

#include "minishell.h"
#include "minishell_executor.h"

/*
 * redirect_fd
 * Envuelve la llamada al sistema 'dup2' para incluir un manejo de errores
 * robusto. Si 'dup2' falla, termina el programa con un mensaje de error.
 *
 * old_fd: El descriptor de fichero que se va a duplicar.
 * new_fd: El descriptor de fichero de destino (ej: STDIN_FILENO).
 */
static void	redirect_fd(int old_fd, int new_fd)
{
	if (dup2(old_fd, new_fd) == -1)
		perror_exit("minishell: dup2 failed", EXIT_FAILURE);
}

/*
 * setup_child_redirections
 * Configura las redirecciones de entrada y salida para un proceso hijo dentro
 * de un pipeline. Da prioridad a las redirecciones de ficheros sobre las
 * tuberías. Cierra los descriptores de fichero originales después de usarlos
 * para evitar fugas y asegurar el correcto funcionamiento de las tuberías.
 *
 * cmd:         El comando cuyas redirecciones se están configurando.
 * pipe_in_fd:  El descriptor de lectura de la tubería de entrada (-1 si no hay).
 * pipe_out_fd: El descriptor de escritura de la tubería de salida (-1 si no
 * hay).
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
