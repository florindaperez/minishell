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
 * Abre un archivo de salida especificado en io->outfile con las banderas
 * y modo proporcionados. Esta función se centra únicamente en la operación
 * de apertura y el manejo de su error inmediato. No realiza la duplicación
 * del descriptor de archivo.
 * Es una función auxiliar para open_and_dup_outfile, extraída para ayudar
 * a cumplir con las restricciones de longitud de línea de norminette.
 *
 * @param io Puntero a la estructura t_cmd_io_exe que contiene el nombre
 * del archivo de salida (io->outfile).
 * @param cmd_name_for_err Nombre del comando actual, para usar en mensajes
 * de error.
 * @param open_flags Banderas para la función open (ej. O_WRONLY | O_CREAT).
 * @param mode Modo de creación del archivo si O_CREAT está en open_flags.
 * @return El descriptor de archivo del archivo abierto en caso de éxito.
 * -1 en caso de error en la apertura (y establece g_get_signal en 1).
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
 * Gestiona la apertura y duplicación de un archivo de salida.
 * Primero, asegura que la ruta del directorio para el archivo de salida exista
 * (llamando a redir_create_path_if_needed). Luego, determina las banderas
 * de apertura correctas (O_APPEND si io->append_mode es true, o O_TRUNC
 * en caso contrario). Abre el archivo usando la función auxiliar
 * open_outfile_for_redir. Si la apertura es exitosa, el descriptor
 * obtenido se almacena en io->fd_out y se duplica a STDOUT_FILENO.
 * Maneja errores en cada paso.
 *
 * @param io Puntero a la estructura t_cmd_io_exe que contiene la información
 * de la redirección de salida (outfile, append_mode, fd_out).
 * @param cmd_name_for_err Nombre del comando actual para mensajes de error.
 * @param mode Modo de creación del archivo si este no existe.
 * @return true si todas las operaciones (creación de path, apertura, dup2)
 * son exitosas.
 * false si alguna operación falla (y establece g_get_signal en 1).
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
 * Gestiona la redirección de la entrada estándar (STDIN_FILENO) para un comando.
 * Si io->fd_in ya tiene un descriptor de archivo válido (distinto de -1),
 * este se duplica directamente a STDIN_FILENO. Esto ocurre, por ejemplo,
 * cuando la entrada proviene de un pipe.
 * Si io->fd_in es -1 pero io->infile está definido, significa que la entrada
 * debe leerse de un archivo. En este caso, se llama a la función
 * open_and_dup_infile (que se asume definida en otro lugar) para que abra
 * el archivo especificado en io->infile y duplique su descriptor a STDIN_FILENO.
 *
 * @param cmd Puntero al comando actual (t_cmd_exe). No se usa directamente
 * en esta versión de la función, pero se mantiene por consistencia
 * con la firma original o por si se necesitara en el futuro.
 * @param io Puntero a la estructura t_cmd_io_exe que contiene la información
 * de la redirección de entrada (fd_in, infile).
 * @param cmd_name_for_err Nombre del comando actual para mensajes de error.
 * @return true si la redirección de entrada se configura correctamente.
 * false si ocurre un error (ej. fallo en dup2 o en open_and_dup_infile),
 * y establece g_get_signal en 1.
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
 * Función auxiliar estática para manejar la duplicación de un descriptor
 * de archivo de salida (io->fd_out) que ya ha sido abierto o establecido
 * previamente (por ejemplo, el extremo de escritura de un pipe).
 * Duplica io->fd_out a STDOUT_FILENO.
 *
 * @param io Puntero a la estructura t_cmd_io_exe que contiene el descriptor
 * de archivo de salida (io->fd_out) y opcionalmente el nombre
 * del archivo de salida (io->outfile) para mensajes de error.
 * @param cmd_name_for_err Nombre del comando actual para mensajes de error.
 * @return true si dup2 tiene éxito.
 * false si dup2 falla (y establece g_get_signal en 1).
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
 * Gestiona la redirección de la salida estándar (STDOUT_FILENO) para un comando.
 * Si io->fd_out ya tiene un descriptor de archivo válido (distinto de -1),
 * se llama a handle_existing_redir_out_fd para duplicarlo a STDOUT_FILENO.
 * Esto ocurre, por ejemplo, cuando la salida se dirige a un pipe.
 * Si io->fd_out es -1 pero io->outfile está definido, significa que la salida
 * debe escribirse en un archivo. En este caso, se llama a la función
 * open_and_dup_outfile para que cree/abra el archivo especificado en
 * io->outfile (considerando el modo de apendizaje io->append_mode) y
 * duplique su descriptor a STDOUT_FILENO.
 *
 * @param cmd Puntero al comando actual (t_cmd_exe). No se usa directamente
 * en esta versión de la función.
 * @param io Puntero a la estructura t_cmd_io_exe que contiene la información
 * de la redirección de salida (fd_out, outfile, append_mode).
 * @param cmd_name_for_err Nombre del comando actual para mensajes de error.
 * @return true si la redirección de salida se configura correctamente.
 * false si ocurre un error (y establece g_get_signal en 1).
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
