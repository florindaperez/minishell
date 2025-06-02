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
#include "minishell_executor.h"
/*
* static bool	report_cd_error(char *path)
* Informa error de chdir y establece estado de salida.
*/
static bool	report_cd_error(char *path)
{
	if (errno == ESTALE)
		errno = ENOENT;
	msg_error_cmd("cd", path, strerror(errno), EXIT_FAILURE);
	set_exit_status(EXIT_FAILURE);
	return (false);
}

/*
* bool	perform_directory_change(t_data_env_exe *data, char *path)
* (Tu implementación, ahora llamando a las funciones no estáticas de otros utils)
*/
bool	perform_directory_change(t_data_env_exe *data, char *path)
{
	char	*new_abs_path;

	if (!path || !data)
		return (false);
	if (chdir(path) != 0)
		return (report_cd_error(path));
	new_abs_path = get_cd_current_abs_path(data, path);
	if (!new_abs_path)
	{
		set_exit_status(EXIT_FAILURE);
		return (false);
	}
	if (!update_all_pwd_vars(data, new_abs_path))
	{
		free_ptr(new_abs_path);
		return (false);
	}
	free_ptr(new_abs_path);
	set_exit_status(EXIT_SUCCESS);
	return (true);
}
