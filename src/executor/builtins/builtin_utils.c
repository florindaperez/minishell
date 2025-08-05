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
#include "minishell_executor.h"

/* 
* get_env_var_val:
* Queremos el valor no la posicion de la 
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

	if (!env)
		return (NULL);
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
