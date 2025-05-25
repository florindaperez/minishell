/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 18:45:16 by asans-go          #+#    #+#             */
/*   Updated: 2025/03/18 19:48:04 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Este archivo contiene funciones para liberar la memoria utilizada por
 * diferentes estructuras de datos en el proyecto minishell.
 *
*/

#include "minishell.h"

/*
 * free_cmd_args: Función auxiliar estática para liberar un array de t_arg_info.
 * 
*/
static void	free_cmd_args(t_arg_info **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i] != NULL)
	{
		free_ptr(args[i]->value);
		args[i]->value = NULL;
		free_ptr(args[i]);
		args[i] = NULL;
		i++;
	}
	free(args);
}

/*
 * free_cmd_io: Función auxiliar estática para liberar t_cmd_io y recursos.
 * 
*/
static void	free_cmd_io(t_cmd_io *io)
{
	if (!io)
		return ;
	if (io->heredoc_delimiter && io->infile)
	{
		unlink(io->infile);
	}
	free_ptr(io->infile);
	io->infile = NULL;
	free_ptr(io->outfile);
	io->outfile = NULL;
	free_ptr(io->heredoc_delimiter);
	io->heredoc_delimiter = NULL;
	free(io);
}

/*
 * free_commands: Libera la memoria de una lista enlazada de estructuras t_cmd.
 * - Recorre la lista enlazada de comandos (pipes).
 * - Para cada comando, llama a las funciones auxiliares para liberar:
 * 1. El array de argumentos (args).
 * 2. La ruta del comando (path).
 * 3. La estructura de I/O (io) y sus recursos asociados.
 * - Finalmente, libera el nodo t_cmd actual.
*/
void	free_commands(t_cmd *cmd_list_head)
{
	t_cmd	*current_cmd;
	t_cmd	*next_cmd;

	current_cmd = cmd_list_head;
	while (current_cmd != NULL)
	{
		next_cmd = current_cmd->next;
		free_cmd_args(current_cmd->args);
		current_cmd->args = NULL;
		free_ptr(current_cmd->path);
		current_cmd->path = NULL;
		free_cmd_io(current_cmd->io);
		current_cmd->io = NULL;
		free(current_cmd);
		current_cmd = next_cmd;
	}
}

/*
 * SE PASA A LEXER_UTILS.C
 *free_tokens: Libera una lista enlazada de tokens (t_token).
 * - Recorre la lista enlazada de tokens generada por el lexer.
 * - Para cada token, libera la cadena 'args' (que contiene el valor del token).
 * - Libera el nodo t_token en sí.
*
void	free_tokens(t_token *head)
{
	t_token	*tmp;

	while (head != NULL)
	{
		tmp = head->next;
		free_ptr(head->args);
		head->args = NULL;
		free(head);
		head = tmp;
	}
}
*/