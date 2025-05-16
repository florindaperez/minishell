/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator_logic.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:43:10 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 14:43:14 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"
#include "minishell_parser.h"

/*
 * Este archivo contiene la lógica para determinar los detalles de los tokens
 * de operador y para crear y añadir dichos tokens a la lista.
 */

/*
 * Establece los detalles para operadores de un solo carácter.
 * Es una función auxiliar estática para get_operator_details.
 */
static void	s_set_single_char_op_details(t_lexer_state *ls,
				t_token_type *type_out)
{
	if (ls->input[ls->i] == '>')
		*type_out = TOKEN_REDIRECT_OUT;
	else if (ls->input[ls->i] == '<')
		*type_out = TOKEN_REDIRECT_IN;
	else if (ls->input[ls->i] == '|')
		*type_out = TOKEN_PIPE;
	else
		*type_out = TOKEN_SEMICOLON;
}

/*
 * Determina los detalles de un token de operador (tipo, cadena y longitud).
 * Esta función es llamada desde create_and_add_operator_token.
 */
void	get_operator_details(t_lexer_state *ls, char *op_str_out,
							t_token_type *type_out, int *len_out)
{
	*len_out = 1;
	op_str_out[0] = ls->input[ls->i];
	op_str_out[1] = '\0';
	op_str_out[2] = '\0';
	if (ls->input[ls->i] == '>' && ls->i + 1 < ls->input_len
		&& ls->input[ls->i + 1] == '>')
	{
		op_str_out[1] = '>';
		*type_out = TOKEN_APPEND;
		*len_out = 2;
	}
	else if (ls->input[ls->i] == '<' && ls->i + 1 < ls->input_len
		&& ls->input[ls->i + 1] == '<')
	{
		op_str_out[1] = '<';
		*type_out = TOKEN_HEREDOC;
		*len_out = 2;
	}
	else
	{
		s_set_single_char_op_details(ls, type_out);
	}
}

/*
 * Crea un token de operador basado en los detalles obtenidos
 * y lo añade a la lista de tokens. Es llamada desde
 * process_char_no_quote_state en lexer_processing.c.
 */
void	create_and_add_operator_token(t_lexer_state *ls)
{
	char			op_str[3];
	t_token_type	op_type;
	int				op_len;
	t_token			*created_token;

	get_operator_details(ls, op_str, &op_type, &op_len);
	created_token = new_token(op_str, op_type, NO_QUOTE);
	if (!created_token)
	{
		ls->error_flag = true;
		return ;
	}
	created_token->no_space_after = check_no_space_after(ls->input,
			ls->i + op_len, ls->input_len);
	add_token_to_list_state(ls, created_token);
	ls->i += op_len;
}
