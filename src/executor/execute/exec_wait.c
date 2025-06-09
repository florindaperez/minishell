/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_wait.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 22:05:57 by flperez-          #+#    #+#             */
/*   Updated: 2025/06/09 22:06:01 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * process_last_child_status
 * -------------------------
 * Espera al último hijo del pipeline, procesa su estado de salida (incluyendo
 * el manejo de señales) y retorna el código de salida final.
 */
static int	process_last_child_status(pid_t last_pid)
{
	int	status;
	int	exit_code;

	exit_code = EXIT_SUCCESS;
	waitpid(last_pid, &status, 0);
	if (WIFEXITED(status))
		exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT || WTERMSIG(status) == SIGQUIT)
			write(STDOUT_FILENO, "\n", 1);
		exit_code = 128 + WTERMSIG(status);
	}
	return (exit_code);
}

/*
 * reap_remaining_children
 * -----------------------
 * Entra en un bucle para esperar y recoger (reap) a cualquier proceso
 * hijo restante, evitando que queden procesos zombies.
 */
static void	reap_remaining_children(void)
{
	pid_t	waited_pid;

	while (1)
	{
		waited_pid = wait(NULL);
		if (waited_pid == -1)
		{
			if (errno == ECHILD || errno == EINTR)
				break ;
		}
	}
}

/*
* wait_for_all_children
*
* Orquesta la espera de todos los procesos hijos. Primero, espera al
* último hijo para determinar el estado de salida de la pipeline.
* Luego, recoge a todos los demás hijos para evitar zombies.
*/
int	wait_for_all_children(pid_t last_pid)
{
	int	exit_code;

	exit_code = EXIT_SUCCESS;
	if (last_pid > 0)
		exit_code = process_last_child_status(last_pid);
	reap_remaining_children();
	return (exit_code);
}
