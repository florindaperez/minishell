/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:26:46 by flperez-          #+#    #+#             */
/*   Updated: 2025/02/26 13:07:42 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"
/*
* is_n_flag:
* Comprobamos si el el argumento es una opcion valida => true ("-n", "-nn",
* "-nnnnnnnnnn", etc....).
* arg[0] = '-' y el resto de argumentos = 'n'. Cuando encontramos
* algun char != 'n' devuelve false ("-nna", "--n", "-n42").
*/
static bool	value_n_flag(char *arg)
{
	int		i;
	bool	n_flag;

	n_flag = false;
	i = 0;
	if (!arg || arg[i] != '-')
		return (n_flag);
	i++;
	while (arg[i] && arg[i] == 'n')
		i++;
	if (arg[i] == '\0')
		n_flag = true;
	return (n_flag);
}

/*
* print_args_echo:
* Imprimimos los argumentos en la terminal STDOUT.
* Cuando no hemos llegado al final y n_flag es false, anade un salto de linea.
* Ejemplo: "echo flowers power"   => flowers power\n
* "echo -n flowers power" => flowers power
* "echo"                  => \n
* "echo -n"               => (no imprime nada)
*/
static void	print_args_echo(char **args, bool n_flag, int i)
{
	if (!args[i])
	{
		if (!n_flag)
			ft_putchar_fd('\n', STDOUT_FILENO);
		return ;
	}
	while (args[i])
	{
		if (args[i])
			ft_putstr_fd(args[i], STDOUT_FILENO);
		if (args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		else if (!n_flag)
			ft_putchar_fd('\n', STDOUT_FILENO);
		i++;
	}
}

/*
* builtin_echo:
* Replica el comportamiento del comando "echo".
* n_flag indica si se debe omitir el salto de línea final.
*/
int	builtin_echo(char **args)
{
	int		i;
	bool	n_flag;

	n_flag = false;
	i = 1;
	while (args[i] && value_n_flag(args[i]))
	{
		n_flag = true;
		i++;
	}
	print_args_echo(args, n_flag, i);
	return (EXIT_SUCCESS);
}
