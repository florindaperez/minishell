/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_processing.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:12:22 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 14:12:25 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"
#include "minishell_parser.h"

/*
 * Este archivo contiene la lógica de procesamiento de caracteres para el lexer.
 * Incluye funciones para manejar comillas y palabras.
 * Depende de lexer_operator_logic.c para la lógica específica de operadores
 * y de lexer_utils.c para utilidades de creación, gestión de tokens y 
 * flush_buffer.
 */

/*
 * Se llama cuando se encuentra una comilla de apertura.
 * Es una función auxiliar estática para process_char_no_quote_state.
 */
static void	s_handle_quote_char(t_lexer_state *ls, char quote_char)
{
	if (ls->buf_idx > 0)
	{
		flush_buffer(ls, NO_QUOTE);
		if (ls->error_flag)
			return ;
		if (ls->current_tail_node)
			ls->current_tail_node->no_space_after = true;
	}
	if (quote_char == '\'')
		ls->current_quote_state = SINGLE_QUOTE;
	else
		ls->current_quote_state = DOUBLE_QUOTE;
	ls->i++;
}

/*
 * Procesa un carácter que es parte de una palabra (no espacio,
 * no operador, no comilla).
 */
static void	s_process_word_char_nq(t_lexer_state *ls)
{
	if (ls->buf_idx < MAX_TOKEN_LENGTH - 1)
	{
		ls->buffer[ls->buf_idx] = ls->input[ls->i];
		ls->buf_idx++;
	}
	else
	{
		msg_error_cmd("lexer", NULL, "token too long", 1);
		g_exit_status = 1;
		ls->error_flag = true;
	}
	ls->i++;
}

/*
 * Añade un carácter al buffer cuando se está dentro de comillas,
 * o maneja el error de token demasiado largo.
 * Es una función auxiliar estática para process_char_in_quote_state.
 */
static void	s_buffer_char_in_quote(t_lexer_state *ls, char char_to_buffer)
{
	if (ls->buf_idx < MAX_TOKEN_LENGTH - 1)
	{
		ls->buffer[ls->buf_idx] = char_to_buffer;
		ls->buf_idx++;
	}
	else
	{
		msg_error_cmd("lexer", NULL, "token too long in quotes", 1);
		g_exit_status = 1;
		ls->error_flag = true;
	}
	ls->i++;
}

/*
 * Procesa un carácter cuando el lexer está en un estado de comillas.
 * Busca la comilla de cierre o añade el carácter al buffer.
 */
void	process_char_in_quote_state(t_lexer_state *ls)
{
	char	current_char;
	char	matching_quote;

	current_char = ls->input[ls->i];
	if (ls->current_quote_state == SINGLE_QUOTE)
		matching_quote = '\'';
	else
		matching_quote = '"';
	if (current_char == matching_quote)
	{
		flush_buffer(ls, ls->current_quote_state);
		if (ls->error_flag)
			return ;
		ls->current_quote_state = NO_QUOTE;
		ls->i++;
	}
	else
		s_buffer_char_in_quote(ls, current_char);
}

/*
 * Procesa un carácter cuando el lexer no está en un estado de comillas.
 * Decide si es un espacio, operador, comilla o parte de una palabra.
 */
void	process_char_no_quote_state(t_lexer_state *ls)
{
	if (ft_isspace(ls->input[ls->i]))
	{
		flush_buffer(ls, NO_QUOTE);
		if (ls->error_flag)
			return ;
		while (ls->i < ls->input_len && ft_isspace(ls->input[ls->i]))
			ls->i++;
	}
	else if (is_shell_operator_char(ls->input[ls->i]))
	{
		flush_buffer(ls, NO_QUOTE);
		if (ls->error_flag)
			return ;
		if (ls->current_tail_node && ls->buf_idx == 0)
			ls->current_tail_node->no_space_after = true;
		create_and_add_operator_token(ls);
	}
	else if (ls->input[ls->i] == '\'' || ls->input[ls->i] == '"')
		s_handle_quote_char(ls, ls->input[ls->i]);
	else
		s_process_word_char_nq(ls);
}
