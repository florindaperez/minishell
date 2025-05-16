/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_data_env.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 19:57:57 by flperez-          #+#    #+#             */
/*   Updated: 2025/03/06 20:09:19 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

/* start_env:
* Reservamos memoria para almacenar una copia del entorno.
* Copia cada variable de entorno en data->env.
* En caso e error devolvemos "false".
*/
static bool	start_env(t_data_env *data, char **env)
{
	int		i;

	data->env = ft_calloc(num_var_env(env) + 1, sizeof * data->env);
	if (!data->env)
		return (false);
	i = 0;
	while (env[i])
	{
		data->env[i] = ft_strdup(env[i]);
		if (!data->env[i])
			return (false);
		i++;
	}
	return (true);
}

/* start_pwd:
* Obtenemos el directorio de trabajo actual con getcwd().
* Guardamos esta ruta en data->pwd.
* Si la variable OLDPWD existe en el entorno, la copia en cao contrario la
* inicializamos con el directorio actual.
* En caso de error devolvemos "false"
*/

static bool	start_pwd(t_data_env *data)
{
	char	buff[MAX_INPUT];
	char	*w_dir;

	w_dir = getcwd(buff, MAX_INPUT);
	data->pwd = ft_strdup(w_dir);
	if (!data->pwd)
		return (false);
	if (get_env_var_id(data->env, "OLDPWD") != -1)
	{
		data->old_pwd = ft_strdup(get_env_var_val(data->env, "OLDPWD"));
		if (!data->old_pwd)
			return (false);
	}
	else
	{
		data->old_pwd = ft_strdup(w_dir);
		if (!data->old_pwd)
			return (false);
	}
	return (true);
}

/* start_data:
* Realizamos la inicialización del entorno en nuestra minishell. Almacenamos 
* las variables de entorno heredadas del shell original, y configuramos 
* el directorio de trabajo actual (PWD) y el anterior (OLDPWD).
*/

bool	start_data(t_data_env *data, char **env)
{
	if (!start_env(data, env))
	{
		msg_error_cmd("Fatal", NULL, "Could not initialize environment", 1);
		return (false);
	}
	if (!start_pwd(data))
	{
		msg_error_cmd("Fatal", NULL, "Could not initialize working directories",
			1);
		return (false);
	}
	data->cmd = NULL;
	g_exit_status = 0;
	return (true);
}
