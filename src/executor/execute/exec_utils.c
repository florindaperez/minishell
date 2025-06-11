/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 15:59:38 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/10 15:59:41 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * count_args
 * Cuenta el número de elementos en un array de estructuras t_arg_info_exe.
 * El array debe estar terminado en NULL.
 *
 * args: El array de punteros a t_arg_info_exe que se va a contar.
 *
 * Retorna: El número de argumentos en el array.
 */
static int	count_args(t_arg_info_exe **args)
{
	int	count;

	count = 0;
	if (args)
	{
		while (args[count] != NULL)
			count++;
	}
	return (count);
}

/*
 * free_argv_on_strdup_error
 * Libera la memoria de un array 'argv' parcialmente construido cuando
 * una llamada a ft_strdup falla. Libera cada cadena individualmente
 * y luego el propio array.
 *
 * argv: El array parcialmente lleno que necesita ser liberado.
 * k:    El índice hasta el cual el array fue llenado antes del error.
 *
 * Retorna: Siempre NULL, para ser usado como valor de retorno de error.
 */
static char	**free_argv_on_strdup_error(char **argv, int k)
{
	msg_error_cmd("exec", "strdup failed building argv", strerror(errno), 1);
	g_get_signal = 1;
	while (k > 0)
	{
		k--;
		free(argv[k]);
	}
	free(argv);
	return (NULL);
}

/*
 * build_empty_argv
 * Crea un array de argumentos (argv) válido pero vacío. Un argv válido
 * debe ser un puntero a un array que contiene un único puntero NULL.
 *
 * Retorna: Un puntero a un array argv vacío y terminado en NULL, o NULL si
 * falla la asignación de memoria.
 */
static char	**build_empty_argv(void)
{
	char	**argv;

	argv = (char **)malloc(sizeof(char *));
	if (!argv)
	{
		msg_error_cmd("exec", "malloc failed for empty argv",
			strerror(errno), 1);
		g_get_signal = 1;
		return (NULL);
	}
	argv[0] = NULL;
	return (argv);
}

/*
 * fill_argv_array
 * Rellena un array argv preasignado con cadenas duplicadas de un array de
 * estructuras t_arg_info_exe.
 *
 * argv:      El array de cadenas (char **) de destino.
 * args_info: El array de origen de estructuras t_arg_info_exe.
 * argc:      El número de argumentos a duplicar.
 *
 * Retorna: 'true' si todas las cadenas se duplicaron con éxito, 'false' si
 * alguna duplicación falla (en cuyo caso, la memoria se libera).
 */
static bool	fill_argv_array(char **argv, t_arg_info_exe **args_info,
								int argc)
{
	int	i;

	i = 0;
	while (i < argc)
	{
		if (args_info[i] && args_info[i]->value)
			argv[i] = ft_strdup(args_info[i]->value);
		else
			argv[i] = ft_strdup("");
		if (!argv[i])
		{
			free_argv_on_strdup_error(argv, i);
			return (false);
		}
		i++;
	}
	argv[argc] = NULL;
	return (true);
}

/*
 * build_argv_from_args
 * Construye un array de cadenas de tipo argv (char **) a partir de la
 * estructura de argumentos del ejecutor (t_arg_info_exe **). El array
 * resultante es adecuado para ser usado con 'execve'.
 *
 * args: El array de estructuras de argumentos del ejecutor.
 *
 * Retorna: Un nuevo array argv terminado en NULL que debe ser liberado por
 * el llamador, o NULL si ocurre un error de asignación de memoria.
 */
char	**build_argv_from_args(t_arg_info_exe **args)
{
	int		argc;
	char	**argv;

	if (!args)
		return (build_empty_argv());
	argc = count_args(args);
	argv = (char **)malloc(sizeof(char *) * (argc + 1));
	if (!argv)
	{
		msg_error_cmd("exec", "malloc failed for argv array",
			strerror(errno), 1);
		g_get_signal = 1;
		return (NULL);
	}
	if (!fill_argv_array(argv, args, argc))
	{
		return (NULL);
	}
	return (argv);
}
