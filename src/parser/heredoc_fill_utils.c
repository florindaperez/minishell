/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_fill_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 11:52:05 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 11:52:08 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h" 
#include "minishell_parser.h" 

/*
*Comprueba si la línea leída termina el heredoc (EOF o delimitador).
* Actualiza overall_success y es responsable de liberar line_read si es el 
* delimitador.
* return true si el bucle debe terminar, false para continuar.
*/
bool	check_heredoc_termination(char *line_r, t_cmd_io *io, bool *o_success)
{
	if (line_r == NULL)
	{
		ft_dprintf(STDERR_FILENO, "minishell: warning: here-document "
			"delimited by end-of-file (wanted `%s')\n",
			io->heredoc_delimiter);
		*o_success = true;
		return (true);
	}
	if (ft_strcmp(line_r, io->heredoc_delimiter) == 0)
	{
		*o_success = true;
		free_ptr(line_r);
		return (true);
	}
	return (false);
}

/*
* Expande (si es necesario) y escribe una línea al heredoc.
* Maneja la liberación de line_read (o su versión expandida, line_to_write).
* return true en éxito, false en fallo (lo que debería detener fill_heredoc).
*/
bool	expand_and_write_line(char *line_r, t_cmd_io *io, \
								t_data_env *env, int fd)
{
	char	*line_to_write;
	int		bytes_written;

	if (!io->heredoc_quotes)
	{
		line_to_write = expand_heredoc_line(line_r, env);
		free_ptr(line_r);
	}
	else
		line_to_write = line_r;
	if (!line_to_write)
		return (false);
	bytes_written = write(fd, line_to_write, ft_strlen(line_to_write));
	if (bytes_written != -1)
		bytes_written = write(fd, "\n", 1);
	if (bytes_written == -1)
	{
		perror("minishell: write error filling heredoc");
		free_ptr(line_to_write);
		return (false);
	}
	free_ptr(line_to_write);
	return (true);
}
