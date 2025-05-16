/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 11:38:38 by flperez-          #+#    #+#             */
/*   Updated: 2025/03/13 11:39:11 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* 
 * builtin_env:Replicamos el comando env . Imprimir las variables de entorno
 * almacenadas en data->env.
 * 
*/
int	builtin_env(t_data_env *data, char **args)
{
	int	i;

	i = 0;
	if (args && args[1])
	{
		msg_error_cmd("env", NULL, "too many arguments", 2);
		return (2);
	}
	if (!data->env)
		return (EXIT_FAILURE);
	while (data->env[i])
		ft_putendl_fd(data->env[i++], STDOUT_FILENO);
	return (EXIT_SUCCESS);
}
