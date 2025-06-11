/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_process_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:59:05 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/12 11:59:07 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * redir_restore_fds_on_fail
 * Restaura los descriptores de fichero estándar (stdin y/o stdout) a partir
 * de sus copias de seguridad. Se utiliza para revertir redirecciones en
 * caso de que ocurra un error a mitad de una operación de E/S.
 *
 * io:   La estructura de E/S que contiene los descriptores de fichero de
 * respaldo.
 * mode: Un 't_restore_mode_exe' que especifica si se debe restaurar stdin,
 * stdout, o ambos.
 */
void	redir_restore_fds_on_fail(t_cmd_io_exe *io, t_restore_mode_exe mode)
{
	if (!io)
		return ;
	if (mode == RESTORE_BOTH || mode == RESTORE_STDOUT_ONLY)
	{
		if (io->stdout_backup != -1)
		{
			dup2(io->stdout_backup, STDOUT_FILENO);
			close(io->stdout_backup);
			io->stdout_backup = -1;
		}
	}
	if (mode == RESTORE_BOTH || mode == RESTORE_STDIN_ONLY)
	{
		if (io->stdin_backup != -1)
		{
			dup2(io->stdin_backup, STDIN_FILENO);
			close(io->stdin_backup);
			io->stdin_backup = -1;
		}
	}
}

/*
 * open_and_dup_infile
 * Gestiona la redirección de entrada ('<'). Abre el fichero especificado
 * en 'io->infile' y duplica su descriptor de fichero a la entrada estándar
 * (STDIN_FILENO) del proceso actual.
 *
 * io:               La estructura de E/S que contiene el nombre del fichero de
 * entrada ('infile').
 * cmd_name_for_err: El nombre del comando actual, para usarlo en mensajes
 * de error más descriptivos.
 *
 * Retorna: 'true' si la apertura y la redirección son exitosas, 'false' en
 * caso de cualquier error.
 */
bool	open_and_dup_infile(t_cmd_io_exe *io, char *cmd_name_for_err)
{
	int	saved_errno;

	if (!io || !io->infile)
		return (false);
	errno = 0;
	io->fd_in = open(io->infile, O_RDONLY);
	saved_errno = errno;
	if (io->fd_in == -1)
	{
		if (saved_errno == ENOENT)
			msg_error_cmd(NULL, io->infile, strerror(saved_errno), 1);
		else
			msg_error_cmd(cmd_name_for_err, io->infile, \
							strerror(saved_errno), 1);
		g_get_signal = 1;
		return (false);
	}
	if (dup2(io->fd_in, STDIN_FILENO) == -1)
	{
		msg_error_cmd(cmd_name_for_err, io->infile, "dup2 failed", 1);
		safe_close(&io->fd_in);
		g_get_signal = 1;
		return (false);
	}
	return (true);
}
