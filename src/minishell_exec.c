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
 * Inicializa la estructura de datos del ejecutor y convierte las listas
 * de comandos y entorno a los formatos requeridos para la ejecución.
 *
 * cmd_list: Cabeza de la lista de comandos (t_cmd *) del parseador.
 * envlist_ptr: Puntero al puntero de la lista de entorno principal.
 * data: Puntero a la estructura de datos del ejecutor (t_data_env_exe *).
 *
 * Retorna: 0 en éxito, 1 si hay un error de traducción o conversión.
 */
static int	init_and_convert_data(t_cmd *cmd_list, t_env **envlist_ptr,
				t_data_env_exe *data)
{
	ft_memset(data, 0, sizeof(t_data_env_exe));
	data->shell_env_list_ptr = envlist_ptr;
	data->last_exit_status = g_get_signal;
	data->cmds_head = convert_cmd_list_to_cms_list_exec(cmd_list);
	if (data->cmds_head == NULL && cmd_list != NULL)
	{
		write(2, "minishell: Error during command translation\n", 44);
		return (1);
	}
	if (data->shell_env_list_ptr && *(data->shell_env_list_ptr))
		data->env_for_execve
			= convert_env_list_to_exec_envp(*(data->shell_env_list_ptr));
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
 * Libera la memoria alocada para las estructuras de comandos del ejecutor
 * y el array de entorno para execve.
 *
 * data: Puntero a la estructura de datos del ejecutor (t_data_env_exe *).
 */
static void	cleanup_exec_data(t_data_env_exe *data)
{
	if (data->cmds_head)
	{
		free_cmd_list(data->cmds_head);
		data->cmds_head = NULL;
	}
	if (data->env_for_execve)
	{
		free_arr2d(data->env_for_execve);
		data->env_for_execve = NULL;
	}
}

/*
 * execute_pipeline_logic
 * Prepara y ejecuta el pipeline de comandos.
 *
 * cmd_list: Cabeza de la lista de comandos (t_cmd *) parseados.
 * envlist_ptr: Puntero al puntero de la cabeza de la lista de entorno principal.
 * data: Puntero a la estructura de datos del ejecutor (t_data_env_exe *).
 */
void	execute_pipeline_logic(t_cmd *cmd_list, t_env **envlist_ptr,
			t_data_env_exe *data)
{
	if (init_and_convert_data(cmd_list, envlist_ptr, data) != 0)
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
