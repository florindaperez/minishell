/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_new_node.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 10:22:21 by jocuni-p          #+#    #+#             */
/*   Updated: 2024/05/29 10:23:24 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*------Returns a new initialized t_redir node-----
* Modificado para que acepte el tercer argumento 'bool had_quotes' y lo asigne 
*	al nuevo campo en la  estructura t_redir.
*	es importante, especialmente para los "here-documents", ya que indica si 
*	el delimitador estaba entrecomillado, lo que a su vez determina si se deben
*	expandir o no las variables dentro del here-document.
*/
t_redir	*redir_new_node(char *str, int redir_type, bool had_quotes)
{
	t_redir	*node;

	node = (t_redir *)p_malloc(sizeof(t_redir));
	node->fname = str;
	node->redir_type = (t_redir_type)redir_type;
	node->original_delimiter_had_quotes = had_quotes;
	node->next = NULL;
	return (node);
}
