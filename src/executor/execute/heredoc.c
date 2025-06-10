/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:37:13 by flperez-          #+#    #+#             */
/*   Updated: 2025/06/02 17:37:20 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * heredoc_create
 * Procesa un "here document" individual.
 * Crea un fichero temporal, lee la entrada del usuario hasta encontrar el
 * delimitador y actualiza el nombre del fichero en el nodo de redirección.
 * También registra el fichero temporal para su posterior limpieza.
 *
 * @param redir Puntero a la estructura de redirección (t_redir) que
 * contiene el delimitador y donde se almacenará el
 * nombre del fichero temporal.
 * @param hd_nbr Número secuencial para crear un nombre de fichero
 * temporal único.
 * @param heredoc_tmp_f Puntero a la lista que almacena los nombres de
 * todos los ficheros temporales creados.
 * @return Retorna 0 si tiene éxito.
 */
static int	heredoc_create(t_redir *redir, int hd_nbr, t_list **heredoc_tmp_f)
{
	char	*tmp_dir;
	int		fd_tmp;
	char	*line;
	char	*hd_nbr_str;

	hd_nbr_str = ft_itoa(hd_nbr);
	tmp_dir = ft_strjoin("/tmp/heredoc", hd_nbr_str);
	free(hd_nbr_str);
	ft_lstadd_back(heredoc_tmp_f, ft_lstnew(ft_strdup(tmp_dir)));
	fd_tmp = open(tmp_dir, O_CREAT | O_WRONLY | O_TRUNC, 0660);
	line = readline("> ");
	while (line && ft_strcmp(line, redir->fname) != 0)
	{
		write(fd_tmp, line, ft_strlen(line));
		write(fd_tmp, "\n", 1);
		free(line);
		line = readline("> ");
	}
	close(fd_tmp);
	free(line);
	free(redir->fname);
	redir->fname = ft_strdup(tmp_dir);
	free(tmp_dir);
	signals_interactive();
	return (0);
}

/*
 * heredoc
 * Itera sobre todos los comandos y sus redirecciones para procesar
 * todos los "here documents" (<< DELIMITADOR) antes de la ejecución.
 *
 * @param cmd Puntero al primer comando de la lista de comandos.
 * @param heredoc_tmp_f Puntero a la lista donde se registrarán todos
 * los ficheros temporales de heredoc.
 * @return Retorna siempre 0.
 */
int	heredoc(t_cmd *cmd, t_list **heredoc_tmp_f)
{
	t_redir	*aux;
	int		hd_nbr;

	hd_nbr = 0;
	while (cmd)
	{
		aux = cmd->redir;
		while (aux)
		{
			if (aux->redir_type == HEREDOC_INPUT)
			{
				heredoc_create(aux, hd_nbr, heredoc_tmp_f);
				hd_nbr++;
			}
			aux = aux->next;
		}
		cmd = cmd->next;
	}
	return (0);
}
