/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_command_args.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 17:33:03 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/06 17:36:34 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* Libera la memoria de un array de strings hasta un cierto punto y 
* el array mismo.
* Usado para la limpieza en caso de error durante la expansión.
*/
static void	cleanup_allocated_args(char **args, int count)
{
	int	k;

	k = 0;
	while (k < count)
	{
		free_ptr(args[k]);
		k++;
	}
	free(args);
}

/*
* Cuenta el número de argumentos en un array de strings terminado en NULL.
*/
static int	count_arguments(char **raw_args)
{
	int	count;

	count = 0;
	while (raw_args[count])
		count++;
	return (count);
}

/*
* Expande cada argumento individualmente y los almacena en el nuevo array.
* Devuelve true si todo tiene éxito, false si ocurre un error.
*/
static bool	perform_args_expansion(char **expanded_args, char **raw_args,
									int arg_count, t_data_env *data_env)
{
	int	i;

	i = 0;
	while (i < arg_count)
	{
		expanded_args[i] = expand_argument(raw_args[i], NO_QUOTE, data_env);
		if (!expanded_args[i])
		{
			msg_error_cmd("expand_args", "malloc failed (single arg)", \
				strerror(errno), 1);
			cleanup_allocated_args(expanded_args, i);
			return (false);
		}
		i++;
	}
	expanded_args[i] = NULL;
	return (true);
}

/*
 * Expande variables y quita comillas para un array de strings crudos.
 * Crea y devuelve un NUEVO array de argumentos (char **).
 * El array original (raw_args) NO se modifica.
 * El array devuelto debe ser liberado por el llamador usando free_str_tab.
 *
 * raw_args => El array de argumentos crudos (char **).
 * data_env => Puntero a los datos del shell para la expansión.
 * return => Un nuevo array (char **) con los argumentos expandidos, 
 * 			o NULL error.
 */
char	**expand_command_args(char **raw_args, t_data_env *data_env)
{
	int		arg_count;
	char	**expanded_args;

	if (!raw_args)
		return (NULL);
	arg_count = count_arguments(raw_args);
	expanded_args = (char **)malloc(sizeof(char *) * (arg_count + 1));
	if (!expanded_args)
	{
		msg_error_cmd("expand_args", "malloc failed (args array)", \
			strerror(errno), 1);
		return (NULL);
	}
	if (!perform_args_expansion(expanded_args, raw_args, arg_count, data_env))
	{
		return (NULL);
	}
	return (expanded_args);
}
