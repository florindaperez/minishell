/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_update_env.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 19:54:25 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/31 19:54:36 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
* static void	local_update_environment_pwds(...)
* Actualiza PWD y OLDPWD en la lista t_env* principal.
*/
static void	local_update_environment_pwds(t_data_env_exe *data, \
											char *new_dir_path)
{
	char	*current_pwd_val_from_list;

	current_pwd_val_from_list = get_env_val("PWD", \
										*(data->shell_env_list_ptr));
	if (current_pwd_val_from_list)
		add_or_update_env_list_var(data->shell_env_list_ptr, "OLDPWD", \
									current_pwd_val_from_list);
	else if (data->pwd)
		add_or_update_env_list_var(data->shell_env_list_ptr, "OLDPWD", \
									data->pwd);
	free_ptr(current_pwd_val_from_list);
	add_or_update_env_list_var(data->shell_env_list_ptr, "PWD", new_dir_path);
}

/*
* static bool	local_update_internal_caches(...)
* Actualiza los cachés data->pwd y data->old_pwd.
*/
static bool	local_update_internal_caches(t_data_env_exe *data, \
											char *new_dir_path)
{
	free_ptr(data->old_pwd);
	if (data->pwd)
		data->old_pwd = ft_strdup(data->pwd);
	else
		data->old_pwd = NULL;
	if (data->pwd && !data->old_pwd)
	{
		msg_error_cmd("cd", "update_caches", \
			"memory error for OLDPWD cache", EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (false);
	}
	free_ptr(data->pwd);
	data->pwd = ft_strdup(new_dir_path);
	if (!data->pwd && new_dir_path != NULL)
	{
		msg_error_cmd("cd", "update_caches", \
			"memory error for PWD cache", EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (false);
	}
	return (true);
}

/*
* bool	update_all_pwd_vars(...)
* Orquesta la actualización de todas las variables PWD/OLDPWD.
* Asume que add_or_update_env_list_var y convert_env_list_to_exec_envp
* están correctamente definidas y gestionan errores de memoria.
*/
bool	update_all_pwd_vars(t_data_env_exe *data, char *new_dir_path)
{
	if (!data || !data->shell_env_list_ptr)
	{
		return (false);
	}
	local_update_environment_pwds(data, new_dir_path);
	if (!local_update_internal_caches(data, new_dir_path))
		return (false);
	free_arr2d(data->env_for_execve);
	data->env_for_execve = NULL;
	if (*(data->shell_env_list_ptr))
	{
		data->env_for_execve = \
			convert_env_list_to_exec_envp(*(data->shell_env_list_ptr));
		if (!data->env_for_execve && *(data->shell_env_list_ptr))
		{
			msg_error_cmd("cd", "(internal)", "env array update failed", 1);
			return (false);
		}
	}
	return (true);
}
