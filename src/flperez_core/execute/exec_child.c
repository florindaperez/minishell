/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 15:53:56 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/10 15:54:01 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"
/*
 * Configuración inicial del proceso hijo:
 * - Restaura el comportamiento por defecto de SIGINT y SIGQUIT.
 * - Si el parser marcó un error de redirección previo, sale con g_get_signal.
 */
static void	child_initial_setup(t_cmd_exe *cmd)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (cmd && cmd->redir_error)
	{
		exit(g_get_signal);
	}
}

/*
 * Maneja la redirección de entrada de archivo ('<') si es necesario.
 * Asume que redirect_io maneja la apertura y los errores.
 * Retorna true si la redirección fue exitosa o no era necesaria,
 * false si falló (redirect_io debería haber establecido g_get_signal).
 */
static bool	child_handle_file_input(t_cmd_exe *cmd)
{
	if (cmd && cmd->io && cmd->io->infile && cmd->io->fd_in == -1)
	{
		if (!redirect_io(cmd))
			return (false);
	}
	return (true);
}

/*
 * Intenta ejecutar un comando externo.
 * Busca el path si es necesario, valida el path y luego llama a execve.
 * Esta función no retorna si execve tiene éxito; siempre llama a exit()
 * en error.
 * Codigo fallo execve = 126
 */
static void	child_exec_external(t_cmd_exe *cmd, t_data_env_exe *data, char **argv)
{
	int	path_error_code;

	if (!cmd->path)
	{
		if (!find_command_path(cmd, data))
		{
			free_str_tab(argv);
			exit(g_get_signal);
		}
	}
	path_error_code = get_path_execution_errors(cmd->path, argv[0]);
	if (path_error_code != 0)
	{
		free_str_tab(argv);
		exit(path_error_code);
	}
	execve(cmd->path, argv, data->env_for_execve);
	path_error_code = 126;
	if (errno == ENOENT)
		path_error_code = 127;
	msg_error_cmd(cmd->path, NULL, strerror(errno), path_error_code);
	free_str_tab(argv);
	exit(path_error_code);
}

/*
 * Decide el tipo de comando (vacío, builtin, externo) y lo ejecuta.
 * Esta función siempre termina el proceso hijo.
 */
static void	execute_prepared_command(t_cmd_exe *cmd, t_data_env_exe *data, \
										char **argv_for_exec)
{
	int	builtin_exit_status;

	if (cmd->was_literal_empty_command
		|| (argv_for_exec[0] && argv_for_exec[0][0] == '\0'))
	{
		child_handle_empty_or_not_found(cmd, argv_for_exec);
	}
	if (!argv_for_exec[0])
	{
		free_str_tab(argv_for_exec);
		exit(EXIT_SUCCESS);
	}
	if (cmd->is_builtin)
	{
		builtin_exit_status = execute_builtin(cmd, data, argv_for_exec);
		free_str_tab(argv_for_exec);
		exit(builtin_exit_status);
	}
	child_exec_external(cmd, data, argv_for_exec);
}

/*
 *  Lógica principal ejecutada dentro del proceso hijo.
 *
 * Esta función orquesta la configuración de senales, manejo de E/S,
 * preparación de argumentos y la ejecución final del comando.
 * No debería retornar; siempre llama a exit().
 * @param cmd El comando a ejecutar.
 * @param data Los datos del entorno.
 * @param pipe_in_fd Descriptor de lectura del pipe de entrada (-1 si no hay).
 * @param pipe_out_fd Descriptor de escritura del pipe de salida (-1 si no hay).
 */
void	execute_child_process(t_cmd_exe *cmd, t_data_env_exe *data, \
								int pipe_in_fd, int pipe_out_fd)
{
	char	**argv_for_exec;
	char	*cmd_name_for_errors;

	if (cmd && cmd->args && cmd->args[0] && cmd->args[0]->value)
		cmd_name_for_errors = cmd->args[0]->value;
	else
		cmd_name_for_errors = "minishell"; // Fallback si no hay nombre de comando
	child_initial_setup(cmd); // 1. Configuración inicial (señales, error de redir del parser)
	// 2. Procesar y abrir archivos de redirección de ENTRADA
	//    Asumimos que child_handle_file_input abre el archivo, establece cmd->io->fd_in,
	//    y llama a msg_error_cmd/set_exit_status y devuelve false en error.
	if (cmd && cmd->io && (cmd->io->infile || cmd->io->heredoc_delimiter || cmd->io->fd_in != -1))
	{
		if (!child_handle_file_input(cmd)) // Pasarle cmd_name_for_errors si lo necesita
			exit(g_get_signal);
	}
	// 3. Procesar y abrir archivos de redirección de SALIDA
	//    Esto es crucial y es lo que añadiste basado en sugerencias anteriores.
	if (cmd && cmd->io && (cmd->io->outfile || cmd->io->fd_out != -1))
	{
		if (!redir_handle_output(cmd, cmd->io, cmd_name_for_errors))
			exit(g_get_signal); // Salir si redir_handle_output falló
	}
	// 4. Aplicar todas las redirecciones (pipes y archivos ya abiertos en fd_in/fd_out)
	//    setup_child_redirections ahora usará los fd_in/fd_out que se establecieron
	//    en los pasos 2 y 3 (o los fds de las tuberías si no hubo redirs de archivo).
	setup_child_redirections(cmd, pipe_in_fd, pipe_out_fd);
	// 5. Construir argv para execve
	argv_for_exec = build_argv_from_args(cmd->args);
	if (!argv_for_exec)
	{
		// build_argv_from_args debería llamar a msg_error_cmd y set_exit_status
		// en caso de fallo de malloc para que g_get_signal sea correcto.
		// Si no, el exit podría ser con un g_get_signal incorrecto.
		exit(g_get_signal); // O un EXIT_FAILURE genérico si g_get_signal no está garantizado.
	}
	// 6. Decidir y ejecutar el comando
	execute_prepared_command(cmd, data, argv_for_exec);
	// execute_prepared_command debería llamar a exit() siempre.
	// Esta línea es un fallback por si acaso.
	free_str_tab(argv_for_exec); // Liberar argv_for_exec si execute_prepared_command no lo hizo y falló en salir.
	exit(EXIT_FAILURE);
}
