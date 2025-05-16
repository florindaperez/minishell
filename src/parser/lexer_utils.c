/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 13:25:19 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 13:25:22 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"
#include "minishell_parser.h"

/*
 * Este archivo contiene funciones de utilidad generales para el lexer.
 * Incluye creación y gestión de tokens, y manejo del buffer de tokens.
 * Se asume que ft_strdup y ft_isspace provienen de libft.
 */
/*
 * Inicializa los argumentos (args) de un token, manejando ft_strdup.
 * Devuelve true si tiene éxito, false si ft_strdup falla (y libera el token).
 */
static bool	s_init_token_args(t_token *token, char *str_val)
{
	if (str_val == NULL)
		token->args = ft_strdup("");
	else
		token->args = ft_strdup(str_val);
	if (!token->args)
	{
		msg_error_cmd("lexer", "strdup error in new_token", strerror(errno), 1);
		g_exit_status = 1;
		free(token);
		return (false);
	}
	return (true);
}

/*
 * Crea un nuevo token y lo inicializa.
 */
t_token	*new_token(char *str_val, t_token_type type,
					t_quote_type quote_status)
{
	t_token	*token;

	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
	{
		msg_error_cmd("lexer", "malloc error in new_token", strerror(errno), 1);
		g_exit_status = 1;
		return (NULL);
	}
	if (!s_init_token_args(token, str_val))
		return (NULL);
	token->type = type;
	token->quote_status = quote_status;
	token->no_space_after = false;
	token->prev = NULL;
	token->next = NULL;
	return (token);
}

/*
 * Añade un token a la lista de tokens dentro del estado del lexer.
 */
void	add_token_to_list_state(t_lexer_state *ls, t_token *new_node)
{
	if (!new_node)
	{
		ls->error_flag = true;
		return ;
	}
	if (!ls->head_node)
	{
		ls->head_node = new_node;
		ls->current_tail_node = new_node;
	}
	else
	{
		ls->current_tail_node->next = new_node;
		new_node->prev = ls->current_tail_node;
		ls->current_tail_node = new_node;
	}
}

/*
 * Libera la memoria de una lista de tokens.
 */
void	free_tokens(t_token *head)
{
	t_token	*current;
	t_token	*next_token;

	current = head;
	while (current != NULL)
	{
		next_token = current->next;
		if (current->args)
			free(current->args);
		free(current);
		current = next_token;
	}
}

/*
 * Crea un token TOKEN_WORD con el contenido actual del buffer.
 * Llamada desde varias partes del lexer.
 */
void	flush_buffer(t_lexer_state *ls,
						t_quote_type quote_status_of_token)
{
	t_token	*created_token;
	int		idx_after_token;

	if (ls->buf_idx == 0 && quote_status_of_token == NO_QUOTE)
		return ;
	ls->buffer[ls->buf_idx] = '\0';
	created_token = new_token(ls->buffer, TOKEN_WORD,
			quote_status_of_token);
	if (!created_token)
	{
		ls->error_flag = true;
		return ;
	}
	if (quote_status_of_token != NO_QUOTE)
		idx_after_token = ls->i + 1;
	else
		idx_after_token = ls->i;
	created_token->no_space_after = check_no_space_after(ls->input,
			idx_after_token, ls->input_len);
	add_token_to_list_state(ls, created_token);
	ls->buf_idx = 0;
}
