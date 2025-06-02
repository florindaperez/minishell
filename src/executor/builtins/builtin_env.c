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
#include "minishell_executor.h"

/* 
 * builtin_env:Replicamos el comando env . Imprimir las variables de entorno
 * almacenadas en data->env.
 * 
*/
int	builtin_env(t_data_env_exe *data, char **args)
{
	int	i;

	i = 0;
	if (args && args[1])
	{
		msg_error_cmd("env", NULL, "too many arguments", 2);
		return (2);
	}
	if (!data->env_for_execve)
		return (EXIT_FAILURE);
	while (data->env_for_execve[i])
		ft_putendl_fd(data->env_for_execve[i++], STDOUT_FILENO);
	return (EXIT_SUCCESS);
}
