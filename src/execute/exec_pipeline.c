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

/*
 * Realiza fork. Si p_state->is_next_command es true, crea un pipe en
 * p_state->current_pipe_fds. Maneja errores. Retorna el PID.
 */
static pid_t	do_fork_and_pipe(t_pipeline_state *p_state)
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
static void	child_exec_logic(t_cmd *cmd, t_data_env *data,
							t_pipeline_state *p_state)
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
static pid_t	process_one_cmd(t_cmd *cmd, t_data_env *data,
								t_pipeline_state *p_state)
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
 * Itera sobre los comandos, ejecutando cada uno en la pipeline.
 * Retorna el PID del último comando. Utiliza t_pipeline_state.
 */
static pid_t	exec_cmd_loop(t_cmd *cmds, t_data_env *data,
								t_pipeline_state *p_state_base)
{
	t_cmd	*current_cmd;
	pid_t	pid;
	pid_t	last_pid;

	current_cmd = cmds;
	last_pid = -1;
	while (current_cmd)
	{
		pid = process_one_cmd(current_cmd, data, p_state_base);
		parent_pipeline_handle_fds(p_state_base->current_pipe_fds,
			p_state_base->prev_pipe_read_fd_ptr, pid, &last_pid);
		current_cmd = current_cmd->next;
	}
	return (last_pid);
}

/*
 * Función principal para ejecutar una lista de comandos (pipeline).
 */
void	execute_pipeline(t_cmd *cmds, t_data_env *data)
{
	pid_t				last_pid_in_pipeline;
	t_pipeline_state	p_state;
	int					initial_prev_pipe_fd;

	initial_prev_pipe_fd = -1;
	p_state.prev_pipe_read_fd_ptr = &initial_prev_pipe_fd;
	p_state.current_pipe_fds[0] = -1;
	p_state.current_pipe_fds[1] = -1;
	p_state.is_next_command = false;
	if (!handle_pipeline_preliminaries(cmds, data))
		return ;
	signals_noninteractive();
	last_pid_in_pipeline = exec_cmd_loop(cmds, data, &p_state);
	safe_close(p_state.prev_pipe_read_fd_ptr);
	if (last_pid_in_pipeline != -1)
		g_exit_status = wait_for_all_children(last_pid_in_pipeline);
	else
	{
		if (cmds && !cmds->next && cmds->redir_error
			&& !is_parent_builtin(cmds))
			g_exit_status = 1;
	}
	signals_interactive();
}
