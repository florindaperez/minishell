/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 11:07:16 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/26 11:07:23 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
*	perform_exit_cleanup:
*	Realiza la limpieza necesaria antes de que el shell termine.
*	Esto incluye limpiar la lista de variables de entorno y el historial
*	de readline.
*/
static void	perform_exit_cleanup(t_env **envlist)
{
	if (envlist && *envlist)
		cleaner_envlist(envlist);
	rl_clear_history();
}

/*
*	print_exit_msg_if_needed:
*	Imprime el mensaje "exit" en STDERR si la variable 'should_print' es true.
*	Esto generalmente ocurre cuando el comando exit se ejecuta en una terminal
*	interactiva y no está en una subshell o pipeline complejo.
*/
static void	print_exit_msg_if_needed(bool should_print)
{
	if (should_print)
		ft_putstr_fd("exit\n", STDERR_FILENO);
}

/*
*	handle_exit_numeric_error:
*	Maneja el caso en que 'exit' recibe un argumento que no es numérico.
*	Imprime el mensaje "exit" si es necesario, luego un mensaje de error
*	específico ("numeric argument required"), realiza la limpieza y termina
*	el proceso con un código de estado 2.
*/
static void	handle_exit_numeric_error(char *arg_val, bool should_print,
										t_env **envlist)
{
	print_exit_msg_if_needed(should_print);
	msg_error_cmd("exit", arg_val, "numeric argument required\n", 2);
	perform_exit_cleanup(envlist);
	exit(2);
}

/*
*	process_exit_arguments:
*	Procesa los argumentos proporcionados al comando 'exit'.
*	Si el primer argumento es numérico y no hay más argumentos, sale con ese
*	código (0-255). Si no es numérico, llama a handle_exit_numeric_error.
*	Si hay demasiados argumentos, imprime un error y retorna 1 (sin salir).
*/
static int	process_exit_arguments(char **args, int arg_count,
									bool should_print, t_env **envlist)
{
	long long	parsed_ll;
	bool		numeric_error;
	int			final_exit_code;

	numeric_error = false;
	parsed_ll = ft_atoll_for_exit(args[1], &numeric_error);
	if (numeric_error)
	{
		handle_exit_numeric_error(args[1], should_print, envlist);
		return (2);
	}
	if (arg_count > 2)
	{
		msg_error_cmd("exit", NULL, "too many arguments\n", 1);
		set_exit_status(1);
		return (1);
	}
	final_exit_code = (int)(parsed_ll & 0xFF);
	print_exit_msg_if_needed(should_print);
	perform_exit_cleanup(envlist);
	exit(final_exit_code);
	return (0);
}

/*
*	builtin_exit:
*	Implementa el comando incorporado 'exit'. Termina el shell.
*	Si no se proporcionan argumentos, usa el estado de salida del último
*	comando ejecutado. Si se proporciona un argumento numérico, lo usa como
*	estado de salida. Maneja errores para argumentos no numéricos o
*	demasiados argumentos. Realiza la limpieza antes de salir.
*	El parámetro 'envlist' se usa para la limpieza.
*/
int	builtin_exit(t_cmd *cmd, t_env **envlist)
{
	char	**args;
	int		arg_count;
	bool	should_print_exit_msg;
	int		final_exit_code;

	args = cmd->commands;
	arg_count = size_arr2d(args);
	should_print_exit_msg = false;
	if (isatty(STDIN_FILENO))
		should_print_exit_msg = true;
	if (arg_count == 1)
	{
		final_exit_code = g_get_signal;
		print_exit_msg_if_needed(should_print_exit_msg);
		perform_exit_cleanup(envlist);
		exit(final_exit_code);
	}
	else
	{
		return (process_exit_arguments(args, arg_count, \
					should_print_exit_msg, envlist));
	}
	return (0);
}
