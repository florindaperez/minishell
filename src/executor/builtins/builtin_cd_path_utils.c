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
 * build_cd_fallback_path
 * Construye una ruta absoluta para 'cd' como método de respaldo cuando
 * 'getcwd' falla. Lo hace uniendo manualmente la variable PWD almacenada
 * con el argumento de ruta relativo proporcionado.
 *
 * data:     La estructura de datos del ejecutor que contiene la variable PWD.
 * path_arg: El argumento de ruta original pasado al comando 'cd'.
 *
 * Retorna: Una nueva cadena de caracteres con la ruta absoluta construida,
 * o NULL si falla la asignación de memoria.
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
 * get_cd_current_abs_path
 * Obtiene la ruta de trabajo absoluta actual, principalmente después de un
 * cambio de directorio exitoso. Intenta usar 'getcwd' como método primario
 * y recurre a 'build_cd_fallback_path' si 'getcwd' falla.
 *
 * data:     La estructura de datos del ejecutor, para pasarla a la función
 * de respaldo si es necesario.
 * path_arg: El argumento de ruta original de 'cd', para la función de
 * respaldo.
 *
 * Retorna: Una nueva cadena de caracteres con la ruta absoluta, o NULL si
 * ambos métodos, el primario y el de respaldo, fallan.
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
