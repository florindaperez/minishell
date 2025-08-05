/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 17:55:12 by castorga          #+#    #+#             */
/*   Updated: 2025/06/04 21:24:36 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

int	g_get_signal = 0;

/*
 *  Punto de entrada principal de minishell.
 * Inicializa el entorno y llama al bucle interactivo.
 */
int	main(int ac, char *av[], char *envp[])
{
	t_env	*envlist;

	(void)av;
	envlist = NULL;
	signals_interactive();
	if (ac != 1)
	{
		write(STDERR_FILENO, "minishell: incorrect arguments\n", 31);
		cleaner_envlist(&envlist);
		return (EXIT_FAILURE);
	}
	env_init_list(envp, &envlist);
	minishell(envlist);
	cleaner_envlist(&envlist);
	return (g_get_signal);
}
