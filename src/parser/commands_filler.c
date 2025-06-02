/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_filler.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 14:16:07 by jocuni-p          #+#    #+#             */
/*   Updated: 2024/06/10 10:37:03 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** static void process_word_token(t_tok **tok, t_cmd *node, size_t *cmd_idx)
** -------------------------------------------------------------------------
** Procesa un token de tipo WORD duplicando su contenido de cadena y 
** añadiéndolo al array de argumentos del nodo de comando. Luego, avanza 
** el puntero del token e incrementa el índice de comandos.
**
** tok: Puntero al puntero del token actual. Se avanza más allá del token WORD.
** node: El nodo de comando que se va a rellenar con el argumento.
** cmd_idx: Puntero al índice actual en el array node->commands. Se incrementa.
*/
static void	process_word_token(t_tok **tok, t_cmd *node, size_t *cmd_idx)
{
	char	*word_str;

	word_str = NULL;
	if ((*tok)->str)
		word_str = ft_strdup((*tok)->str);
	node->commands[*cmd_idx] = word_str;
	(*cmd_idx)++;
	*tok = (*tok)->next;
}

/*
** static void process_redirection_token(t_tok **tok, t_cmd *node)
** ---------------------------------------------------------------
** Procesa un token de redirección (ej., >, <, >>, <<). Espera que el
** token de redirección sea seguido por un token WORD que representa el
** nombre de archivo o el delimitador del "here-document". Extrae este
** nombre de archivo/delimitador, determina si estaba entrecomillado
** (relevante para la expansión en "here-documents"), crea un nuevo nodo
** de redirección y lo añade a la lista del comando. El puntero del token
** se avanza más allá tanto del token de redirección como del nombre de
** archivo/delimitador.
**
** La expresión 'current_redir_tok->type - 2' es probablemente una
** correspondencia (mapeo) de los tipos de token (ej., T_GREAT, T_DGREAT)
** a tipos de redirección específicos (ej., REDIR_OUT, REDIR_APPEND)
** utilizados por 'redir_new_node'.
**
** La bandera 'delimiter_quoted' es crucial para los "here-documents" (<<).
** Si el delimitador estaba originalmente entrecomillado en la entrada
** (ej., cat << "EOF"), entonces la expansión de parámetros no debería
** ocurrir dentro del "here-document". Esta bandera corresponde a
** 'original_delimiter_had_quotes' en t_redir, que luego sería utilizada
** por el ejecutor.
**
** tok: Puntero al puntero del token actual. Se avanza más allá de la 
** redirección y el nombre de archivo.
** node: El nodo de comando que se va a rellenar con la redirección.
*/
static void	process_redirection_token(t_tok **tok, t_cmd *node)
{
	char	*filename_or_delimiter;
	bool	delimiter_quoted;
	t_tok	*current_redir_tok;
	t_tok	*filename_tok;

	current_redir_tok = *tok;
	filename_tok = current_redir_tok->next;
	filename_or_delimiter = NULL;
	if (filename_tok->str)
		filename_or_delimiter = ft_strdup(filename_tok->str);
	delimiter_quoted = false;
	if (filename_tok->quote_type == Q_SINGLE || \
		filename_tok->quote_type == Q_DOUBLE)
	{
		delimiter_quoted = true;
	}
	redir_add_back(&node->redir, \
		redir_new_node(filename_or_delimiter, \
		current_redir_tok->type - 2, delimiter_quoted));
	*tok = filename_tok->next;
}

/*
** void commands_filler(t_tok **tok, t_cmd *node)
** -----------------------------------------------
** Rellena la lista de argumentos de un nodo de comando (node->commands) y
** su lista de redirecciones (node->redir) iterando a través de los tokens.
** Utiliza funciones auxiliares para procesar los tokens WORD y de redirección.
** La iteración se detiene si se encuentra un token NULL_TYPE (fin de los
** tokens) o un token PIPE. Los tokens no reconocidos en este contexto
** se omiten.
**
** tok: Puntero al puntero del token actual. Será avanzado por las funciones 
** auxiliares.
** node: El nodo de comando que se va a rellenar.
*/
void	commands_filler(t_tok **tok, t_cmd *node)
{
	size_t	i;

	i = 0;
	while (*tok && (*tok)->type != NULL_TYPE)
	{
		if ((*tok)->type == WORD)
			process_word_token(tok, node, &i);
		else if (is_redirection(*tok) && (*tok)->next && \
				(*tok)->next->type == WORD)
			process_redirection_token(tok, node);
		else if ((*tok)->type == PIPE)
		{
			*tok = (*tok)->next;
			break ;
		}
		else
		{
			*tok = (*tok)->next;
		}
	}
}
