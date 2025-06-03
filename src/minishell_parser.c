/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_parser.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 16:20:34 by flperez-          #+#    #+#             */
/*   Updated: 2025/06/03 16:20:42 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/* Variable global para el estado de salida (definida en main.c) */
extern int	g_get_signal;

/*
 * Parte 1 del procesamiento: Tokeniza la línea.
 * Retorna la lista de tokens o NULL si hay error o señal.
*/
static t_tok	*tokenize_input_line(char *line)
{
	t_tok	*token_list;

	token_list = NULL;
	tokenizer(&token_list, line);
	if (token_list == NULL || g_get_signal != 0)
	{
		if (token_list)
			tok_free(&token_list);
		return (NULL);
	}
	return (token_list);
}

/*
 * Parte 2 del procesamiento: Parsea los tokens.
 * Libera token_list siempre. Retorna cmd_list o NULL en error/señal.
 */
static t_cmd	*parse_token_list(t_tok **token_list_ptr)
{
	t_cmd	*cmd_list;

	cmd_list = NULL;
	if (parser(&cmd_list, *token_list_ptr) == 1)
	{
		if (cmd_list)
			cmd_free(&cmd_list);
		tok_free(token_list_ptr);
		return (NULL);
	}
	tok_free(token_list_ptr);
	if (g_get_signal != 0)
	{
		if (cmd_list)
			cmd_free(&cmd_list);
		return (NULL);
	}
	return (cmd_list);
}

/*
 * Parte 3 del procesamiento: Expande y maneja heredocs.
 * Retorna cmd_list modificado o NULL si hay error/señal (liberando cmd_list).
 */
static t_cmd	*expand_and_heredoc(t_cmd *cmd_list, t_env *env_list_head)
{
	if (cmd_list == NULL)
		return (NULL);
	should_expand(cmd_list, env_list_head);
	if (g_get_signal != 0)
	{
		cmd_free(&cmd_list);
		return (NULL);
	}
	if (heredoc(cmd_list) != 0)
	{
		cmd_free(&cmd_list);
		return (NULL);
	}
	if (g_get_signal != 0)
	{
		cmd_free(&cmd_list);
		return (NULL);
	}
	return (cmd_list);
}

/*
 * Procesa una línea de entrada completa.
 * Orquesta la tokenización, parseo, expansión y heredocs.
 * line: La línea de comando en bruto leída del usuario.
 * env_list_head: Puntero a la cabeza de la lista de variables de entorno.
 * Retorna: Una lista de comandos (t_cmd *) lista para ejecución,
 * o NULL si ocurre un error, la línea está vacía,
 * o una señal interrumpe el proceso.
 * Esta función será llamada desde minishell.c.
 */
t_cmd	*parse_and_prepare_line(char *line, t_env *env_list_head)
{
	t_tok	*token_list;
	t_cmd	*cmd_list;

	if (!line || !*line)
		return (NULL);
	token_list = tokenize_input_line(line);
	if (!token_list)
		return (NULL);
	cmd_list = parse_token_list(&token_list);
	if (!cmd_list)
		return (NULL);
	cmd_list = expand_and_heredoc(cmd_list, env_list_head);
	return (cmd_list);
}
