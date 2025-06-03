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
 * Este archivo gestiona el comportamiento del shell ante SIGINT y SIGQUIT.
 * Distingue entre modo interactivo (esperando input) y no interactivo
 * (ejecutando un comando).
 *
 * Modo Interactivo:
 * - Ctrl+C (SIGINT): Cancela línea, g_get_signal=130, nuevo prompt.
 * - Ctrl+\ (SIGQUIT): Ignorado.
 *
 * Modo No Interactivo:
 * - Ctrl+C / Ctrl+\: Padre imprime newline. Senal afecta al hijo.
 * g_get_signal se actualiza (respaldo), waitpid determina
 * el final. Hijos restauran manejo por defecto (terminar).
 */

/*
 * signal_reset_prompt: Manejador para SIGINT (Ctrl+C) en modo interactivo.
 *
 * Acción:
 * - Establece g_get_signal a 130 (128 + SIGINT).
 * - Imprime una nueva línea.
 * - Notifica a readline que empiece en una nueva línea.
 * - Borra el buffer actual de readline.
 * - Vuelve a mostrar el prompt vacío.
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
 * ignore_sigquit: Configura el manejador para SIGQUIT (Ctrl+\) a SIG_IGN.
 * Usado en modo interactivo para que Ctrl+\ no afecte al shell.
 */
static void	ignore_sigquit(void)
{
	struct sigaction	conf_signal;

	ft_memset(&conf_signal, 0, sizeof(conf_signal));
	conf_signal.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &conf_signal, NULL);
}

/*
 * signals_interactive: Configura manejadores para el modo interactivo.
 * - SIGINT (Ctrl+C): Llama a signal_reset_prompt.
 * - SIGQUIT (Ctrl+\): Se ignora (llama a ignore_sigquit).
 */
void	signals_interactive(void)
{
	struct sigaction	conf_signal;

	ignore_sigquit();
	ft_memset(&conf_signal, 0, sizeof(conf_signal));
	conf_signal.sa_handler = &signal_reset_prompt;
	conf_signal.sa_flags = SA_RESTART;
	sigaction(SIGINT, &conf_signal, NULL);
}

/*
 * signal_print_newline: Manejador para SIGINT/SIGQUIT en modo no interactivo.
 *
 * Acción:
 * - Imprime una nueva línea para limpieza visual si el hijo fue interrumpido.
 * - Establece g_get_signal (130 o 131) como respaldo. El valor principal
 * vendrá de waitpid al recoger el estado del hijo terminado por senal.
 */
static void	signal_print_newline(int signal_num)
{
	if (signal_num == SIGINT)
		g_get_signal = 130;
	else if (signal_num == SIGQUIT)
		g_get_signal = 131;
	write(STDOUT_FILENO, "\n", 1);
}

/*
 * signals_noninteractive: Configura manejadores para modo no interactivo.
 *
 * Llama a signal_print_newline tanto para SIGINT como para SIGQUIT.
 * El shell padre reacciona mínimamente, la senal va al hijo.
 */
void	signals_noninteractive(void)
{
	struct sigaction	conf_signal;

	ft_memset(&conf_signal, 0, sizeof(conf_signal));
	conf_signal.sa_handler = &signal_print_newline;
	conf_signal.sa_flags = 0;
	sigaction(SIGINT, &conf_signal, NULL);
	sigaction(SIGQUIT, &conf_signal, NULL);
}
