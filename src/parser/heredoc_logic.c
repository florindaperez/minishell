/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_logic.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 00:26:14 by flperez-          #+#    #+#             */
/*   Updated: 2025/04/25 00:31:40 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h"

/*
* Detecta si el delimitador raw contiene comillas.
*/
bool	detect_heredoc_quotes(const char *delimiter_raw)
{
	size_t	len;

	if (!delimiter_raw)
		return (false);
	len = ft_strlen(delimiter_raw);
	if (len < 2)
		return (false);
	if ((delimiter_raw[0] == '\'' && delimiter_raw[len - 1] == '\'')
		|| (delimiter_raw[0] == '"' && delimiter_raw[len - 1] == '"'))
	{
		return (true);
	}
	return (false);
}

/*
* Elimina las comillas externas del string si existen.
*/
char	*remove_quotes(const char *str_raw)
{
	size_t	len;
	char	*new_str;

	if (!str_raw)
		return (NULL);
	len = ft_strlen(str_raw);
	new_str = NULL;
	if (len >= 2
		&& ((str_raw[0] == '\'' && str_raw[len - 1] == '\'')
			|| (str_raw[0] == '"' && str_raw[len - 1] == '"')))
	{
		new_str = ft_strndup(str_raw + 1, len - 2);
		if (!new_str)
			msg_error_cmd("heredoc", "malloc failed in remove_quotes",
				strerror(errno), EXIT_FAILURE);
		return (new_str);
	}
	else
	{
		new_str = ft_strdup(str_raw);
		if (!new_str)
			msg_error_cmd("heredoc", "malloc failed in remove_quotes",
				strerror(errno), EXIT_FAILURE);
		return (new_str);
	}
}

/*
* Genera un nombre de archivo temporal único para heredoc.
*/
char	*generate_heredoc_filename(void)
{
	char	template[31];
	int		fd;
	char	*filename_dup;

	ft_strncpy(template, "/tmp/minishell_heredoc_XXXXXX", sizeof(template) - 1);
	template[sizeof(template) - 1] = '\0';
	fd = mkstemp(template);
	if (fd == -1)
	{
		perror("minishell: mkstemp failed for heredoc");
		return (NULL);
	}
	close(fd);
	filename_dup = ft_strdup(template);
	if (!filename_dup)
	{
		msg_error_cmd("heredoc", "strdup failed for heredoc filename",
			strerror(errno), EXIT_FAILURE);
		return (NULL);
	}
	return (filename_dup);
}

/*
*  Expande variables en una línea de heredoc.
*/
char	*expand_heredoc_line(char *line, t_data_env *data_env)
{
	(void)data_env;
	return (ft_strdup(line));
}

/*
* Función principal que maneja la lectura y escritura del heredoc.
*/
bool	fill_heredoc(t_data_env *data_env, t_cmd_io *io, int tmp_fd)
{
	char	*line_read;
	bool	overall_success;

	if (!io || !io->heredoc_delimiter)
		return (false);
	overall_success = false;
	while (true)
	{
		line_read = readline("> ");
		if (check_heredoc_termination(line_read, io, &overall_success))
			break ;
		if (!expand_and_write_line(line_read, io, data_env, tmp_fd))
		{
			overall_success = false;
			break ;
		}
	}
	return (overall_success);
}
