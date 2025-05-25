/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 10:03:35 by flperez-          #+#    #+#             */
/*   Updated: 2025/02/28 13:37:40 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* Divide "KEY=VALUE" en un array de dos strings: {KEY, VALUE}.
* Retorna NULL en caso de error de alocación o si no hay '='.
*/
static char	**get_key_value_pair(char *arg)
{
	char	**pair;
	char	*equal_sign_pos;
	size_t	key_len;

	equal_sign_pos = ft_strchr(arg, '=');
	if (!equal_sign_pos)
		return (NULL);
	pair = (char **)malloc(sizeof(char *) * 3);
	if (!pair)
		return (NULL);
	key_len = equal_sign_pos - arg;
	pair[0] = ft_substr(arg, 0, key_len);
	pair[1] = ft_substr(arg, key_len + 1, ft_strlen(equal_sign_pos + 1));
	pair[2] = NULL;
	if (!pair[0] || !pair[1])
	{
		free_str_tab(pair);
		return (NULL);
	}
	return (pair);
}

/*
* Maneja un argumento de export que contiene "=".
*/
static int	handle_export_with_equals(char *arg, t_data_env *data)
{
	char	**key_val_pair;

	key_val_pair = get_key_value_pair(arg);
	if (!key_val_pair || !is_valid_env_var_key(key_val_pair[0]))
	{
		msg_error_cmd("export", arg, "not a valid identifier", 1);
		free_str_tab(key_val_pair);
		return (EXIT_FAILURE);
	}
	if (!add_env_var(data, key_val_pair[0], key_val_pair[1]))
	{
		free_str_tab(key_val_pair);
		return (EXIT_FAILURE);
	}
	free_str_tab(key_val_pair);
	return (EXIT_SUCCESS);
}

/*
* Procesa un único argumento para el comando export.
* Valida el argumento y lo anade al entorno si es válido.
*/
static int	process_export_argument(char *arg, t_data_env *data)
{
	if (ft_strchr(arg, '=') == NULL)
	{
		if (!is_valid_env_var_key(arg))
		{
			msg_error_cmd("export", arg, "not a valid identifier", 1);
			return (EXIT_FAILURE);
		}
		if (!add_env_var(data, arg, NULL))
			return (EXIT_FAILURE);
	}
	else
	{
		return (handle_export_with_equals(arg, data));
	}
	return (EXIT_SUCCESS);
}

/*
* builtin_export: Agrega variables al entorno.
* Si no hay argumentos, se comporta como 'env' (imprime variables).
*/
int	builtin_export(char **args, t_data_env *data)
{
	int	i;
	int	overall_ret_status;

	overall_ret_status = EXIT_SUCCESS;
	i = 1;
	if (!args[i])
		return (builtin_env(data, args));
	while (args && args[i])
	{
		if (process_export_argument(args[i], data) == EXIT_FAILURE)
			overall_ret_status = EXIT_FAILURE;
		i++;
	}
	return (overall_ret_status);
}
