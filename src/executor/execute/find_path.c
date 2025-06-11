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

/*
 * validate_path_access
 * Valida si una ruta completa ('full_path') corresponde a un fichero
 * ejecutable y accesible. No modifica el comando ni muestra errores.
 *
 * full_path: La ruta completa del fichero a validar.
 *
 * Retorna: Un estado 't_path_status' que detalla el resultado: si se
 * encontró y es ejecutable, si es un directorio, si no existe, si hay
 * problemas de permisos, o si falló la llamada a 'stat'.
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
 * attempt_explicit_path
 * Intenta procesar el nombre de un comando como una ruta explícita (si
 * contiene '/'). Si la ruta es válida y ejecutable, actualiza 'cmd->path'.
 *
 * cmd:      El comando cuyo 'path' se actualizará si la ruta es válida.
 * cmd_name: El nombre del comando, que se evalúa como una ruta.
 *
 * Retorna: Un estado 't_path_status' indicando el resultado. Si no contiene
 * '/', retorna 'PATH_NOT_EXPLICIT' para que se intente otra estrategia.
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
 * check_dir_for_cmd
 * Construye y valida una ruta completa para un comando dentro de un
 * directorio específico. Si es ejecutable, actualiza 'cmd->path'.
 *
 * dir:      El directorio en el que se buscará el comando.
 * cmd_name: El nombre del comando a buscar.
 * cmd:      El comando cuyo 'path' se actualizará si se encuentra.
 *
 * Retorna: Un 't_path_status' con el resultado de la validación.
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
 * attempt_path_env_search
 * Busca un comando recorriendo los directorios listados en la variable de
 * entorno PATH. Si encuentra un ejecutable válido, actualiza 'cmd->path'.
 *
 * cmd:      El comando cuyo 'path' se actualizará si se encuentra.
 * cmd_name: El nombre del comando a buscar en los directorios del PATH.
 * data:     La estructura de datos del ejecutor para acceder al entorno.
 *
 * Retorna: 'PATH_FOUND_EXECUTABLE' si se encuentra, o un estado de error
 * si no se encuentra o si ocurre un fallo de memoria.
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
 * find_command_path
 * Orquesta la búsqueda de la ruta ejecutable para un comando. Sigue el orden
 * de búsqueda estándar: validación básica, ruta explícita, y finalmente
 * búsqueda en los directorios de la variable PATH.
 *
 * cmd:  El comando para el cual se busca la ruta. El campo 'cmd->path' será
 * actualizado si se encuentra una ruta válida.
 * data: La estructura de datos del ejecutor que contiene el entorno.
 *
 * Retorna: 'true' si se encuentra una ruta ejecutable, 'false' en caso
 * contrario. En caso de fallo, delega la impresión de errores a
 * 'handle_path_search_error'.
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
