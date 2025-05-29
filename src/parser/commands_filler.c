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

void	commands_filler(t_tok **tok, t_cmd *node)
{
	size_t	i;
	char	*tp;

	i = 0;
	while (*tok && (*tok)->type != NULL_TYPE)
	{
		if ((*tok)->type == WORD)
		{
			node->commands[i] = ft_strdup((*tok)->str);
			i++;
			*tok = (*tok)->next;
		}
	else if (is_redirection(*tok) && (*tok)->next && (*tok)->next->type == WORD) // Añadida comprobación para (*tok)->next
	{
    	tp = ft_strdup((*tok)->next->str);
    	bool delimiter_was_quoted = false;

    	// Determinar si el delimitador/nombre de archivo estaba entrecomillado
    	// Esto es crucial para los here-documents (tipo D_SMALLER).
    	// Para otros tipos de redirección, el comportamiento de expansión del nombre
    	// del archivo por el 'expander' podría ser diferente.
    	if ((*tok)->next->quote_type == Q_SINGLE || (*tok)->next->quote_type == Q_DOUBLE)
   		{
        	delimiter_was_quoted = true;
    	}
    	// Si es un here-document (D_SMALLER), este 'delimiter_was_quoted' se usará
    	// para el campo 'original_delimiter_had_quotes' en t_redir.
    	// Este campo es el que tu t_cmd_io_exe->heredoc_quotes usará.

    	redir_add_back(&node->redir, redir_new_node(tp, (*tok)->type - 2, delimiter_was_quoted));
    	*tok = (*tok)->next->next;
	}
		else if ((*tok)->type == PIPE)
		{
			*tok = (*tok)->next;
			break ;
		}
	}
}

/*
{
	s

	node->commands[*i] = ft_strdup((*tok)->str);
	*tok = (*tok)->next;
	(*i)++;
}*/