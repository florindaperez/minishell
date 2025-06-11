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
 * do_fork_and_pipe
 * Realiza fork. Si hay un siguiente comando en la pipeline, crea un pipe.
 * Maneja los errores que puedan ocurrir durante la creación del pipe o el fork,
 * cerrando los descriptores de fichero abiertos antes de terminar.
 *
 * p_state: Puntero al estado actual de la pipeline, que contiene los fds.
 *
 * Retorna: El PID del proceso hijo creado o termina el programa si falla.
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
 * child_exec_logic
 * Lógica que ejecuta el proceso hijo. Configura las señales, redirige los
 * descriptores de fichero para la comunicación entre pipes y llama a la
 * función que ejecutará el comando final.
 *
 * cmd:     El comando que el proceso hijo debe ejecutar.
 * data:    Puntero a la estructura de datos principal del ejecutor.
 * p_state: Puntero al estado actual de la pipeline.
 */
static void	child_exec_logic(t_cmd_exe *cmd, t_data_env_exe *data,
							t_pipeline_state_exe *p_state)
{
	int	out_fd;
	int	in_fd;

	signals_child_default();
	in_fd = *(p_state->prev_pipe_read_fd_ptr);
	out_fd = -1;
	if (p_state->is_next_command)
	{
		safe_close(&p_state->current_pipe_fds[0]);
		out_fd = p_state->current_pipe_fds[1];
	}
	execute_child_process(cmd, data, in_fd, out_fd);
	exit(EXIT_FAILURE);
}

/*
 * process_one_cmd
 * Procesa un único comando en la pipeline. Determina si hay un comando
 * siguiente, crea un nuevo proceso con fork y, si es el hijo, ejecuta su
 * lógica.
 *
 * cmd:     El comando actual a procesar.
 * data:    Puntero a la estructura de datos del ejecutor.
 * p_state: Puntero al estado de la pipeline.
 *
 * Retorna: El PID del proceso hijo creado.
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
 * exec_cmd_loop_and_set_status
 * Itera sobre los comandos, ejecutando cada uno en la pipeline. Gestiona los
 * descriptores de fichero en el proceso padre, espera a todos los hijos y
 * establece el estado de salida final.
 *
 * cmds_head: La cabeza de la lista de comandos a ejecutar.
 * data:      Puntero a la estructura de datos del ejecutor.
 * p_state:   Puntero al estado de la pipeline.
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
	else if (cmds_head && !cmds_head->next && cmds_head->redir_error
		&& !is_parent_builtin(cmds_head))
		data->last_exit_status = 1;
}

/*
 * execute_pipeline
 * Función principal para ejecutar una lista de comandos (pipeline). Gestiona
 * el caso especial de built-ins que deben ejecutarse en el proceso padre.
 * Si no, inicializa el estado y orquesta el bucle de ejecución de la pipeline.
 *
 * cmds: La cabeza de la lista de comandos a ejecutar.
 * data: Puntero a la estructura de datos del ejecutor.
 */
void	execute_pipeline(t_cmd_exe *cmds, t_data_env_exe *data)
{
	t_pipeline_state_exe	p_state;
	int						initial_prev_pipe_fd;
	int						status_builtin;

	if (cmds && !cmds->next && is_parent_builtin(cmds))
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
		return ;
	signals_execution();
	exec_cmd_loop_and_set_status(cmds, data, &p_state);
	signals_interactive();
}
