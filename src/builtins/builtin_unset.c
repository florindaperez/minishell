/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 16:48:15 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/23 16:48:22 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

/*
*	process_unset_argument:
*	Procesa un único argumento para el comando 'unset'.
*	Valida que el argumento no contenga '=' (lo que sería un identificador no
*	válido para 'unset'). Si es válido, intenta eliminar la variable de la
*	lista de entorno interna y también del entorno del proceso actual.
*	Un argumento vacío se ignora silenciosamente (éxito).
*	Retorna EXIT_SUCCESS si el procesamiento es exitoso o si el argumento
*	es inválido de una manera que 'unset' ignora (como una cadena vacía).
*	Retorna EXIT_FAILURE si el argumento es un identificador no válido
*	(ej. "VAR=val").
*/
static int	process_unset_argument(const char *arg, t_env **env)
{
	char	*key_to_delete;

	if (ft_strchr(arg, '=') != NULL)
	{
		msg_error_cmd("unset", (char *)arg, \
			"not a valid identifier\n", EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	if (arg[0] == '\0')
	{
		return (EXIT_SUCCESS);
	}
	key_to_delete = (char *)arg;
	env_delone(env, &key_to_delete, free);
	unsetenv(arg);
	return (EXIT_SUCCESS);
}

/*
*	builtin_unset:
*	Implementa el comando 'unset' para eliminar variables de entorno.
*	Itera sobre cada argumento proporcionado a 'unset'. Si un argumento
*	es inválido (ej. contiene '='), 'unset' reporta un error para ese
*	argumento y establece su estado de salida general a 1, pero continúa
*	procesando los demás argumentos. Si todos los argumentos son válidos
*	(o no hay argumentos), el estado de salida es 0.
*/
int	builtin_unset(t_cmd *cmd, t_env **env)
{
	int	i;
	int	individual_arg_status;
	int	overall_builtin_status;

	i = 1;
	overall_builtin_status = EXIT_SUCCESS;
	if (cmd->commands[i] == NULL)
	{
		set_exit_status(EXIT_SUCCESS);
		return (EXIT_SUCCESS);
	}
	while (cmd->commands[i] != NULL)
	{
		individual_arg_status = process_unset_argument(cmd->commands[i], env);
		if (individual_arg_status == EXIT_FAILURE)
		{
			overall_builtin_status = EXIT_FAILURE;
		}
		i++;
	}
	set_exit_status(overall_builtin_status);
	return (overall_builtin_status);
}
