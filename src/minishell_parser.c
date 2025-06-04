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

/*
 * parse_and_handle_tokens
 * Intenta parsear la lista de tokens en una lista de comandos.
 * Libera la lista de tokens al finalizar.
 *
 * tok_list: Puntero al puntero de la lista de tokens.
 * cmd_list: Puntero al puntero de la lista de comandos.
 *
 * Retorna: 0 en éxito, 1 en caso de error de parseo.
 */
static int	parse_and_handle_tokens(t_tok **tok_list, t_cmd **cmd_list)
{
	if (parser(cmd_list, *tok_list) == 1)
	{
		if (*cmd_list)
			cmd_free(cmd_list);
		tok_free(tok_list);
		return (1);
	}
	tok_free(tok_list);
	return (0);
}

/*
 * expand_and_heredoc
 * Realiza la expansión de variables y el manejo de heredocs para los comandos.
 *
 * cmd_list: Puntero al puntero de la lista de comandos.
 * env: Puntero a la cabeza de la lista de entorno.
 *
 * Retorna: 0 en éxito, 1 en caso de error (ej. heredoc).
 */
static int	expand_and_heredoc(t_cmd **cmd_list, t_env *env)
{
	should_expand(*cmd_list, env);
	if (heredoc(*cmd_list) != 0)
	{
		cmd_free(cmd_list);
		return (1);
	}
	return (0);
}

/*
 * tokenize_parse_expand
 * Tokeniza, parsea, expande variables y maneja heredocs para una línea.
 *
 * line: La línea de comando en bruto.
 * env: Puntero a la cabeza de la lista de entorno, necesario para la expansión.
 *
 * Retorna: La cabeza de la lista de comandos procesada (t_cmd *),
 * o NULL si hay un error o no hay comandos.
 */
t_cmd	*tokenize_parse_expand(char *line, t_env *env)
{
	t_tok	*tok_list;
	t_cmd	*cmd_list;

	tok_list = NULL;
	cmd_list = NULL;
	tokenizer(&tok_list, line);
	if (tok_list == NULL && line != NULL && *line != '\0')
	{
		msg_error_cmd("", NULL, "command not found", 127);
		return (NULL);
	}
	if (tok_list == NULL)
		return (NULL);
	if (parse_and_handle_tokens(&tok_list, &cmd_list) == 1)
		return (NULL);
	if (cmd_list == NULL)
		return (NULL);
	if (expand_and_heredoc(&cmd_list, env) == 1)
		return (NULL);
	return (cmd_list);
}
