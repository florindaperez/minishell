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
 * signal_handler_interactive
 * Manejador de señal para SIGINT (Ctrl+C) en modo interactivo.
 * Muestra una nueva línea, limpia la línea de readline y la vuelve a mostrar.
 *
 * sig: El número de la señal recibida (no se usa directamente).
 */
static void	signal_handler_interactive(int sig)
{
	(void)sig;
	g_get_signal = 130;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/*
 * signals_interactive
 * Configura los manejadores de señal para el modo interactivo principal.
 * SIGINT (Ctrl+C) se maneja para mostrar un nuevo prompt.
 * SIGQUIT (Ctrl+\) se ignora.
 */
void	signals_interactive(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	ft_memset(&sa_int, 0, sizeof(sa_int));
	ft_memset(&sa_quit, 0, sizeof(sa_quit));
	sa_int.sa_handler = signal_handler_interactive;
	sigaction(SIGINT, &sa_int, NULL);
	sa_quit.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/*
 * signal_handler_heredoc
 * Manejador de señal para SIGINT (Ctrl+C) durante la lectura de un heredoc.
 * Cierra la entrada estándar para interrumpir el bloqueo de readline.
 *
 * sig: El número de la señal recibida (no se usa directamente).
 */
static void	signal_handler_heredoc(int sig)
{
	(void)sig;
	g_get_signal = 130;
	close(STDIN_FILENO);
}

/*
 * signals_heredoc
 * Configura los manejadores de señal para el modo de lectura de heredoc.
 * SIGINT (Ctrl+C) se maneja para interrumpir la lectura.
 * SIGQUIT (Ctrl+\) se sigue ignorando para que no cierre la shell.
 */
void	signals_heredoc(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = signal_handler_heredoc;
	sigaction(SIGINT, &sa, NULL);
}
