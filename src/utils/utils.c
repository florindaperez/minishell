/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 20:59:20 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/29 20:59:33 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * Cierra de forma segura un descriptor de archivo y lo marca como -1.
 * Solo intenta cerrar si el puntero es válido y el fd no es -1.
 * fd => Puntero al descriptor de archivo a cerrar.
 */
void	safe_close(int *fd)
{
	if (fd != NULL && *fd != -1)
	{
		close(*fd);
		*fd = -1;
	}
}

/*
 * Imprime un mensaje de error del sistema en stderr y sale.
 *  
 * Formato: "minishell: <context>: <error_del_sistema>\n"
 *  Utilizado para errores fatales (fork, pipe, dup2 crítico, etc.)
 *  donde la ejecución no puede continuar de forma segura.
 *
 * context => Un string descriptivo del lugar donde ocurrió el error
 *                (e.g., "pipe", "fork", "dup2 input redirection").
 * g_get_signal => El código de salida con el que terminará el programa.
 */
void	perror_exit(const char *context, int g_get_signal)
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
	exit(g_get_signal);
}

/*
* ft_isspace: Comprueba si el carácter es un espacio en blanco.
* Espacio (' '), tab ('\t'), newline ('\n'), vertical tab ('\v'),
* form feed ('\f'), carriage return ('\r').
*/
int	ft_isspace(int c)
{
	if (c == ' ' || (c >= '\t' && c <= '\r'))
		return (1);
	return (0);
}

/*
 * Comprueba si el comando dado es un builtin.
 * Retorna 1 si es un builtin, 0 en caso contrario.
 */
int	is_builtins(t_cmd *cmd)
{
	if (!cmd || !cmd->commands || !cmd->commands[0])
		return (0);
	if (ft_strcmp(cmd->commands[0], "exit") == 0
		|| ft_strcmp(cmd->commands[0], "pwd") == 0
		|| ft_strcmp(cmd->commands[0], "cd") == 0
		|| ft_strcmp(cmd->commands[0], "env") == 0
		|| ft_strcmp(cmd->commands[0], "echo") == 0
		|| ft_strcmp(cmd->commands[0], "export") == 0
		|| ft_strcmp(cmd->commands[0], "unset") == 0
		|| ft_strcmp(cmd->commands[0], "help") == 0)
		return (1);
	return (0);
}
