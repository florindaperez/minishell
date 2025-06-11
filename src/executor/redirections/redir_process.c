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
#include "minishell_executor.h"

/*
 * open_outfile_for_redir
 * Realiza la llamada 'open' para un fichero de salida, usando las banderas
 * y modo especificados. Centraliza la apertura y el manejo de errores
 * inmediatos de esta operación.
 *
 * io:               Puntero a la estructura de E/S con el nombre del fichero.
 * cmd_name_for_err: Nombre del comando actual, para mensajes de error.
 * open_flags:       Banderas para la función 'open' (ej. O_WRONLY).
 * mode:             Modo de creación del fichero si se usa O_CREAT.
 *
 * Retorna: El descriptor del fichero abierto, o -1 si 'open' falla.
 */
static int	open_outfile_for_redir(t_cmd_io_exe *io, char *cmd_name_for_err,
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
		g_get_signal = 1;
		return (-1);
	}
	return (fd);
}

/*
 * open_and_dup_outfile
 * Orquesta la redirección de salida a un fichero. Asegura que la ruta del
 * directorio exista, determina las banderas de apertura (truncar o añadir),
 * abre el fichero y duplica su descriptor a STDOUT_FILENO.
 *
 * io:               Puntero a la estructura de E/S con la información de la
 * redirección.
 * cmd_name_for_err: Nombre del comando para mensajes de error.
 * mode:             Modo de creación del fichero si no existe.
 *
 * Retorna: 'true' si todas las operaciones son exitosas, 'false' si alguna
 * falla.
 */
static bool	open_and_dup_outfile(t_cmd_io_exe *io, char *cmd_name_for_err,
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
 * redir_handle_input
 * Gestiona la redirección de la entrada estándar (STDIN_FILENO). Decide si
 * usar un descriptor de fichero ya existente (de un pipe) o si abrir un
 * nuevo fichero (de una redirección '<').
 *
 * cmd:              El comando actual (no usado directamente en esta versión).
 * io:               La estructura de E/S con la información de entrada.
 * cmd_name_for_err: Nombre del comando para mensajes de error.
 *
 * Retorna: 'true' si la redirección de entrada se configura correctamente,
 * 'false' en caso de error.
 */
bool	redir_handle_input(t_cmd_exe *cmd, t_cmd_io_exe *io, \
							char *cmd_name_for_err)
{
	(void)cmd;
	if (!io)
		return (false);
	if (io->fd_in != -1)
	{
		if (dup2(io->fd_in, STDIN_FILENO) == -1)
		{
			msg_error_cmd(cmd_name_for_err, "stdin", "dup2 failed", 1);
			g_get_signal = 1;
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
 * handle_existing_redir_out_fd
 * Duplica un descriptor de fichero de salida ya existente (ej. de un pipe)
 * a STDOUT_FILENO. Es una función auxiliar para 'redir_handle_output'.
 *
 * io:               La estructura de E/S que contiene el 'fd_out'.
 * cmd_name_for_err: Nombre del comando para mensajes de error.
 *
 * Retorna: 'true' si 'dup2' tiene éxito, 'false' si falla.
 */
static bool	handle_existing_redir_out_fd(t_cmd_io_exe *io, \
										char *cmd_name_for_err)
{
	char	*err_arg;

	if (dup2(io->fd_out, STDOUT_FILENO) == -1)
	{
		if (io->outfile)
			err_arg = io->outfile;
		else
			err_arg = "stdout";
		msg_error_cmd(cmd_name_for_err, err_arg, "dup2 failed", 1);
		g_get_signal = 1;
		return (false);
	}
	return (true);
}

/*
 * redir_handle_output
 * Gestiona la redirección de la salida estándar (STDOUT_FILENO). Decide si
 * usar un descriptor ya existente (de un pipe) o si abrir/crear un nuevo
 * fichero (de una redirección '>' o '>>').
 *
 * cmd:              El comando actual (no usado directamente en esta versión).
 * io:               La estructura de E/S con la información de salida.
 * cmd_name_for_err: Nombre del comando para mensajes de error.
 *
 * Retorna: 'true' si la redirección de salida se configura correctamente,
 * 'false' en caso de error.
 */
bool	redir_handle_output(t_cmd_exe *cmd, t_cmd_io_exe *io, \
							char *cmd_name_for_err)
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
