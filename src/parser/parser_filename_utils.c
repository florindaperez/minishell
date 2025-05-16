/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_filname_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 18:18:58 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 18:19:01 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>

/*
* Expande un segmento de token para usarlo en un nombre de archivo.
* Maneja errores si la expansión falla.
*/
static char	*get_expanded_segment(char *curr_fname, t_token *tk_seg,
									t_data_env *data_env, t_cmd *cmd)
{
	char	*expanded_seg;

	expanded_seg = NULL;
	expanded_seg = expand_argument(tk_seg->args,
			tk_seg->quote_status, data_env);
	if (!expanded_seg)
	{
		free_ptr(curr_fname);
		if (g_exit_status == 0 && errno == ENOMEM)
			g_exit_status = 1;
		cmd->redir_error = true;
		return (NULL);
	}
	return (expanded_seg);
}

/*
* Añade un segmento expandido al nombre de archivo actual.
*/
static char	*append_fname_seg(char *curr_fname, t_token *tk_seg,
										t_data_env *data_env, t_cmd *cmd)
{
	char	*expanded_seg;
	char	*new_fname;

	new_fname = NULL;
	expanded_seg = get_expanded_segment(curr_fname, tk_seg, data_env, cmd);
	if (!expanded_seg)
		return (NULL);
	if (!curr_fname)
		return (expanded_seg);
	new_fname = ft_strjoin(curr_fname, expanded_seg);
	free_ptr(curr_fname);
	free_ptr(expanded_seg);
	if (!new_fname)
	{
		msg_error_cmd("parser", "strjoin for filename", strerror(errno), 1);
		g_exit_status = 1;
		cmd->redir_error = true;
		return (NULL);
	}
	return (new_fname);
}

/*
* Ensambla un nombre de archivo completo a partir de uno o más tokens 
* TOKEN_WORD. Maneja tokens concatenados (ej. "archivo"nombre o 
* archivo"nombre").
* El puntero curr_part_ptr se avanza hasta el último token consumido.
*/
char	*assemble_filename(t_token **curr_part_ptr, t_data_env *data_env,
							t_quote_type *initial_q_status, t_cmd *cmd)
{
	char	*acc_fname;
	t_token	*tk_iter;

	tk_iter = *curr_part_ptr;
	*initial_q_status = tk_iter->quote_status;
	acc_fname = NULL;
	acc_fname = append_fname_seg(acc_fname, tk_iter, data_env, cmd);
	if (!acc_fname)
		return (NULL);
	while (tk_iter->no_space_after == true
		&& tk_iter->next != NULL
		&& tk_iter->next->type == TOKEN_WORD)
	{
		tk_iter = tk_iter->next;
		acc_fname = append_fname_seg(acc_fname, tk_iter,
				data_env, cmd);
		if (!acc_fname)
		{
			*curr_part_ptr = tk_iter;
			return (NULL);
		}
	}
	*curr_part_ptr = tk_iter;
	return (acc_fname);
}

/*
* Verifica si un nombre de archivo no entrecomillado se expande a múltiples 
* campos.
*/
static bool	check_multifield_unquoted(char *fname,
										t_token *first_fname_tk, t_cmd *cmd)
{
	int		field_count;
	bool	in_field_flag;
	char	*ptr;

	field_count = 0;
	in_field_flag = false;
	ptr = fname;
	while (ptr && *ptr)
	{
		if (!ft_isspace((unsigned char)*ptr) && !in_field_flag)
		{
			field_count++;
			in_field_flag = true;
		}
		else if (ft_isspace((unsigned char)*ptr))
			in_field_flag = false;
		ptr++;
	}
	if (field_count <= 1)
		return (false);
	msg_error_cmd(first_fname_tk->args, NULL,
		"ambiguous redirect", 1);
	g_exit_status = 1;
	cmd->redir_error = true;
	return (true);
}

/*
* Verifica si una redirección es ambigua.
* Una redirección es ambigua si una variable sin comillas se expande a nada
* o a múltiples palabras.
*/
bool	check_ambiguous_redirect(char *fname, t_quote_type q_status,
									t_token *first_fname_tk, t_cmd *cmd)
{
	if (fname && fname[0] == '\0' && q_status == NO_QUOTE)
	{
		msg_error_cmd(first_fname_tk->args, NULL, "ambiguous redirect", 1);
		g_exit_status = 1;
		cmd->redir_error = true;
		return (true);
	}
	if (fname && q_status == NO_QUOTE)
	{
		if (check_multifield_unquoted(fname, first_fname_tk, cmd))
			return (true);
	}
	return (false);
}
