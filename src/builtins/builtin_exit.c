/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 13:04:13 by flperez-          #+#    #+#             */
/*   Updated: 2025/03/13 13:09:30 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* Determina si el shell debe terminar.
*/
static bool	should_terminate_shell(t_data_env *data)
{
	return (!data->cmd || (!data->cmd->next && !data->cmd->prev));
}

/*
* Procesa un único argumento para 'exit', determinando si es numérico
* y calculando el código de salida.
*/
static void	process_single_exit_arg(char *arg_str, int *exit_code_val,
										bool *is_err)
{
	long long	parsed_ll;

	*is_err = false;
	parsed_ll = ft_atoll_with_error_check(arg_str, is_err);
	if (*is_err)
	{
		msg_error_cmd("exit", arg_str, "numeric argument required", 2);
		*exit_code_val = 2;
	}
	else
	{
		*exit_code_val = (int)(parsed_ll & 0xFF);
	}
}

/*
* Maneja la lógica de salida cuando se proporcionan argumentos a 'exit'.
* Devuelve el código de estado final y determina si se debe abortar
* una terminación del shell (ej., en caso de "too many arguments").
*/
static int	handle_exit_args(char **args, int arg_count,
								bool *allow_termination)
{
	int		exit_code;
	bool	numeric_error;

	process_single_exit_arg(args[1], &exit_code, &numeric_error);
	if (numeric_error)
	{
		*allow_termination = true;
		return (exit_code);
	}
	if (arg_count > 2)
	{
		msg_error_cmd("exit", NULL, "too many arguments", 1);
		*allow_termination = false;
		return (EXIT_FAILURE);
	}
	*allow_termination = true;
	return (exit_code);
}

/*
* Implementación del builtin exit.
*/
int	builtin_exit(char **args, t_data_env *data)
{
	int		final_exit_code;
	int		arg_count;
	bool	terminate_shell_now;
	bool	can_terminate_based_on_args;

	arg_count = 0;
	while (args && args[arg_count])
		arg_count++;
	terminate_shell_now = should_terminate_shell(data);
	can_terminate_based_on_args = true;
	if (isatty(STDIN_FILENO) && terminate_shell_now)
		ft_putendl_fd("exit", STDERR_FILENO);
	if (arg_count == 1)
		final_exit_code = g_exit_status;
	else
	{
		final_exit_code = handle_exit_args(args, arg_count,
				&can_terminate_based_on_args);
	}
	if (terminate_shell_now && can_terminate_based_on_args)
	{
		free_data_env(data);
		exit(final_exit_code);
	}
	return (final_exit_code);
}
