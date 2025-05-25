/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 09:41:30 by flperez-          #+#    #+#             */
/*   Updated: 2025/03/15 14:07:52 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	cd_to_home(t_data_env *data)
{
	char	*path_home;

	path_home = get_env_var_val(data->env, "HOME");
	if (!path_home || *path_home == '\0' || ft_isspace(*path_home))
	{
		msg_error_cmd("cd", NULL, "HOME not set", EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	return (!perform_directory_change(data, path_home));
}

static int	cd_to_oldpwd(t_data_env *data)
{
	char	*path_oldpwd;

	path_oldpwd = get_env_var_val(data->env, "OLDPWD");
	if (!path_oldpwd)
	{
		msg_error_cmd("cd", NULL, "OLDPWD not set", EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	ft_putendl_fd(path_oldpwd, STDOUT_FILENO);
	return (!perform_directory_change(data, path_oldpwd));
}

static int	cd_handle_path_arg(char *path_arg, t_data_env *data)
{
	if (ft_strcmp(path_arg, ".") == 0)
		return (EXIT_SUCCESS);
	return (!perform_directory_change(data, path_arg));
}

/*
* builtin_cd:
* Implementa el comando `cd`.
* - Sin argumentos o con "--": Cambia al directorio HOME.
* - Con "-": Cambia al directorio OLDPWD y muestra la nueva ruta.
* - Con un argumento de ruta: Intenta cambiar a esa ruta.
* - Maneja errores como "too many arguments", HOME/OLDPWD no definidos.
*/
int	builtin_cd(char **args, t_data_env *data)
{
	if (!args[1] || ft_isspace(args[1][0])
		|| args[1][0] == '\0' || ft_strncmp(args[1], "--", 3) == 0)
	{
		return (cd_to_home(data));
	}
	if (args[2])
	{
		msg_error_cmd("cd", NULL, "too many arguments", EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	if (ft_strcmp(args[1], "-") == 0)
	{
		return (cd_to_oldpwd(data));
	}
	return (cd_handle_path_arg(args[1], data));
}
