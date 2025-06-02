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
static void	child_exec_external(t_cmd_exe *cmd, t_data_env_exe *data,
								char **argv)
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
	int		builtin_exit_status;
	bool	is_empty_cmd;

	is_empty_cmd = false;
	if (cmd->was_literal_empty_command)
		is_empty_cmd = true;
	else if (argv_for_exec[0] && argv_for_exec[0][0] == '\0')
		is_empty_cmd = true;
	if (is_empty_cmd)
		child_handle_empty_or_not_found(cmd, argv_for_exec);
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
 * Lógica principal ejecutada dentro del proceso hijo.
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

	cmd_name_for_errors = "minishell";
	if (cmd && cmd->args && cmd->args[0] && cmd->args[0]->value)
		cmd_name_for_errors = cmd->args[0]->value;
	child_initial_setup(cmd);
	if (cmd && cmd->io && (cmd->io->infile || cmd->io->heredoc_delimiter \
		|| cmd->io->fd_in != -1))
	{
		if (!child_handle_file_input(cmd))
			exit(g_get_signal);
	}
	if (cmd && cmd->io && (cmd->io->outfile || cmd->io->fd_out != -1))
	{
		if (!redir_handle_output(cmd, cmd->io, cmd_name_for_errors))
			exit(g_get_signal);
	}
	setup_child_redirections(cmd, pipe_in_fd, pipe_out_fd);
	argv_for_exec = build_argv_from_args(cmd->args);
	if (!argv_for_exec)
		exit(g_get_signal);
	execute_prepared_command(cmd, data, argv_for_exec);
}
