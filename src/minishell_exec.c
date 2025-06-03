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

/* Variable global para el estado de salida (definida en main.c) */
extern int	g_get_signal;

/*
 * Prepara las estructuras de datos necesarias para la ejecución de comandos.
 * Convierte la lista comandos del parser y el entorno al formato del ejecutor.
 * p_cmd_list: Lista de comandos generada por el parser.
 * env_l: Lista de variables de entorno actual.
 * data: Puntero a la estructura t_data_env_exe que se poblará.
 * Retorna: 0 en caso de éxito.
 * 1 si hay error en la traducción de comandos.
 * 2 si hay error en la conversión del entorno.
 * Los errores también actualizan g_get_signal mediante set_exit_status.
 * Esta función es estática.
 */
static int	prepare_execution_data(t_cmd *p_cmd_list, t_env *env_l,
	t_data_env_exe *data)
{
	data->cmds_head = convert_cmd_list_to_cms_list_exec(p_cmd_list);
	if (env_l)
		data->env_for_execve = convert_env_list_to_exec_envp(env_l);
	else
		data->env_for_execve = NULL;
	if (data->cmds_head == NULL && p_cmd_list != NULL)
	{
		write(STDERR_FILENO, "minishell: error en traducción de cmd\n", 38);
		set_exit_status(EXIT_FAILURE);
		return (1);
	}
	if (!data->env_for_execve && env_l && data->cmds_head)
	{
		write(STDERR_FILENO, "minishell: error en conversión de env\n", 38);
		set_exit_status(EXIT_FAILURE);
		return (2);
	}
	return (0);
}

/*
 * Función pública de este módulo: Ejecuta una pipeline de comandos.
 * Toma la lista de comandos parseada, la prepara para ejecución,
 * la ejecuta y luego limpia los recursos utilizados.
 * cmd_list_parse: La lista de comandos (t_cmd *) obtenida del parser.
 * envlist_head_ptr: Puntero al puntero de la cabeza de la lista de entorno.
 * Permite modificar el entorno (ej. para builtins como export).
 * data_storage: Puntero a una estructura t_data_env_exe que se usará
 * internamente para la ejecución.
 * Esta función será declarada en minishell.h y llamada desde minishell.c.
 */
void	execute_prepared_commands(t_cmd *cmd_list_parse,
	t_env **envlist_head_ptr, t_data_env_exe *data_storage)
{
	int	preparation_status;

	if (!cmd_list_parse)
		return ;
	ft_memset(data_storage, 0, sizeof(t_data_env_exe));
	data_storage->shell_env_list_ptr = envlist_head_ptr;
	data_storage->last_exit_status = g_get_signal;
	preparation_status = prepare_execution_data(cmd_list_parse,
			*envlist_head_ptr, data_storage);
	if (preparation_status == 0 && data_storage->cmds_head)
	{
		execute_pipeline(data_storage->cmds_head, data_storage);
		g_get_signal = data_storage->last_exit_status;
	}
	free_cmd_list(data_storage->cmds_head);
	data_storage->cmds_head = NULL;
	free_arr2d(data_storage->env_for_execve);
	data_storage->env_for_execve = NULL;
}
