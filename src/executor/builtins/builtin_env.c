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
* builtin_env
*
* Replica el comportamiento del comando 'env'.
* Si no tiene argumentos, imprime la lista de variables de entorno.
* Si tiene argumentos, simula el error de 'bash', que trata el argumento
* como un comando a ejecutar, fallando con "No such file or directory"
* y código de salida 127.
*/
int	builtin_env(t_data_env_exe *data, char **args)
{
	t_env	*current;

	if (args[1] != NULL)
	{
		msg_error_cmd("env: ", args[1], ": No such file or directory", 127);
		return (127);
	}
	current = *(data->shell_env_list_ptr);
	while (current)
	{
		if (current->val)
		{
			ft_putstr_fd(current->key, STDOUT_FILENO);
			ft_putchar_fd('=', STDOUT_FILENO);
			ft_putendl_fd(current->val, STDOUT_FILENO);
		}
		current = current->next;
	}
	return (EXIT_SUCCESS);
}
