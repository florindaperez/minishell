/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_exec.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:21:05 by flperez-          #+#    #+#             */
/*   Updated: 2025/06/03 16:21:08 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"
#include "minishell_executor.h"

/*
 * init_and_convert_data
 * Prepara los datos para la ejecución del pipeline actual.
 * NO inicializa toda la estructura, solo los datos del comando actual.
 * Asume que 'data->shell_env_list_ptr' ya está configurado.
 */
static int	init_and_convert_data(t_cmd *cmd_list, t_data_env_exe *data)
{
	data->last_exit_status = g_get_signal;
	data->cmds_head = convert_cmd_list_to_cms_list_exec(cmd_list);
	if (data->cmds_head == NULL && cmd_list != NULL)
	{
		write(2, "minishell: Error during command translation\n", 44);
		return (1);
	}
	if (data->env_for_execve)
		free_arr2d(data->env_for_execve);
	if (data->shell_env_list_ptr && *(data->shell_env_list_ptr))
		data->env_for_execve = \
			convert_env_list_to_exec_envp(*(data->shell_env_list_ptr));
	else
		data->env_for_execve = NULL;
	if (!data->env_for_execve && data->shell_env_list_ptr
		&& *(data->shell_env_list_ptr) && data->cmds_head)
	{
		write(2, "minishell: Error converting environment\n", 40);
		return (1);
	}
	return (0);
}

/*
 * cleanup_exec_data
 * Libera la memoria temporal alocada para un pipeline.
 * NO libera pwd, old_pwd ni la lista de entorno principal.
 */
static void	cleanup_exec_data(t_data_env_exe *data)
{
	if (data->cmds_head)
	{
		free_cmd_list(data->cmds_head);
		data->cmds_head = NULL;
	}
}

/*
 * execute_pipeline_logic
 * Prepara y ejecuta el pipeline de comandos.
 * 'data' es ahora un puntero a una estructura persistente.
 * Por problemas de leaks se pasa a declarar envlist_ptr una sola vez en
 * data
 */
void	execute_pipeline_logic(t_cmd *cmd_list, t_data_env_exe *data)
{
	if (init_and_convert_data(cmd_list, data) != 0)
	{
		cleanup_exec_data(data);
		return ;
	}
	if (data->cmds_head)
	{
		execute_pipeline(data->cmds_head, data);
		g_get_signal = data->last_exit_status;
	}
	cleanup_exec_data(data);
}
