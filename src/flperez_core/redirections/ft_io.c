/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_io.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 13:43:30 by flperez-          #+#    #+#             */
/*   Updated: 2025/04/10 15:53:56 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * ft_io.c:
 * Este archivo contiene las funciones principales para la gestión de
 * entrada/salida (E/S) en minishell. Orquesta el proceso de redirección
 * y restauración de los descriptores de fichero estándar.
 */

/*
 * Inicializa los campos de la estructura t_cmd_io a sus valores por defecto
 * (NULL o -1 para punteros y descriptores de fichero, false para booleanos).
 *
 * io: Puntero a la estructura t_cmd_io que se va a inicializar.
 */
void	init_io_fds(t_cmd_io *io)
{
	if (!io)
		return ;
	io->infile = NULL;
	io->outfile = NULL;
	io->heredoc_delimiter = NULL;
	io->heredoc_quotes = false;
	io->append_mode = false;
	io->fd_in = -1;
	io->fd_out = -1;
	io->stdin_backup = -1;
	io->stdout_backup = -1;
}

/*
 * Orquesta el proceso de redirección de E/S para un comando dado.
 * Realiza copias de seguridad de los descriptores estándar, luego aplica
 * las redirecciones de entrada y salida según se especifique en cmd->io.
 * Utiliza funciones auxiliares de redir_utils.c y redir_process.c.
 *
 * cmd: Puntero a la estructura del comando (t_cmd) que contiene la
 * información de E/S en cmd->io.
 *
 * Retorna: true si todas las redirecciones fueron exitosas.
 * false si ocurrió algún error durante el proceso.
 * En caso de error, intenta restaurar los descriptores afectados.
 */
bool	redirect_io(t_cmd *cmd)
{
	t_cmd_io	*io;
	char		*cmd_name_for_error;

	if (!cmd || !cmd->io)
		return (false);
	io = cmd->io;
	cmd_name_for_error = redir_determine_cmd_name(cmd);
	if (!redir_backup_fds(io))
		return (false);
	if (!redir_handle_input(cmd, io, cmd_name_for_error))
	{
		redir_restore_fds_on_fail(io, RESTORE_STDIN_ONLY);
		return (false);
	}
	if (!redir_handle_output(cmd, io, cmd_name_for_error))
	{
		redir_restore_fds_on_fail(io, RESTORE_BOTH);
		return (false);
	}
	return (true);
}

/*
 * Función auxiliar estática para restaurar STDIN y cerrar el fd de entrada.
 * Es llamada por restore_io.
 *
 * io: Puntero a la estructura t_cmd_io.
 * success_ptr: Puntero a un booleano que se establece a false si dup2 falla.
 */
static void	process_restore_stdin(t_cmd_io *io, bool *success_ptr)
{
	if (io->stdin_backup != -1)
	{
		if (dup2(io->stdin_backup, STDIN_FILENO) == -1)
		{
			perror("minishell: ft_io: restore_io: dup2 restore stdin");
			*success_ptr = false;
		}
		close(io->stdin_backup);
		io->stdin_backup = -1;
	}
	if (io->fd_in != -1)
	{
		close(io->fd_in);
		io->fd_in = -1;
	}
}

/*
 * Función auxiliar estática para restaurar STDOUT y cerrar el fd de salida.
 * Es llamada por restore_io.
 *
 * io: Puntero a la estructura t_cmd_io.
 * success_ptr: Puntero a un booleano que se establece a false si dup2 falla.
 */
static void	process_restore_stdout(t_cmd_io *io, bool *success_ptr)
{
	if (io->stdout_backup != -1)
	{
		if (dup2(io->stdout_backup, STDOUT_FILENO) == -1)
		{
			perror("minishell: ft_io: restore_io: dup2 restore stdout");
			*success_ptr = false;
		}
		close(io->stdout_backup);
		io->stdout_backup = -1;
	}
	if (io->fd_out != -1)
	{
		close(io->fd_out);
		io->fd_out = -1;
	}
}

/*
 * Restaura los descriptores de fichero estándar (STDIN_FILENO, STDOUT_FILENO)
 * a partir de las copias de seguridad guardadas en la estructura io.
 * También cierra los descriptores de fichero que se abrieron para las
 * redirecciones (io->fd_in, io->fd_out).
 *
 * io: Puntero a la estructura t_cmd_io que contiene los backups y fds.
 *
 * Retorna: true si la restauración fue completamente exitosa.
 * false si alguna llamada a dup2(2) falló durante la restauración.
 */
bool	restore_io(t_cmd_io *io)
{
	bool	success;

	success = true;
	if (!io)
		return (true);
	process_restore_stdin(io, &success);
	process_restore_stdout(io, &success);
	return (success);
}
