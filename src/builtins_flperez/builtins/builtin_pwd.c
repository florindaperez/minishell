/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 13:48:12 by flperez-          #+#    #+#             */
/*   Updated: 2025/02/20 13:59:39 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
*	pwd_builtin: Replicamos el comando pwd.  Muestra la ruta del directorio de 
*	donde estamos, como siempre si todo fue OK=> EXIT_SUCCESS en caso contrario
*	NOOK =>EXIT_FAILURE, se ha producido un error.
*/

int	builtin_pwd(t_data_env *data)
{
	char	buf[MAX_INPUT];
	char	*cwd;

	if (data->pwd)
	{
		ft_putendl_fd(data->pwd, STDOUT_FILENO);
		return (EXIT_SUCCESS);
	}
	cwd = getcwd(buf, MAX_INPUT);
	if (cwd)
	{
		ft_putendl_fd(cwd, STDOUT_FILENO);
		return (EXIT_SUCCESS);
	}
	msg_error_cmd("pwd", NULL, strerror(errno), errno);
	return (EXIT_FAILURE);
}
