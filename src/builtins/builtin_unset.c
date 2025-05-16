/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 19:39:13 by flperez-          #+#    #+#             */
/*   Updated: 2025/02/28 11:16:08 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* Elimina una variable del array de entorno a partir de su 'index_to_remove'.
* Desplaza los elementos subsiguientes para llenar el vacío.
* El array original data->env es modificado directamente.
*/
static bool	remove_env_var(t_data_env *data, int index_to_remove)
{
	int	i;
	int	current_env_size;

	if (!data || !data->env)
		return (false);
	current_env_size = num_var_env(data->env);
	if (index_to_remove < 0 || index_to_remove >= current_env_size)
		return (false);
	free_ptr(data->env[index_to_remove]);
	data->env[index_to_remove] = NULL;
	i = index_to_remove;
	while (i < current_env_size - 1)
	{
		data->env[i] = data->env[i + 1];
		i++;
	}
	data->env[current_env_size - 1] = NULL;
	return (true);
}

/*
* Procesa un único argumento para el comando unset.
* Valida el argumento y lo elimina del entorno si es válido y existe.
* Devuelve EXIT_SUCCESS o EXIT_FAILURE.
*/
static int	process_unset_argument(char *arg, t_data_env *data)
{
	int	var_id;

	if (!is_valid_env_var_key(arg) || ft_strchr(arg, '='))
	{
		msg_error_cmd("unset", arg, "not a valid identifier", 1);
		return (EXIT_FAILURE);
	}
	else
	{
		var_id = get_env_var_id(data->env, arg);
		if (var_id != -1)
		{
			if (!remove_env_var(data, var_id))
			{
				return (EXIT_FAILURE);
			}
		}
	}
	return (EXIT_SUCCESS);
}

/*
* builtin_unset: Elimina variables de entorno.
* Itera sobre los argumentos y procesa cada uno.
* Retorna EXIT_FAILURE si algún argumento es inválido, de lo contrario SUCCESS.
*/
int	builtin_unset(char **args, t_data_env *data)
{
	int	i;
	int	overall_ret_status;

	overall_ret_status = EXIT_SUCCESS;
	i = 1;
	while (args && args[i])
	{
		if (process_unset_argument(args[i], data) == EXIT_FAILURE)
			overall_ret_status = EXIT_FAILURE;
		i++;
	}
	return (overall_ret_status);
}
