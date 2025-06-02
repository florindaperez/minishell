/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_to_exec_args.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 13:03:19 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/29 13:03:22 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * Crea un único elemento t_arg_info_exe a partir de una cadena de comando.
 * Asigna memoria para la estructura y duplica la cadena del comando.
 * Establece q_status a NO_QUOTE_EXEC.
 *
 * @param command_str Cadena de caracteres que representa el argumento.
 * @return Un puntero al nuevo t_arg_info_exe, o NULL si falla la asignación.
 */
static t_arg_info_exe	*create_arg_info_element(char *command_str)
{
	t_arg_info_exe	*arg_info;

	arg_info = (t_arg_info_exe *)p_malloc(sizeof(t_arg_info_exe));
	if (!arg_info)
		return (NULL);
	arg_info->value = ft_strdup(command_str);
	if (!arg_info->value && command_str)
	{
		free(arg_info);
		return (NULL);
	}
	arg_info->q_status = NO_QUOTE_EXEC;
	return (arg_info);
}

/*
 * Traduce la estructura de argumentos de un comando del formato del parser
 * (un array de strings char **) al formato utilizado por el ejecutor
 * (un array de punteros a estructuras t_arg_info_exe).
 *
 * @param parser_commands Array de strings (char **) que representa los
 * argumentos del comando en el formato del parser, terminado en NULL.
 * @return Un array de punteros a t_arg_info_exe (t_arg_info_exe **) terminado
 * en NULL, o NULL si ocurre un error de asignación de memoria.
 */
t_arg_info_exe	**translate_parser_args_to_exec_args(char **parser_commands)
{
	t_arg_info_exe	**exec_args;
	int				arg_count;
	int				i;

	arg_count = 0;
	if (parser_commands)
		while (parser_commands[arg_count])
			arg_count++;
	exec_args = (t_arg_info_exe **)p_malloc(sizeof(t_arg_info_exe *) \
		* (arg_count + 1));
	if (!exec_args)
		return (NULL);
	i = 0;
	while (i < arg_count)
	{
		exec_args[i] = create_arg_info_element(parser_commands[i]);
		if (!exec_args[i])
		{
			free_arg_info_array(exec_args);
			return (NULL);
		}
		i++;
	}
	exec_args[i] = NULL;
	return (exec_args);
}
