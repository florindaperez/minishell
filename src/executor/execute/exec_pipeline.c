/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 15:50:51 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/10 15:50:59 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * Realiza fork. Si p_state->is_next_command es true, crea un pipe en
 * p_state->current_pipe_fds. Maneja errores. Retorna el PID.
 */
static pid_t	do_fork_and_pipe(t_pipeline_state_exe *p_state)
{
	pid_t	pid;

	if (p_state->is_next_command)
	{
		if (pipe(p_state->current_pipe_fds) == -1)
		{
			safe_close(p_state->prev_pipe_read_fd_ptr);
			perror_exit("minishell: pipe creation failed", EXIT_FAILURE);
		}
	}
	pid = fork();
	if (pid == -1)
	{
		if (p_state->is_next_command)
		{
			safe_close(&p_state->current_pipe_fds[0]);
			safe_close(&p_state->current_pipe_fds[1]);
		}
		safe_close(p_state->prev_pipe_read_fd_ptr);
		perror_exit("minishell: fork failed", EXIT_FAILURE);
	}
	return (pid);
}

/*
 * Lógica ejecutada por el proceso hijo. Configura FDs y ejecuta el comando.
 */
static void	child_exec_logic(t_cmd_exe *cmd, t_data_env_exe *data,
							t_pipeline_state_exe *p_state)
{
	int	out_fd_for_child;

	out_fd_for_child = -1;
	if (p_state->is_next_command)
	{
		safe_close(&p_state->current_pipe_fds[0]);
		out_fd_for_child = p_state->current_pipe_fds[1];
	}
	execute_child_process(cmd, data, *(p_state->prev_pipe_read_fd_ptr),
		out_fd_for_child);
	exit(EXIT_FAILURE);
}

/*
 * Procesa un único comando en la pipeline: forkea y organiza la ejecución.
 * Retorna el PID del hijo. Utiliza t_pipeline_state.
 */
static pid_t	process_one_cmd(t_cmd_exe *cmd, t_data_env_exe *data,
								t_pipeline_state_exe *p_state)
{
	pid_t	pid;

	p_state->is_next_command = (cmd->next != NULL);
	pid = do_fork_and_pipe(p_state);
	if (pid == 0)
	{
		child_exec_logic(cmd, data, p_state);
	}
	return (pid);
}

/*
 * Itera sobre los comandos, ejecutando cada uno en la pipeline,
 * cierra los FDs y establece el estado de salida final.
 */
static void	exec_cmd_loop_and_set_status(t_cmd_exe *cmds_head,
										t_data_env_exe *data,
										t_pipeline_state_exe *p_state)
{
	t_cmd_exe	*current_cmd;
	pid_t		pid;
	pid_t		last_pid;
	int			pipeline_status;

	current_cmd = cmds_head;
	last_pid = -1;
	pid = -1;
	while (current_cmd)
	{
		pid = process_one_cmd(current_cmd, data, p_state);
		parent_pipeline_handle_fds(p_state->current_pipe_fds,
			p_state->prev_pipe_read_fd_ptr, pid, &last_pid);
		current_cmd = current_cmd->next;
	}
	safe_close(p_state->prev_pipe_read_fd_ptr);
	if (last_pid != -1)
	{
		pipeline_status = wait_for_all_children(last_pid);
		data->last_exit_status = pipeline_status;
	}
	else if (cmds_head && !cmds_head->next && cmds_head->redir_error \
		&& !is_parent_builtin(cmds_head))
		data->last_exit_status = 1;
}

/*
 * Función principal para ejecutar una lista de comandos (pipeline).
 */
void	execute_pipeline(t_cmd_exe *cmds, t_data_env_exe *data)
{
	t_pipeline_state_exe	p_state;
	int						initial_prev_pipe_fd;
	int						status_builtin;

	if (cmds && cmds->next == NULL && is_parent_builtin(cmds))
	{
		status_builtin = execute_parent_builtin(cmds, data);
		data->last_exit_status = status_builtin;
		return ;
	}
	initial_prev_pipe_fd = -1;
	p_state.prev_pipe_read_fd_ptr = &initial_prev_pipe_fd;
	p_state.current_pipe_fds[0] = -1;
	p_state.current_pipe_fds[1] = -1;
	p_state.is_next_command = false;
	if (!handle_pipeline_preliminaries(cmds, data))
	{
		return ;
	}
	signals_noninteractive();
	exec_cmd_loop_and_set_status(cmds, data, &p_state);
	signals_interactive();
}
