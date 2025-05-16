/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 19:24:32 by asans-go          #+#    #+#             */
/*   Updated: 2025/04/24 23:19:06 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h" /* Para t_parser_ctx y prototipos */

/*
* Dirige el procesamiento del token actual a la función manejadora adecuada.
* Parámetros:
* ctx: Puntero a la estructura de contexto del parser.
* Retorno:
* 1 si el parseo debe continuar, 0 en error fatal.
*/
static int	dispatch_token_processing(t_parser_ctx *ctx)
{
	if (ctx->curr_tk->type == TOKEN_PIPE
		|| ctx->curr_tk->type == TOKEN_SEMICOLON)
	{
		*(ctx->cmd_ptr) = handle_operator_token(*(ctx->cmd_ptr),
				ctx->tk_iter_ptr, *(ctx->first_cmd_ptr));
		if (!*(ctx->cmd_ptr))
			return (0);
	}
	else if (is_redirection_token(ctx->curr_tk->type))
	{
		if (process_redir_token(*(ctx->cmd_ptr), ctx->tk_iter_ptr,
				ctx->d_env, *(ctx->first_cmd_ptr)) == 0)
			return (0);
	}
	else if (ctx->curr_tk->type == TOKEN_WORD)
	{
		if (process_word_token(*(ctx->cmd_ptr), ctx->tk_iter_ptr,
				ctx->d_env, *(ctx->first_cmd_ptr)) == 0)
			return (0);
	}
	else
		return (handle_unhandled_token(ctx->curr_tk, *(ctx->first_cmd_ptr)));
	return (1);
}

/*
* Procesa el token actual en el flujo de parseo.
* Delega a manejadores específicos según el tipo de token.
* Parámetros:
* ctx: Puntero a la estructura de contexto del parser.
* Retorno:
* 1 si el parseo debe continuar, 0 en error fatal.
*/
static int	process_current_token(t_parser_ctx *ctx)
{
	ctx->curr_tk = *(ctx->tk_iter_ptr);
	if ((*(ctx->cmd_ptr))->redir_error && ctx->curr_tk->type != TOKEN_PIPE
		&& ctx->curr_tk->type != TOKEN_SEMICOLON)
	{
		*(ctx->tk_iter_ptr) = ctx->curr_tk->next;
		return (1);
	}
	return (dispatch_token_processing(ctx));
}

/*
* Función principal del parser. Convierte una lista de tokens en una lista de
* comandos (t_cmd) listos para ser ejecutados.
* Parámetros:
* tokens: Puntero al primer token de la lista generada por el lexer.
* data_env: Puntero a la estructura de datos del entorno (para expansiones, ..
* Retorno:
* Puntero al primer comando de la lista de comandos, o NULL si hay error 
* o no hay tokens.
*/
t_cmd	*parse_tokens(
	t_token *tokens,
	t_data_env *data_env)
{
	t_parser_ctx	ctx;
	t_cmd			*current_cmd_node;
	t_cmd			*first_cmd_node;
	t_token			*token_iterator_node;
	int				status_code;

	if (!tokens)
		return (NULL);
	current_cmd_node = new_command();
	if (!current_cmd_node)
		return (NULL);
	first_cmd_node = current_cmd_node;
	token_iterator_node = tokens;
	status_code = 0;
	ctx.cmd_ptr = &current_cmd_node;
	ctx.tk_iter_ptr = &token_iterator_node;
	ctx.d_env = data_env;
	ctx.first_cmd_ptr = &first_cmd_node;
	while (*(ctx.tk_iter_ptr) != NULL)
	{
		status_code = process_current_token(&ctx);
		if (status_code == 0)
			return (NULL);
	}
	return (*(ctx.first_cmd_ptr));
}
