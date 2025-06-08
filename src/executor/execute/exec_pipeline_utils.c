/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 14:54:05 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/11 14:54:08 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * Tareas del padre después de un fork en el bucle de la pipeline.
 * Cierra los descriptores de pipe apropiados y actualiza prev_pipe_fd.
 */
void	parent_pipeline_handle_fds(int *pipe_fds, int *prev_pipe_fd_ptr,
									pid_t current_pid,
									pid_t *last_spawned_pid_ptr)
{
	if (last_spawned_pid_ptr)
		*last_spawned_pid_ptr = current_pid;
	safe_close(&pipe_fds[1]);
	safe_close(prev_pipe_fd_ptr);
	*prev_pipe_fd_ptr = pipe_fds[0];
}

/*
 * Obtiene el estado de salida del proceso hijo especificado.
 * Retorna el código de salida procesado.
 */
int	get_specific_child_exit_status(pid_t child_pid)
{
	int	status;
	int	exit_val;

	exit_val = EXIT_SUCCESS;
	if (child_pid <= 0)
		return (exit_val);
	if (waitpid(child_pid, &status, 0) == child_pid)
	{
		if (WIFEXITED(status))
			exit_val = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exit_val = 128 + WTERMSIG(status);
	}
	else if (errno != ECHILD)
	{
		perror("minishell: waitpid failed for specific child");
		exit_val = EXIT_FAILURE;
	}
	return (exit_val);
}

/*
 * Maneja casos especiales: sin comandos o si es un builtin del padre.
 * Retorna true si la ejecución de la pipeline debe continuar, false si no.
 */
bool	handle_pipeline_preliminaries(t_cmd_exe *cmds, t_data_env_exe *data)
{
	if (!cmds)
	{
		g_get_signal = 0;
		return (false);
	}
	if (!cmds->next && is_parent_builtin(cmds))
	{
		if (cmds->redir_error)
		{
			g_get_signal = 1;
			if (cmds->io)
			{
				safe_close(&cmds->io->fd_in);
				safe_close(&cmds->io->fd_out);
			}
		}
		else
			g_get_signal = execute_parent_builtin(cmds, data);
		return (false);
	}
	return (true);
}

/*
* wait_for_all_children
*
* Espera a que todos los procesos hijos terminen.
* Primero, espera de forma bloqueante al último proceso del pipeline
* para obtener su estado de salida. Luego, entra en un bucle para
* recoger (reap) a todos los demás hijos que pudieran haber quedado
* como zombies, manejando las interrupciones por señales.
*/
int	wait_for_all_children(pid_t last_pid)
{
	int		status;
	int		exit_code;
	pid_t	waited_pid;

	exit_code = EXIT_SUCCESS;
	if (last_pid > 0)
	{
		waitpid(last_pid, &status, 0);
		if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exit_code = 128 + WTERMSIG(status);
	}
	while (1)
	{
		waited_pid = wait(NULL);
		if (waited_pid == -1)
		{
			if (errno == ECHILD || errno == EINTR)
				break ;
		}
	}
	return (exit_code);
}
