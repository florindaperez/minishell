/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_argument_assembly.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 19:54:44 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 19:54:52 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h"

/*
* Maneja el fallo de expand_argument dentro de join_next_segment.
*/
static bool	handle_expand_fail(char **assembled_str_ptr,
								t_cmd *first_cmd_to_free)
{
	msg_error_cmd("parser", "expand_argument fail", strerror(errno), 1);
	free_ptr(*assembled_str_ptr);
	g_exit_status = 1;
	free_commands(first_cmd_to_free);
	return (false);
}

/*
* Maneja el fallo de ft_strjoin dentro de join_next_segment.
*/
static bool	handle_join_fail(t_cmd *first_cmd_to_free)
{
	msg_error_cmd("parser", "ft_strjoin fail", strerror(errno), 1);
	g_exit_status = 1;
	free_commands(first_cmd_to_free);
	return (false);
}

/*
* Expande y une el siguiente segmento de token al argumento ya ensamblado.
* Libera la memoria de las cadenas intermedias.
* Parámetros:
* assembled_str_ptr: Puntero a la cadena del argumento ensamblado hasta ahora.
* next_node: Token que contiene el siguiente segmento a procesar.
* d_env: Puntero a la estructura de datos del entorno.
* first_cmd_to_free: Puntero al primer comando (para liberar en error).
* Retorno:
* true si la operación fue exitosa, false en caso de error.
* El resultado de la unión se almacena en *assembled_str_ptr.
*/
static bool	join_next_segment(char **assembled_str_ptr, t_token *next_node,
								t_data_env *d_env, t_cmd *first_cmd_to_free)
{
	char	*expanded_part;
	char	*temp_join;

	expanded_part = NULL;
	temp_join = NULL;
	expanded_part = expand_argument(next_node->args,
			next_node->quote_status, d_env);
	if (!expanded_part)
		return (handle_expand_fail(assembled_str_ptr, first_cmd_to_free));
	temp_join = ft_strjoin(*assembled_str_ptr, expanded_part);
	free_ptr(*assembled_str_ptr);
	free_ptr(expanded_part);
	*assembled_str_ptr = temp_join;
	if (!*assembled_str_ptr)
		return (handle_join_fail(first_cmd_to_free));
	return (true);
}

/*
* Ensambla una cadena de argumento a partir de uno o más tokens TOKEN_WORD
* que están concatenados (sin espacios intermedios).
* Actualiza curr_seg_node_ptr al último token consumido para este argumento.
* Parámetros:
* curr_seg_node_ptr: Puntero al iterador de tokens (apunta al primer token).
* d_env: Puntero a la estructura de datos del entorno.
* first_cmd_to_free: Puntero al primer comando de la lista (para liberar
*  en error).
* Retorno:
* Cadena ensamblada del argumento, o NULL en caso de error (ya libera comandos).
*/
char	*assemble_arg_segments(
	t_token **curr_seg_node_ptr,
	t_data_env *d_env,
	t_cmd *first_cmd_to_free)
{
	char	*assembled_str;
	t_token	*current_node;

	current_node = *curr_seg_node_ptr;
	assembled_str = NULL;
	assembled_str = expand_argument(current_node->args,
			current_node->quote_status, d_env);
	if (!assembled_str)
	{
		msg_error_cmd("parser", "expand_argument fail", strerror(errno), 1);
		g_exit_status = 1;
		free_commands(first_cmd_to_free);
		return (NULL);
	}
	while (current_node->no_space_after && current_node->next
		&& current_node->next->type == TOKEN_WORD)
	{
		current_node = current_node->next;
		if (!join_next_segment(&assembled_str, current_node, d_env,
				first_cmd_to_free))
			return (NULL);
	}
	*curr_seg_node_ptr = current_node;
	return (assembled_str);
}
