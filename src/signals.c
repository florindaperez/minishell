/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 14:54:49 by castorga          #+#    #+#             */
/*   Updated: 2024/06/07 17:08:19 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** signal_reset_prompt
** -------------------
** Descripción:
** Manejador para la señal SIGINT (Ctrl+C) en modo interactivo.
** Establece una variable global a 130, imprime una nueva línea y reinicia
** el prompt de readline para una nueva entrada de usuario.
**
** Parámetros:
** - int signal_num: El número de la señal recibida (no se utiliza).
*/
static void	signal_reset_prompt(int signal_num)
{
	(void)signal_num;
	g_get_signal = 130;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/*
** signals_interactive
** -------------------
** Descripción:
** Configura los manejadores para el modo interactivo usando sigaction.
** - SIGINT (Ctrl+C): Llama a signal_reset_prompt. La bandera SA_RESTART
** ayuda a evitar que algunas llamadas al sistema sean interrumpidas.
** - SIGQUIT (Ctrl+\): Se ignora.
*/
void	signals_interactive(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	ft_memset(&sa_int, 0, sizeof(sa_int));
	ft_memset(&sa_quit, 0, sizeof(sa_quit));
	sa_int.sa_handler = signal_reset_prompt;
	sa_int.sa_flags = SA_RESTART;
	sa_quit.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/*
** signals_during_execution
** --------------------------
** Descripción:
** Configura al padre para que ignore las señales mientras un hijo se ejecuta.
** De esta forma, la señal solo afectará al proceso hijo.
** Se debe llamar en el proceso padre antes de la llamada a waitpid.
*/
void	signals_during_execution(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

/*
** signals_default_for_child
** ---------------------------
** Descripción:
** Restaura el comportamiento por defecto de las señales en el proceso hijo
** justo antes de llamar a execve. Esto asegura que el comando que se ejecuta
** (como 'sleep') termine al recibir Ctrl+C, como lo haría en un shell normal.
*/
void	signals_default_for_child(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}
