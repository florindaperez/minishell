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
 * Rellena el array argv con cadenas duplicadas de args_info.
 * Retorna true si tiene éxito, false si falla un strdup (y libera argv).
 */
static bool	populate_argv_array(char **argv, t_arg_info_exe **args_info,
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
 * Construye un array argv (char**) a partir de la estructura t_arg_info**.
 * Duplica las cadenas usando ft_strdup.
 * El array devuelto debe ser liberado con free_str_tab.
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
	if (!populate_argv_array(argv, args, argc))
	{
		return (NULL);
	}
	return (argv);
}
