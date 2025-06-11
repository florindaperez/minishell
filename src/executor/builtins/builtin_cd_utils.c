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
 * report_cd_error
 * Informa de un error ocurrido durante la llamada al sistema 'chdir'.
 * Normaliza ciertos errores (ESTALE a ENOENT) para mensajes más claros y
 * establece el estado de salida global a fallo.
 *
 * path: La ruta que no se pudo cambiar, para incluirla en el mensaje de error.
 *
 * Retorna: Siempre 'false', para ser usado convenientemente en la función que
 * lo llama para indicar un fallo en la operación.
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
 * perform_directory_change
 * Realiza el cambio de directorio de trabajo. Esta función orquesta la
 * llamada a 'chdir', la obtención de la nueva ruta absoluta y la
 * actualización de las variables de entorno PWD y OLDPWD.
 *
 * data: La estructura de datos del ejecutor que contiene las variables de
 * entorno a actualizar.
 * path: La ruta del directorio al que se desea cambiar.
 *
 * Retorna: 'true' si el cambio de directorio y la actualización del entorno
 * fueron exitosos, 'false' si alguna de las operaciones falla.
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
