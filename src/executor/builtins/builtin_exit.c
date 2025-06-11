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
#include "minishell_executor.h"

/*
 * should_terminate_shell
 * Determina si la shell debe terminar al llamar a 'exit'. La terminación
 * solo debe ocurrir si 'exit' es el único comando en el pipeline, no si
 * forma parte de una cadena más larga (ej. 'exit | cat').
 *
 * data: La estructura de datos del ejecutor que contiene la lista de comandos.
 *
 * Retorna: 'true' si 'exit' es el único comando, 'false' en caso contrario.
 */
static bool	should_terminate_shell(t_data_env_exe *data)
{
	return (!data->cmds_head || (!data->cmds_head->next
			&& !data->cmds_head->prev));
}

/*
 * process_single_exit_arg
 * Procesa un único argumento en formato de cadena para 'exit', determinando si
 * es numérico y calculando el código de salida correspondiente (0-255).
 *
 * arg_str:       El argumento en formato de cadena a procesar.
 * exit_code_val: Puntero a un entero donde se almacenará el código de salida.
 * is_err:        Puntero a un booleano que se establecerá a 'true' si el
 * argumento no es un número válido.
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
 * handle_exit_args
 * Maneja la lógica de salida cuando se proporcionan uno o más argumentos a
 * 'exit'. Valida los argumentos y determina si la terminación del shell
 * debe proceder.
 *
 * args:              El array de argumentos del comando 'exit'.
 * arg_count:         El número total de argumentos.
 * allow_termination: Puntero a un booleano. Se establece a 'false' si hay
 * demasiados argumentos, para prevenir la salida.
 *
 * Retorna: El código de estado final que 'exit' debe usar.
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
 * builtin_exit
 * Implementación del comando built-in 'exit'. Gestiona la terminación de la
 * shell, incluyendo el manejo de argumentos, códigos de salida y la liberación
 * de recursos antes de salir.
 *
 * args: El array de argumentos pasados al comando 'exit'.
 * data: La estructura de datos principal del ejecutor.
 *
 * Retorna: El código de salida. Si la shell termina, esta función no retorna
 * a su llamador, ya que el proceso finaliza.
 */
int	builtin_exit(char **args, t_data_env_exe *data)
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
		final_exit_code = g_get_signal;
	else
	{
		final_exit_code = handle_exit_args(args, arg_count,
				&can_terminate_based_on_args);
	}
	if (terminate_shell_now && can_terminate_based_on_args)
	{
		free_data_env_exe(data);
		exit(final_exit_code);
	}
	return (final_exit_code);
}
