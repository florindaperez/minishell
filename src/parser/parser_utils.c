/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:02:26 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 17:02:30 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h"

/*
* Comprueba si un tipo de token dado corresponde a una redirección.
*/
bool	is_redirection_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN
		|| type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_APPEND
		|| type == TOKEN_HEREDOC);
}

/*
* Limpia los recursos de E/S previos para un comando (infile o outfile).
* Cierra descriptores de archivo y libera memoria.
*/
void	cleanup_previous_io(t_cmd_io *io, bool is_infile)
{
	if (!io)
		return ;
	if (is_infile)
	{
		if (io->fd_in != -1 && io->fd_in != STDIN_FILENO)
			close(io->fd_in);
		free_ptr(io->infile);
		io->infile = NULL;
		if (io->heredoc_delimiter)
		{
			free_ptr(io->heredoc_delimiter);
			io->heredoc_delimiter = NULL;
		}
		io->fd_in = -1;
	}
	else
	{
		if (io->fd_out != -1 && io->fd_out != STDOUT_FILENO)
			close(io->fd_out);
		free_ptr(io->outfile);
		io->outfile = NULL;
		io->fd_out = -1;
	}
}

/*
* Obtiene el nombre del comando para mostrar en mensajes de error del parser.
* Devuelve el primer argumento del comando, o un nombre por defecto.
*/
char	*cmd_name_for_error_in_parser(t_cmd *cmd)
{
	const char	*default_name;

	if (PROG_NAME)
		default_name = PROG_NAME;
	else
		default_name = "minishell";
	if (!cmd)
		return ((char *)default_name);
	if (!cmd->args || !cmd->args[0] || !cmd->args[0]->value)
		return ((char *)default_name);
	return (cmd->args[0]->value);
}
