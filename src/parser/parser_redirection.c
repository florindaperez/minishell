/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:03:07 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 17:03:10 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h"

/*
* Configura una redirección de tipo "here document" ('<<').
* Almacena el delimitador y marca si estaba entrecomillado.
*/
static int	handle_heredoc_setup(t_cmd *cmd, t_token *delimiter_token)
{
	cleanup_previous_io(cmd->io, true);
	cmd->io->heredoc_delimiter = ft_strdup(delimiter_token->args);
	if (!cmd->io->heredoc_delimiter)
	{
		msg_error_cmd("parser", "strdup for heredoc delim",
			strerror(errno), 1);
		g_exit_status = 1;
		cmd->redir_error = true;
		return (0);
	}
	cmd->io->heredoc_quotes = (delimiter_token->quote_status != NO_QUOTE);
	cmd->io->fd_in = -1;
	return (1);
}

/*
* Procesa redirecciones de archivo generales ('<', '>', '>>').
* Ensambla el nombre del archivo, verifica ambigüedades y aplica la redirección.
*/
static int	process_general_redir(
	t_cmd *cmd,
	t_token *redir_tk,
	t_token **fname_tks_ptr,
	t_data_env *data_env)
{
	t_token			*actual_fname_start_tk;
	t_token			*current_fname_node;
	char			*final_fname;
	t_quote_type	initial_q_status;

	actual_fname_start_tk = *fname_tks_ptr;
	current_fname_node = actual_fname_start_tk;
	final_fname = NULL;
	initial_q_status = NO_QUOTE;
	final_fname = assemble_filename(&current_fname_node, data_env,
			&initial_q_status, cmd);
	*fname_tks_ptr = current_fname_node;
	if (!final_fname)
		return (0);
	if (check_ambiguous_redirect(final_fname, initial_q_status,
			actual_fname_start_tk, cmd))
	{
		free_ptr(final_fname);
		return (-1);
	}
	if (apply_file_redirection(cmd, redir_tk, final_fname) == -1)
		return (-1);
	return (1);
}

/*
* Valida la sintaxis después de un operador de redirección.
* Comprueba que el siguiente token sea una palabra (nombre de 
* archivo/delimitador).
*/
static bool	check_syntax_after_redir_op(t_token *fname_start_val, t_cmd *cmd)
{
	char	*err_arg;

	if (!fname_start_val || fname_start_val->type != TOKEN_WORD)
	{
		if (fname_start_val)
			err_arg = fname_start_val->args;
		else
			err_arg = "newline";
		msg_error_cmd(PROG_NAME, "syntax error near token", err_arg, 2);
		g_exit_status = 2;
		cmd->redir_error = true;
		return (false);
	}
	return (true);
}

/*
* Función principal para manejar todos los tokens de redirección.
* Es llamada desde el bucle principal del parser.
* Actualiza *tokens_ptr para apuntar al token que sigue a la secuencia de
* redirección completa (operador + nombre de archivo/delimitador).
*/
int	handle_redirection(
	t_cmd *cmd,
	t_token **tokens_ptr,
	t_data_env *data_env)
{
	t_token	*redir_tk_val;
	t_token	*fname_start_val;
	int		status;
	t_token	*last_fname_token;

	redir_tk_val = *tokens_ptr;
	fname_start_val = redir_tk_val->next;
	status = 0;
	if (!check_syntax_after_redir_op(fname_start_val, cmd))
		return (0);
	if (redir_tk_val->type == TOKEN_HEREDOC)
	{
		status = handle_heredoc_setup(cmd, fname_start_val);
		*tokens_ptr = fname_start_val->next;
		return (status);
	}
	last_fname_token = fname_start_val;
	status = process_general_redir(cmd, redir_tk_val, &last_fname_token,
			data_env);
	*tokens_ptr = last_fname_token->next;
	return (status);
}
