/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token_type_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:36:00 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 16:36:03 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h" 
#include "minishell_parser.h"

/*
 * Este archivo contiene funciones de utilidad para verificar tipos
 * de tokens y caracteres específicos del lexer, y otras validaciones
 * sintácticas menores.
 */

/*
 * Verifica si un carácter es un operador de shell.
 */
bool	is_shell_operator_char(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == ';');
}

/*
 * Verifica si un tipo de token es un operador general.
 */
bool	is_op_type(t_token_type type)
{
	return (type == TOKEN_PIPE || type == TOKEN_SEMICOLON
		|| type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_APPEND || type == TOKEN_HEREDOC);
}

/*
 * Verifica si un tipo de token es un tipo de redirección.
 */
bool	is_redir_type(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_APPEND || type == TOKEN_HEREDOC);
}

/*
 * Devuelve true si el carácter en check_idx NO es un espacio y NO es un
 * operador, indicando que está "pegado" al token anterior.
 * Esta función ha sido movida aquí desde lexer_utils.c.
 */
bool	check_no_space_after(const char *input, int check_idx,
				int input_len)
{
	char	c;

	if (check_idx >= input_len)
		return (false);
	c = input[check_idx];
	if (!ft_isspace(c) && !is_shell_operator_char(c)
		&& c != '\'' && c != '"')
		return (true);
	if (!ft_isspace(c))
		return (true);
	return (false);
}
