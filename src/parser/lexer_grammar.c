/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_grammar.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 13:25:42 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 13:25:44 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"
#include "minishell_parser.h"

/* --- Prototipos Estáticos Internos --- */
static char	*s_get_double_operator_error_arg(t_token *current);
static char	*s_get_redir_syntax_error_arg(t_token *current);
static char	*s_get_op_at_end_error_arg(t_token *current);

/*
 * Verifica si hay dos operadores consecutivos que constituyan un error y
 * devuelve el argumento ofensivo si se encuentra un error.
 */
static char	*s_get_double_operator_error_arg(t_token *current)
{
	if (!(is_op_type(current->type) && current->next
			&& is_op_type(current->next->type)))
		return (NULL);
	if (is_redir_type(current->type)
		&& is_redir_type(current->next->type))
		return (current->next->args);
	if ((current->type == TOKEN_PIPE || current->type == TOKEN_SEMICOLON)
		&& (current->next->type == TOKEN_PIPE
			|| current->next->type == TOKEN_SEMICOLON))
		return (current->next->args);
	if (current->type == TOKEN_PIPE && current->next
		&& is_redir_type(current->next->type))
		return (current->next->args);
	return (NULL);
}

/*
 * Verifica errores de sintaxis en redirecciones, como la falta de un argumento.
 * Devuelve el argumento ofensivo o "newline" si se encuentra un error.
 */
static char	*s_get_redir_syntax_error_arg(t_token *current)
{
	if (is_redir_type(current->type)
		&& (!current->next || current->next->type != TOKEN_WORD))
	{
		if (current->next)
			return (current->next->args);
		else
			return ("newline");
	}
	return (NULL);
}

/*
 * Verifica si un operador (pipe o redirección) está al final de la línea
 * sin un argumento o comando subsiguiente.
 * Devuelve "newline" si se encuentra dicho error.
 */
static char	*s_get_op_at_end_error_arg(t_token *current)
{
	if ((current->type == TOKEN_PIPE || is_redir_type(current->type))
		&& !current->next)
	{
		return ("newline");
	}
	return (NULL);
}

/*
 * Valida la secuencia de un token y su siguiente para errores gramaticales.
 * Devuelve true si la secuencia es válida, false si se detecta un error
 * y se reporta.
 */
bool	validate_token_sequence(t_token *current)
{
	char	*err_arg;

	err_arg = s_get_double_operator_error_arg(current);
	if (err_arg)
	{
		msg_error_cmd(PROG_NAME, "syntax error: unexpected token", err_arg, 2);
		g_exit_status = 2;
		return (false);
	}
	err_arg = s_get_redir_syntax_error_arg(current);
	if (err_arg)
	{
		msg_error_cmd(PROG_NAME, "syntax error: unexpected token", err_arg, 2);
		g_exit_status = 2;
		return (false);
	}
	err_arg = s_get_op_at_end_error_arg(current);
	if (err_arg)
	{
		msg_error_cmd(PROG_NAME, "syntax error: unexpected token", err_arg, 2);
		g_exit_status = 2;
		return (false);
	}
	return (true);
}

/*
 * Verifica la gramática general de la lista de tokens.
 * Itera sobre los tokens y valida secuencias.
 */
bool	check_grammar(t_token *head)
{
	t_token	*current;

	current = head;
	if (!current)
		return (true);
	if (current->type == TOKEN_PIPE || current->type == TOKEN_SEMICOLON)
	{
		msg_error_cmd(PROG_NAME, "syntax error: unexpected token",
			current->args, 2);
		g_exit_status = 2;
		return (false);
	}
	while (current)
	{
		if (!validate_token_sequence(current))
			return (false);
		current = current->next;
	}
	return (true);
}
