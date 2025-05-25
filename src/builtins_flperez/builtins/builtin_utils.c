/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 10:12:24 by flperez-          #+#    #+#             */
/*   Updated: 2025/02/28 13:07:57 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* 
*	num_var_env: Devolvemos el numero de variables de entorno que hay.
*/
int	num_var_env(char **env)
{
	int	i;

	i = 0;
	while (env && env[i])
		i++;
	return (i);
}

/*
*  get_env_var_id: Buscamos si la variable de entorno existe en "env"
* Si la encontramos retornamos su id en caso contrario
* "-1".El array de variables de entorno lo pasa Alba ?. 
* al igual que "key".
*/
int	get_env_var_id(char **env, char *key)
{
	int		i;
	char	*tmp;

	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (-1);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(tmp, env[i], ft_strlen(tmp)) == 0)
		{
			free_ptr(tmp);
			return (i);
		}
		i++;
	}
	free_ptr(tmp);
	return (-1);
}

/* 
* get_env_var_val:
* A diferencia de "get_env_var_id" queremos el valor no la posicion de la 
* variable de entorno, por lo que necesitaremos memoria y luego liberarla
* para evitar leaks.
* Devuelve un puntero al valor de la variable de entorno si la encuentra, 
* en caso contrario NULL.
*
*/

char	*get_env_var_val(char **env, char *val)
{
	int		i;
	char	*tmp;

	tmp = ft_strjoin(val, "=");
	if (!tmp)
		return (NULL);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(tmp, env[i], ft_strlen(tmp)) == 0)
		{
			free_ptr(tmp);
			return (ft_strchr(env[i], '=') + 1);
		}
		i++;
	}
	free_ptr(tmp);
	return (NULL);
}

/**
 * get_env_var:
 *  Obtiene el valor de una variable de entorno del array envp.
 *  Busca la variable 'var_name' en el array 'envp'.
 * Si la encuentra, devuelve un puntero al valor (la parte después del '=').
 * Si no la encuentra, devuelve NULL.
 * envp = Array de cadenas del entorno (ej: {"PATH=/bin", "USER=test", NULL}).
 * var_name = Nombre de la variable a buscar (ej: "PATH").
 */
char	*get_env_var(char **envp, const char *var_name)
{
	int		i;
	size_t	name_len;

	if (!envp || !var_name)
		return (NULL);
	name_len = ft_strlen(var_name);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(var_name, envp[i], name_len) == 0 \
			&& envp[i][name_len] == '=')
		{
			return (envp[i] + name_len + 1);
		}
		i++;
	}
	return (NULL);
}
