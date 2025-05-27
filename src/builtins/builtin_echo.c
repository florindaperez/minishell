/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 12:47:21 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/23 12:47:48 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Comprueba si el argumento es una opción -n válida (ej: "-n", "-nnn").
 * El argumento sólo se lee.
 */
static bool
	is_valid_n_option(const char *arg)
{
	int		i;
	bool	is_n_option;

	is_n_option = false;
	i = 0;
	if (!arg || arg[i] != '-')
		return (is_n_option);
	i++;
	if (arg[i] == '\0')
		return (false);
	while (arg[i] && arg[i] == 'n')
		i++;
	if (arg[i] == '\0')
		is_n_option = true;
	return (is_n_option);
}

/*
 * Procesa los flags -n al inicio de los argumentos de echo.
 * Devuelve el índice del primer argumento que no es un flag -n.
 * Actualiza n_flag_ptr para indicar si se encontró algún flag -n.
 */
static int
	process_n_flags_echo(char **args, bool *n_flag_ptr)
{
	int	i;

	i = 1;
	*n_flag_ptr = false;
	while (args[i] && is_valid_n_option(args[i]))
	{
		*n_flag_ptr = true;
		i++;
	}
	return (i);
}

/*
 * Imprime los argumentos de echo, comenzando desde start_index.
 * Aplica la lógica de n_flag para el salto de línea final.
 */
static void
	print_echo_arguments(char **args, bool n_flag, int start_index)
{
	if (!args[start_index] && !n_flag)
	{
		ft_putchar_fd('\n', STDOUT_FILENO);
		return ;
	}
	if (!args[start_index] && n_flag)
		return ;
	while (args[start_index])
	{
		ft_putstr_fd(args[start_index], STDOUT_FILENO);
		start_index++;
		if (args[start_index])
			ft_putchar_fd(' ', STDOUT_FILENO);
	}
	if (!n_flag)
		ft_putchar_fd('\n', STDOUT_FILENO);
}

/*
 * Maneja el caso especial 'echo ~' (solo), imprimiendo el directorio HOME.
 * Devuelve true si el caso fue manejado, false en caso contrario.
 */
static bool
	handle_special_echo_tilde(char **args, t_env *env)
{
	char	*home_path;

	if (args[1] && ft_strcmp(args[1], "~") == 0 && args[2] == NULL)
	{
		home_path = util_cd_get_env_val(env, "HOME");
		if (home_path)
		{
			ft_putstr_fd(home_path, STDOUT_FILENO);
			ft_putchar_fd('\n', STDOUT_FILENO);
		}
		else
			ft_putchar_fd('\n', STDOUT_FILENO);
		return (true);
	}
	return (false);
}

/*
 * Réplica del comando "echo".
 */
int
	builtin_echo(t_cmd *cmd, t_env *env)
{
	bool	n_flag;
	char	**args;
	int		print_start_index;

	args = cmd->commands;
	if (args[1] == NULL)
	{
		ft_putchar_fd('\n', STDOUT_FILENO);
		set_exit_status(0);
		return (0);
	}
	if (handle_special_echo_tilde(args, env))
	{
		set_exit_status(0);
		return (0);
	}
	print_start_index = process_n_flags_echo(args, &n_flag);
	print_echo_arguments(args, n_flag, print_start_index);
	set_exit_status(0);
	return (0);
}
