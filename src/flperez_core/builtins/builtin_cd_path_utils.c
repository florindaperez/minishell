/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_path_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 19:51:10 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/31 19:51:15 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h" 

/*
* char *build_cd_fallback_path(t_data_env_exe *data, char *path_arg)
* (Tu implementación de build_fallback_path renombrada y hecha no estática)
*/
static char	*build_cd_fallback_path(t_data_env_exe *data, char *path_arg)
{
	char	*new_abs_p;
	char	*temp_join;

	new_abs_p = NULL;
	temp_join = NULL;
	if (path_arg[0] == '/')
		new_abs_p = ft_strdup(path_arg);
	else if (data->pwd)
	{
		temp_join = ft_strjoin(data->pwd, "/");
		if (temp_join)
		{
			new_abs_p = ft_strjoin(temp_join, path_arg);
			free_ptr(temp_join);
		}
	}
	if (!new_abs_p && (path_arg[0] == '/' || data->pwd))
		msg_error_cmd("cd", "build_cd_fallback_path", \
			"memory allocation error in fallback", EXIT_FAILURE);
	return (new_abs_p);
}

/*
* char *get_cd_current_abs_path(t_data_env_exe *data, char *path_arg)
* (Tu implementación de get_current_abs_path renombrada y hecha no estática)
*/
char	*get_cd_current_abs_path(t_data_env_exe *data, char *path_arg)
{
	char	cwd_buffer[PATH_SIZE];
	char	*new_abs_p;

	new_abs_p = NULL;
	if (getcwd(cwd_buffer, PATH_SIZE) != NULL)
	{
		new_abs_p = ft_strdup(cwd_buffer);
		if (!new_abs_p)
			msg_error_cmd("cd", "get_cd_current_abs_path", \
				"memory allocation error", EXIT_FAILURE);
		return (new_abs_p);
	}
	msg_error_cmd("cd: error retrieving current directory", \
		"getcwd: cannot access parent directories", strerror(errno), errno);
	return (build_cd_fallback_path(data, path_arg));
}
