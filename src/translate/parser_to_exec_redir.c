/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_to_exec_redir.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 13:03:35 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/29 13:03:41 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * Maneja la redirección de tipo REDIR_INPUT.
 * Actualiza exec_io->infile y resetea la configuración de heredoc.
 *
 * @param exec_io Puntero a la estructura t_cmd_io_exe a actualizar.
 * @param redir_node Puntero al nodo de redirección t_redir actual.
 * @return EXIT_SUCCESS o EXIT_FAILURE si ft_strdup falla.
 */
static int	handle_redir_input(t_cmd_io_exe *exec_io, t_redir *redir_node)
{
	free_ptr(exec_io->infile);
	exec_io->infile = ft_strdup(redir_node->fname);
	if (!exec_io->infile && redir_node->fname)
		return (EXIT_FAILURE);
	free_ptr(exec_io->heredoc_delimiter);
	exec_io->heredoc_delimiter = NULL;
	exec_io->heredoc_quotes = false;
	return (EXIT_SUCCESS);
}

/*
 * Maneja la redirección de tipo REDIR_OUTPUT.
 * Actualiza exec_io->outfile y establece append_mode a false.
 *
 * @param exec_io Puntero a la estructura t_cmd_io_exe a actualizar.
 * @param redir_node Puntero al nodo de redirección t_redir actual.
 * @return EXIT_SUCCESS o EXIT_FAILURE si ft_strdup falla.
 */
static int	handle_redir_output(t_cmd_io_exe *exec_io, t_redir *redir_node)
{
	free_ptr(exec_io->outfile);
	exec_io->outfile = ft_strdup(redir_node->fname);
	if (!exec_io->outfile && redir_node->fname)
		return (EXIT_FAILURE);
	exec_io->append_mode = false;
	return (EXIT_SUCCESS);
}

/*
 * Maneja la redirección de tipo REDIR_OUTPUT_APPEND.
 * Actualiza exec_io->outfile y establece append_mode a true.
 *
 * @param exec_io Puntero a la estructura t_cmd_io_exe a actualizar.
 * @param redir_node Puntero al nodo de redirección t_redir actual.
 * @return EXIT_SUCCESS o EXIT_FAILURE si ft_strdup falla.
 */
static int	handle_redir_output_append(t_cmd_io_exe *exec_io, \
										t_redir *redir_node)
{
	free_ptr(exec_io->outfile);
	exec_io->outfile = ft_strdup(redir_node->fname);
	if (!exec_io->outfile && redir_node->fname)
		return (EXIT_FAILURE);
	exec_io->append_mode = true;
	return (EXIT_SUCCESS);
}

/*
 * Rellena la estructura de E/S del ejecutor (t_cmd_io_exe)
 * a partir de la lista de redirecciones del parser (t_redir).
 *
 * @param exec_io Puntero a t_cmd_io_exe que se va a rellenar.
 * @param redir_list Puntero a la lista de redirecciones t_redir.
 * @return EXIT_SUCCESS si todo va bien, EXIT_FAILURE si falla ft_strdup.
 */
int	fill_exec_io_from_redirections(t_cmd_io_exe *exec_io, \
										t_redir *redir_list)
{
	t_redir	*current_redir;
	int		status;

	current_redir = redir_list;
	status = EXIT_SUCCESS;
	while (current_redir && status == EXIT_SUCCESS)
	{
		if (!current_redir->fname)
		{
			current_redir = current_redir->next;
			continue ;
		}
		if (current_redir->redir_type == REDIR_INPUT
			||current_redir->redir_type == HEREDOC_INPUT)
			status = handle_redir_input(exec_io, current_redir);
		else if (current_redir->redir_type == REDIR_OUTPUT)
			status = handle_redir_output(exec_io, current_redir);
		else if (current_redir->redir_type == REDIR_OUTPUT_APPEND)
			status = handle_redir_output_append(exec_io, current_redir);
		current_redir = current_redir->next;
	}
	return (status);
}
