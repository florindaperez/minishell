/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 20:04:00 by flperez-          #+#    #+#             */
/*   Updated: 2025/06/10 20:04:14 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * signals_execution
 * Configura las señales para cuando un comando se está ejecutando.
 * Ambas señales, SIGINT y SIGQUIT, son ignoradas por el proceso padre
 * para que solo el proceso hijo las maneje.
 */
void	signals_execution(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

/*
 * signals_child_default
 * Restaura el comportamiento por defecto de las señales para un proceso hijo.
 * Esto permite que los comandos ejecutados (como `cat` o `sleep`) puedan
 * ser interrumpidos con Ctrl+C.
 */
void	signals_child_default(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}
