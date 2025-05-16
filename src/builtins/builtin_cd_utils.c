/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_cd_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 16:21:54 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/09 16:21:57 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_pwd_vars(t_data_env *data, char *new_dir_path)
{
	char	*current_pwd_val;

	current_pwd_val = get_env_var_val(data->env, "PWD");
	if (current_pwd_val)
		add_env_var(data, "OLDPWD", current_pwd_val);
	else if (data->pwd)
		add_env_var(data, "OLDPWD", data->pwd);
	add_env_var(data, "PWD", new_dir_path);
	free_ptr(data->old_pwd);
	if (data->pwd)
		data->old_pwd = ft_strdup(data->pwd);
	else
		data->old_pwd = NULL;
	free_ptr(data->pwd);
	data->pwd = ft_strdup(new_dir_path);
	if (!data->pwd && new_dir_path)
		msg_error_cmd("cd", "update_pwd_vars", "strdup error PWD", 1);
	if (!data->old_pwd && current_pwd_val && data->pwd)
		msg_error_cmd("cd", "update_pwd_vars", "strdup error OLDPWD", 1);
}

static bool	report_chdir_error(char *path)
{
	if (errno == ESTALE)
		errno = ENOENT;
	msg_error_cmd("cd", path, strerror(errno), errno);
	g_exit_status = 1;
	return (false);
}

/*
* Intenta obtener la ruta absoluta del directorio actual.
* Si getcwd falla, intenta construirla manualmente.
* Retorna la ruta absoluta alocada, o NULL en caso de error grave.
*/
static char	*get_current_abs_path(t_data_env *data, char *path_arg)
{
	char	cwd_buffer[MAX_INPUT];
	char	*new_abs_path_val;
	char	*temp_join;

	new_abs_path_val = getcwd(cwd_buffer, MAX_INPUT);
	if (!new_abs_path_val)
	{
		msg_error_cmd("cd: error retrieving current directory", \
			"getcwd: cannot access parent directories", strerror(errno), errno);
		if (path_arg[0] == '/')
			return (ft_strdup(path_arg));
		else
		{
			if (!data->pwd)
				return (NULL);
			temp_join = ft_strjoin(data->pwd, "/");
			if (!temp_join)
				return (NULL);
			new_abs_path_val = ft_strjoin(temp_join, path_arg);
			free_ptr(temp_join);
			return (new_abs_path_val);
		}
	}
	return (ft_strdup(cwd_buffer));
}

/*
* Intenta cambiar el directorio actual al 'path' especificado.
* Actualiza las variables PWD y OLDPWD si tiene éxito.
*/
bool	perform_directory_change(t_data_env *data, char *path)
{
	char	*new_abs_path;

	if (chdir(path) != 0)
		return (report_chdir_error(path));
	new_abs_path = get_current_abs_path(data, path);
	if (!new_abs_path)
	{
		msg_error_cmd("cd", "perform_directory_change", \
			"could not determine new PWD", 1);
		g_exit_status = 1;
		return (false);
	}
	update_pwd_vars(data, new_abs_path);
	free_ptr(new_abs_path);
	g_exit_status = 0;
	return (true);
}
