/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 20:41:39 by asans-go          #+#    #+#             */
/*   Updated: 2025/03/17 19:08:07 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h"

/*
 * Inicializa las variables de la estructura t_lexer_state.
 * Esta función es una auxiliar estática para initial_tokenizer.
 */
static void	initialize_lexer_state_vars(t_lexer_state *ls_ptr,
										char *input_str)
{
	ls_ptr->buf_idx = 0;
	ft_memset(ls_ptr->buffer, 0, MAX_TOKEN_LENGTH);
	ls_ptr->current_quote_state = NO_QUOTE;
	ls_ptr->head_node = NULL;
	ls_ptr->current_tail_node = NULL;
	ls_ptr->input = input_str;
	ls_ptr->input_len = ft_strlen(input_str);
	ls_ptr->i = 0;
	ls_ptr->error_flag = false;
}

/*
 * Maneja el error de comillas no cerradas al final de la entrada.
 * Esta función es una auxiliar estática para run_tokenizer_loop.
 */
static t_token	*handle_eof_unclosed_quote(t_lexer_state *ls)
{
	char	quote_char_val;
	char	error_msg_buffer[100];

	if (ls->current_quote_state == SINGLE_QUOTE)
		quote_char_val = '\'';
	else
		quote_char_val = '"';
	snprintf(error_msg_buffer, sizeof(error_msg_buffer),
		"syntax error: unexpected EOF while looking for matching `%c'",
		quote_char_val);
	msg_error_cmd(PROG_NAME, error_msg_buffer, NULL, SYNTAX_ERROR_STATUS);
	g_exit_status = SYNTAX_ERROR_STATUS;
	free_tokens(ls->head_node);
	return (NULL);
}

/*
 * Bucle principal del tokenizer. Itera sobre la entrada procesando caracteres.
 * Delega el procesamiento de cada carácter a funciones en lexer_processing.c.
 */
static t_token	*run_tokenizer_loop(t_lexer_state *ls)
{
	while (ls->i < ls->input_len && !ls->error_flag)
	{
		if (ls->current_quote_state == NO_QUOTE)
			process_char_no_quote_state(ls);
		else
			process_char_in_quote_state(ls);
	}
	if (ls->error_flag)
	{
		free_tokens(ls->head_node);
		return (NULL);
	}
	if (ls->buf_idx > 0)
	{
		if (ls->current_quote_state == NO_QUOTE)
			flush_buffer(ls, NO_QUOTE);
		if (ls->error_flag)
		{
			free_tokens(ls->head_node);
			return (NULL);
		}
	}
	if (ls->current_quote_state != NO_QUOTE)
		return (handle_eof_unclosed_quote(ls));
	return (ls->head_node);
}

/*
 * Inicializa el estado del lexer y comienza el proceso de tokenización.
 * Llama a run_tokenizer_loop para el procesamiento principal.
 */
static t_token	*initial_tokenizer(char *input)
{
	t_lexer_state	ls;

	if (!input)
		return (NULL);
	initialize_lexer_state_vars(&ls, input);
	return (run_tokenizer_loop(&ls));
}

/*
 * Función principal del lexer. Toma una cadena de entrada,
 * la tokeniza y verifica la gramática básica.
 */
t_token	*lexer(char *input)
{
	t_token	*tokens_list;

	tokens_list = NULL;
	if (input == NULL || input[0] == '\0')
		return (NULL);
	tokens_list = initial_tokenizer(input);
	if (!tokens_list)
	{
		return (NULL);
	}
	if (!check_grammar(tokens_list))
	{
		free_tokens(tokens_list);
		return (NULL);
	}
	return (tokens_list);
}
