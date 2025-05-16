/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_token_handlers.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 21:13:07 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 21:13:11 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h"

/*
* Verifica la sintaxis de un token de operador. Si hay un error,
* muestra un mensaje, establece el estado de salida y libera los comandos.
*
*/
static bool	check_operator_syntax_error(t_cmd *curr_cmd, t_token *op_tk,
										t_cmd *first_cmd_list)
{
	char	*err_token_arg;

	err_token_arg = NULL;
	if (curr_cmd->args == NULL && !curr_cmd->redir_error
		&& (op_tk->prev == NULL || op_tk->prev->type == TOKEN_PIPE
			|| op_tk->prev->type == TOKEN_SEMICOLON))
	{
		if (op_tk->args)
			err_token_arg = op_tk->args;
		else if (op_tk->type == TOKEN_PIPE)
			err_token_arg = "|";
		else
			err_token_arg = ";";
		msg_error_cmd(PROG_NAME, "syntax error near unexpected token",
			err_token_arg, 2);
		g_exit_status = 2;
		free_commands(first_cmd_list);
		return (true);
	}
	return (false);
}

/*
* Maneja los tokens de operador (PIPE o SEMICOLON).
* Crea un nuevo comando si la sintaxis es válida.
*/
t_cmd	*handle_operator_token(
	t_cmd *curr_cmd,
	t_token **tk_iter_ptr,
	t_cmd *first_cmd_list)
{
	t_cmd	*next_cmd_node;
	t_token	*op_tk;

	op_tk = *tk_iter_ptr;
	next_cmd_node = NULL;
	if (check_operator_syntax_error(curr_cmd, op_tk, first_cmd_list))
		return (NULL);
	next_cmd_node = new_command();
	if (!next_cmd_node)
	{
		free_commands(first_cmd_list);
		return (NULL);
	}
	curr_cmd->next = next_cmd_node;
	next_cmd_node->prev = curr_cmd;
	*tk_iter_ptr = op_tk->next;
	return (next_cmd_node);
}

/*
* Maneja un token no esperado o no reconocido durante el parseo.
* Muestra un mensaje de error y libera los comandos.
*/
int	handle_unhandled_token(
	t_token *token,
	t_cmd *first_cmd_list)
{
	char	*err_arg;

	if (token->args)
		err_arg = token->args;
	else
		err_arg = "<?>";
	msg_error_cmd(PROG_NAME, "syntax error near unhandled token",
		err_arg, 2);
	g_exit_status = 2;
	free_commands(first_cmd_list);
	return (0);
}

/*
* Procesa un token de redirección.
* Llama a handle_redirection y gestiona errores.
*/
int	process_redir_token(
	t_cmd *cmd,
	t_token **tk_iter_ptr,
	t_data_env *d_env,
	t_cmd *first_cmd_list)
{
	int	result;

	result = handle_redirection(cmd, tk_iter_ptr, d_env);
	if (result == 0)
	{
		free_commands(first_cmd_list);
		return (0);
	}
	if (result == -1)
	{
		cmd->redir_error = true;
		return (-1);
	}
	return (1);
}
