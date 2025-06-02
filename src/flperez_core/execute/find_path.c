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
* Valida si 'full_path' es un archivo ejecutable accesible.
* No asigna cmd->path. No llama a msg_error_cmd ni set_exit_status.
* Devuelve un t_path_status detallando el resultado.
*/
static t_path_status	validate_path_access(const char *full_path)
{
	struct stat	statbuf;

	if (!full_path || full_path[0] == '\0')
		return (PATH_IS_EMPTY_CMD); // Tratar path nulo o vacío como comando vacío
	if (stat(full_path, &statbuf) != 0)
	{
		if (errno == ENOENT || errno == ENOTDIR)
			return (PATH_ERR_NOT_FOUND);
		if (errno == EACCES) // Permiso denegado para acceder a un componente de la ruta
			return (PATH_ERR_NO_PERMISSION);
		return (PATH_ERR_STAT_FAILED); // Otro error de stat
	}
	if (S_ISDIR(statbuf.st_mode))
		return (PATH_ERR_IS_DIRECTORY);
	if (access(full_path, X_OK) != 0)
	{
		if (errno == EACCES)
			return (PATH_ERR_NO_PERMISSION);
		if (errno == ENOENT) // Desapareció entre stat y access
			return (PATH_ERR_NOT_FOUND);
		return (PATH_ERR_ACCESS_FAILED); // Otro error de access
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
		free_ptr(cmd->path); // Liberar path anterior si existiera
		cmd->path = ft_strdup(cmd_name);
		if (!cmd->path)
			return (PATH_ERR_MALLOC); // Fallo de strdup
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
	char			*current_path;
	int				i;
	t_path_status	current_status;
	char			*path_env_val;

	path_env_val = get_env_var_val(data->env_for_execve, "PATH");
	if (!path_env_val || path_env_val[0] == '\0')
		return (PATH_NOT_IN_ENV); // PATH no existe o está vacía
	paths_arr = ft_split(path_env_val, ':');
	if (!paths_arr)
		return (PATH_ERR_MALLOC); // Fallo de ft_split
	i = -1;
	current_status = PATH_NOT_IN_ENV; // Default si no se encuentra
	while (paths_arr[++i])
	{
		if (paths_arr[i][0] == '\0') // Componente vacío en PATH (ej. PATH=":/bin")
			current_path = ft_strjoin_path(".", cmd_name);
		else
			current_path = ft_strjoin_path(paths_arr[i], cmd_name);
		if (!current_path)
		{
			current_status = PATH_ERR_MALLOC; // Fallo ft_strjoin_path
			break ;
		}
		current_status = validate_path_access(current_path);
		if (current_status == PATH_FOUND_EXECUTABLE)
		{
			free_ptr(cmd->path);
			cmd->path = current_path; // Asigna, current_path ya está alocado
			break ;
		}
		free_ptr(current_path);
		if (current_status == PATH_ERR_MALLOC) // Si validate_path_access diera error de malloc
			break ;
		current_status = PATH_NOT_IN_ENV; // Reset para la siguiente iteración
	}
	free_str_tab(paths_arr);
	return (current_status);
}

/*
* Busca la ruta ejecutable de un comando.
* Primero verifica si es una ruta explícita, luego busca en PATH.
* Emite mensajes de error apropiados y establece el código de salida global.
* Almacena la ruta completa encontrada en cmd->path.
* Retorna true si la ruta es válida y se encuentra, false si no.
*/
bool	find_command_path(t_cmd_exe *cmd, t_data_env_exe *data)
{
	char			*cmd_name;
	t_path_status	status;

	if (!cmd || !cmd->args || !cmd->args[0] || !cmd->args[0]->value)
		return (msg_error_cmd("", NULL, "No such file or directory", 127), false);
	cmd_name = cmd->args[0]->value;
	if (cmd_name[0] == '\0') // Comando "" (cadena vacía)
		return (msg_error_cmd(cmd_name, NULL, "No such file or directory", 127), false);
	if (ft_strcmp(cmd_name, ".") == 0)
		return (msg_error_cmd(".", NULL, "filename argument required", 2), \
				ft_putendl_fd("minishell: .: usage: . filename [arguments]", 2), false);
	free_ptr(cmd->path);
	cmd->path = NULL;
	if (ft_strcmp(cmd_name, "..") == 0 && ft_strchr(cmd_name, '/') == NULL)
		status = validate_path_access(".."); // Chequeo especial para ".." como comando
	else
		status = attempt_explicit_path(cmd, cmd_name);
	if (status == PATH_NOT_EXPLICIT) // Si no fue explícita o ".." especial no resolvió
		status = attempt_path_env_search(cmd, cmd_name, data);
	if (status == PATH_FOUND_EXECUTABLE)
		return (true);
	if (status == PATH_ERR_IS_DIRECTORY)
		msg_error_cmd(cmd_name, NULL, "Is a directory", 126);
	else if (status == PATH_ERR_NO_PERMISSION || status == PATH_ERR_ACCESS_FAILED)
		msg_error_cmd(cmd_name, NULL, "Permission denied", 126);
	else if (status == PATH_ERR_MALLOC)
		msg_error_cmd(cmd_name, "memory error", strerror(errno), EXIT_FAILURE);
	else if (status == PATH_ERR_STAT_FAILED)
		msg_error_cmd(cmd_name, "stat error", strerror(errno), EXIT_FAILURE);
	else if (status == PATH_NOT_IN_ENV) // Fallo la búsqueda en PATH
	{
		char *path_val = get_env_var_val(data->env_for_execve, "PATH");
		if (!path_val || path_val[0] == '\0') // PATH no existía o estaba vacía
			msg_error_cmd(cmd_name, NULL, "No such file or directory", 127);
		else // PATH existía pero el comando no se encontró en ella
			msg_error_cmd(cmd_name, NULL, "command not found", 127);
	}
	else // Cubre PATH_ERR_NOT_FOUND (de ruta explícita) y PATH_IS_EMPTY_CMD
		msg_error_cmd(cmd_name, NULL, "No such file or directory", 127);
	return (false);
}