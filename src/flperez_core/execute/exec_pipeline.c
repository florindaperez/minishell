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

#include "minishell.h"         // Primero, para definir t_cmd, t_env, etc.
#include "minishell_executor.h" // Luego, para usar t_cmd en sus prototipos y definir t_cmd_exe, etc.

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
 * Itera sobre los comandos, ejecutando cada uno en la pipeline.
 * Retorna el PID del último comando. Utiliza t_pipeline_state.
 */
static pid_t	exec_cmd_loop(t_cmd_exe *cmds, t_data_env_exe *data,
								t_pipeline_state_exe *p_state_base)
{
	t_cmd_exe	*current_cmd;
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
void    execute_pipeline(t_cmd_exe *cmds, t_data_env_exe *data)
{
    pid_t               last_pid_in_pipeline;
    t_pipeline_state_exe    p_state;
    int                 initial_prev_pipe_fd;

    // INICIO DE SECCIÓN MODIFICADA / AÑADIDA
    // 1. Manejar builtins del padre si es un comando simple (sin pipes)
    if (cmds && cmds->next == NULL && is_parent_builtin(cmds)) //
    {
        // Es un comando simple y un builtin que debe ejecutarse en el proceso padre.
        // La función execute_parent_builtin ya maneja las redirecciones necesarias.
        int status = execute_parent_builtin(cmds, data); //
        data->last_exit_status = status; // ACTUALIZAR el estado para que minishell.c lo use.

        // No necesitamos configurar pipes, ni hacer fork, ni esperar hijos.
        // Las señales interactivas se restaurarán al final de prepare_and_execute_flperez
        // o al inicio del siguiente bucle en minishell.c.
        return; // Salir de execute_pipeline ya que el comando se ha ejecutado.
    }
    // FIN DE SECCIÓN MODIFICADA / AÑADIDA

    initial_prev_pipe_fd = -1;
    p_state.prev_pipe_read_fd_ptr = &initial_prev_pipe_fd;
    p_state.current_pipe_fds[0] = -1;
    p_state.current_pipe_fds[1] = -1;
    p_state.is_next_command = false; // Se establecerá en process_one_cmd

    // handle_pipeline_preliminaries: podría manejar heredocs u otras preparaciones.
    // Si falla (ej. Ctrl+C en heredoc), debería establecer data->last_exit_status
    // y retornar false.
    if (!handle_pipeline_preliminaries(cmds, data))
    {
        // Asumimos que handle_pipeline_preliminaries ya ha establecido
        // data->last_exit_status con el código de error apropiado (ej. 1 o 130).
        return;
    }
	signals_noninteractive(); // Cambiar manejadores de señales para ejecución de hijos.

    last_pid_in_pipeline = exec_cmd_loop(cmds, data, &p_state);

    safe_close(p_state.prev_pipe_read_fd_ptr); // Cerrar el último pipe de lectura si quedó abierto.

    if (last_pid_in_pipeline != -1)
    {
        // wait_for_all_children espera a todos los hijos y devuelve
        // el estado de salida del último comando en el pipeline.
        int pipeline_status = wait_for_all_children(last_pid_in_pipeline);
        data->last_exit_status = pipeline_status; // ACTUALIZAR el estado.
    }
    else // No se ejecutó ningún comando en un proceso hijo.
    {
        // Esto puede ocurrir si handle_pipeline_preliminaries tuvo éxito pero
        // la lista de comandos 'cmds' estaba vacía (raro), o si hubo un error
        // de redirección en un comando simple que *no* es un parent_builtin
        // y que no se manejó antes.
        // La lógica original para redir_error en un comando no builtin:
        if (cmds && !cmds->next && cmds->redir_error && !is_parent_builtin(cmds)) //
        {
            // El g_get_signal global podría haber sido establecido por el error de redirección.
            // Sincronizamos data->last_exit_status. O simplemente ponemos 1.
            data->last_exit_status = 1; // Típicamente, un error de redirección es 1.
                                        // O data->last_exit_status = g_get_signal; si confías que ya tiene el valor.
        }
        // Si no hubo PIDs Y no fue un error de redirección conocido aquí,
        // data->last_exit_status podría ya tener un valor de un paso previo
        // (ej. de handle_pipeline_preliminaries) o debería reflejar que no se hizo nada
        // (lo cual es manejado por `prepare_and_execute_flperez` que inicializa
        // `data_flp.last_exit_status` con el `g_get_signal` anterior).
    }
    signals_interactive(); // Restaurar manejadores de señales interactivos.
}
/*
 * Función principal para ejecutar una lista de comandos (pipeline).
 *
void	execute_pipeline(t_cmd_exe *cmds, t_data_env_exe *data)
{
	pid_t				last_pid_in_pipeline;
	t_pipeline_state_exe	p_state;
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
		g_get_signal = wait_for_all_children(last_pid_in_pipeline);
	else
	{
		if (cmds && !cmds->next && cmds->redir_error
			&& !is_parent_builtin(cmds))
			g_get_signal = 1;
	}
	signals_interactive();
}
*/