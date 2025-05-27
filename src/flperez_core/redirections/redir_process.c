/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:19:59 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/12 10:20:06 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Prototipos internos si son necesarios por el orden */
/* static bool open_and_dup_outfile(t_cmd_io *io, char *cmd_name_for_err, */
/* mode_t mode); */
/* static bool handle_existing_redir_out_fd(t_cmd_io *io, */
/* char *cmd_name_for_err); */

/*
 * [NUEVA FUNCIÓN AUXILIAR ESTATICA]
 * Solo abre el archivo de salida especificado. Devuelve fd o -1.
 * Extraída de open_and_dup_outfile para reducir líneas.
 */
static int	open_outfile_for_redir(t_cmd_io *io, char *cmd_name_for_err,
	int open_flags, mode_t mode)
{
	int	fd;
	int	saved_errno_out;

	errno = 0;
	fd = open(io->outfile, open_flags, mode);
	saved_errno_out = errno;
	if (fd == -1)
	{
		msg_error_cmd(cmd_name_for_err, io->outfile, \
						strerror(saved_errno_out), 1);
		g_exit_status = 1;
		return (-1);
	}
	return (fd);
}

/*
 * [FUNCIÓN MODIFICADA]
 * Abre (usando helper), duplica a STDOUT_FILENO y maneja errores de dup2.
 * Ahora cumple el límite de líneas.
 */
static bool	open_and_dup_outfile(t_cmd_io *io, char *cmd_name_for_err,
	mode_t mode)
{
	int	open_flags;
	int	fd;

	if (redir_create_path_if_needed(io->outfile) == -1)
		return (false);
	open_flags = O_WRONLY | O_CREAT;
	if (io->append_mode)
		open_flags |= O_APPEND;
	else
		open_flags |= O_TRUNC;
	fd = open_outfile_for_redir(io, cmd_name_for_err, open_flags, mode);
	if (fd == -1)
		return (false);
	io->fd_out = fd;
	if (dup2(io->fd_out, STDOUT_FILENO) == -1)
	{
		msg_error_cmd(cmd_name_for_err, io->outfile, "dup2 failed", 1);
		safe_close(&io->fd_out);
		return (false);
	}
	return (true);
}

/*
 * Gestiona la redirección de entrada para un comando. Mantiene la lógica
 * original (prioriza fd_in sobre infile). Llama a open_and_dup_infile (movida).
 */
bool	redir_handle_input(t_cmd *cmd, t_cmd_io *io, char *cmd_name_for_err)
{
	(void)cmd;
	if (!io)
		return (false);
	if (io->fd_in != -1)
	{
		if (dup2(io->fd_in, STDIN_FILENO) == -1)
		{
			msg_error_cmd(cmd_name_for_err, "stdin", "dup2 failed", 1);
			g_exit_status = 1;
			return (false);
		}
	}
	else if (io->infile)
	{
		if (!open_and_dup_infile(io, cmd_name_for_err))
			return (false);
	}
	return (true);
}

/*
 * Función auxiliar para manejar dup2 de un fd de salida existente (io->fd_out).
 */
static bool	handle_existing_redir_out_fd(t_cmd_io *io, char *cmd_name_for_err)
{
	char	*err_arg;

	if (dup2(io->fd_out, STDOUT_FILENO) == -1)
	{
		if (io->outfile)
			err_arg = io->outfile;
		else
			err_arg = "stdout";
		msg_error_cmd(cmd_name_for_err, err_arg, "dup2 failed", 1);
		g_exit_status = 1;
		return (false);
	}
	return (true);
}

/*
 * Gestiona la redirección de salida para un comando.
 * Cumple Norminette (líneas, ternario) manteniendo lógica original.
 */
bool	redir_handle_output(t_cmd *cmd, t_cmd_io *io, char *cmd_name_for_err)
{
	mode_t	mode;

	(void)cmd;
	if (!io)
		return (false);
	if (io->fd_out != -1)
	{
		if (!handle_existing_redir_out_fd(io, cmd_name_for_err))
			return (false);
	}
	else if (io->outfile)
	{
		mode = 0644;
		if (!open_and_dup_outfile(io, cmd_name_for_err, mode))
			return (false);
	}
	return (true);
}
