/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 02:40:33 by flperez-          #+#    #+#             */
/*   Updated: 2025/04/25 02:41:10 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/*
* Valida si 'full_path' es un archivo ejecutable accesible.
* No asigna cmd->path. No llama a msg_error_cmd ni set_exit_status.
* Devuelve un t_path_status detallando el resultado.
*/
static t_path_status	validate_path_access(const char *full_path)
{
	struct stat	statbuf;

	if (!full_path || full_path[0] == '\0')
		return (PATH_IS_EMPTY_CMD);
	if (stat(full_path, &statbuf) != 0)
	{
		if (errno == ENOENT || errno == ENOTDIR)
			return (PATH_ERR_NOT_FOUND);
		if (errno == EACCES)
			return (PATH_ERR_NO_PERMISSION);
		return (PATH_ERR_STAT_FAILED);
	}
	if (S_ISDIR(statbuf.st_mode))
		return (PATH_ERR_IS_DIRECTORY);
	if (access(full_path, X_OK) != 0)
	{
		if (errno == EACCES)
			return (PATH_ERR_NO_PERMISSION);
		if (errno == ENOENT)
			return (PATH_ERR_NOT_FOUND);
		return (PATH_ERR_ACCESS_FAILED);
	}
	return (PATH_FOUND_EXECUTABLE);
}

/*
* Intenta procesar 'cmd_name' como una ruta explícita (si contiene '/').
* Si es válida y ejecutable, asigna cmd->path.
* Devuelve un t_path_status.
*/
static t_path_status	attempt_explicit_path(t_cmd_exe *cmd, \
											const char *cmd_name)
{
	t_path_status	status;

	if (ft_strchr(cmd_name, '/') == NULL)
		return (PATH_NOT_EXPLICIT);
	status = validate_path_access(cmd_name);
	if (status == PATH_FOUND_EXECUTABLE)
	{
		free_ptr(cmd->path);
		cmd->path = NULL;
		cmd->path = ft_strdup(cmd_name);
		if (!cmd->path)
			return (PATH_ERR_MALLOC);
	}
	return (status);
}

/*
* Construye y valida una ruta para un comando en un directorio específico.
* Si es ejecutable, actualiza cmd->path.
*/
static t_path_status	check_dir_for_cmd(const char *dir, \
										const char *cmd_name, \
										t_cmd_exe *cmd)
{
	char			*current_full_path;
	t_path_status	status;
	char			*effective_dir;

	if (dir[0] == '\0')
		effective_dir = ".";
	else
		effective_dir = (char *)dir;
	current_full_path = ft_strjoin_path(effective_dir, cmd_name);
	if (!current_full_path)
		return (PATH_ERR_MALLOC);
	status = validate_path_access(current_full_path);
	if (status == PATH_FOUND_EXECUTABLE)
	{
		free_ptr(cmd->path);
		cmd->path = current_full_path;
	}
	else
	{
		free_ptr(current_full_path);
	}
	return (status);
}

/*
* Busca 'cmd_name' en los directorios de la variable PATH.
* Si encuentra un ejecutable válido, asigna cmd->path.
* Devuelve t_path_status.
*/
static t_path_status	attempt_path_env_search(t_cmd_exe *cmd, \
												const char *cmd_name, \
												t_data_env_exe *data)
{
	char			**paths_arr;
	int				i;
	t_path_status	final_status;
	char			*path_env_value;

	path_env_value = get_env_var_val(data->env_for_execve, "PATH");
	if (!path_env_value || path_env_value[0] == '\0')
		return (PATH_NOT_IN_ENV);
	paths_arr = ft_split(path_env_value, ':');
	if (!paths_arr)
		return (PATH_ERR_MALLOC);
	i = -1;
	final_status = PATH_NOT_IN_ENV;
	while (paths_arr[++i])
	{
		final_status = check_dir_for_cmd(paths_arr[i], cmd_name, cmd);
		if (final_status == PATH_FOUND_EXECUTABLE)
			break ;
		if (final_status == PATH_ERR_MALLOC)
			break ;
		if (final_status != PATH_FOUND_EXECUTABLE)
			final_status = PATH_NOT_IN_ENV;
	}
	free_str_tab(paths_arr);
	return (final_status);
}

/*
* Busca la ruta ejecutable de un comando.
* Orquesta la validación, búsqueda explícita y búsqueda en PATH.
* Delega la validacion en validate_basic_cmd_input y los mensajes de 
* error en =>  handle_path_search_erro.
*/
bool	find_command_path(t_cmd_exe *cmd, t_data_env_exe *data)
{
	char			*cmd_name_str;
	t_path_status	current_status;

	if (!validate_basic_cmd_input(cmd, &cmd_name_str))
		return (false);
	free_ptr(cmd->path);
	cmd->path = NULL;
	if (ft_strcmp(cmd_name_str, "..") == 0 && \
		ft_strchr(cmd_name_str, '/') == NULL)
	{
		current_status = validate_path_access("..");
	}
	else
	{
		current_status = attempt_explicit_path(cmd, cmd_name_str);
	}
	if (current_status == PATH_NOT_EXPLICIT)
	{
		current_status = attempt_path_env_search(cmd, cmd_name_str, data);
	}
	if (current_status == PATH_FOUND_EXECUTABLE)
		return (true);
	handle_path_search_error(current_status, cmd_name_str, data);
	return (false);
}
