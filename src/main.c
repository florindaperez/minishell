/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 17:55:12 by castorga          #+#    #+#             */
/*   Updated: 2024/06/07 16:59:38 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

int	g_get_signal = 0; // Se define aquí, como punto de entrada del programa.

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
		set_exit_status(EXIT_FAILURE);
		cleaner_envlist(&envlist);
		return (EXIT_FAILURE);
	}
	env_init_list(envp, &envlist);
	minishell(envlist);
	cleaner_envlist(&envlist);
	return (g_get_signal);
}
