/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 02:16:47 by flperez-          #+#    #+#             */
/*   Updated: 2025/04/25 02:26:16 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Imprime un mensaje de error del sistema en stderr y sale.
 *  
 * Formato: "minishell: <context>: <error_del_sistema>\n"
 *  Utilizado para errores fatales (fork, pipe, dup2 crítico, etc.)
 *  donde la ejecución no puede continuar de forma segura.
 *
 * context => Un string descriptivo del lugar donde ocurrió el error
 *                (e.g., "pipe", "fork", "dup2 input redirection").
 * g_exit_status => El código de salida con el que terminará el programa.
 */
void	perror_exit(const char *context, int g_exit_status)
{
	char	*err_str;

	err_str = strerror(errno);
	write(STDERR_FILENO, "minishell: ", 11);
	if (context)
	{
		write(STDERR_FILENO, context, ft_strlen(context));
		write(STDERR_FILENO, ": ", 2);
	}
	write(STDERR_FILENO, err_str, ft_strlen(err_str));
	write(STDERR_FILENO, "\n", 1);
	exit(g_exit_status);
}
