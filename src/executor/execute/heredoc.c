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
 * Procesa un "here document" individual.
 * Crea un archivo temporal para almacenar el contenido introducido por el
 * usuario hasta que se encuentra la cadena delimitadora especificada en
 * redir->fname (antes de ser modificado). El nombre del archivo temporal
 * generado se usa para actualizar redir->fname, de modo que el comando
 * pueda leer de este archivo como su entrada estándar.
 *
 * @param redir Puntero a la estructura de redirección que contiene el
 * delimitador (originalmente en redir->fname) y donde se
 * almacenará el nombre del archivo temporal.
 * @param hd_nbr Número secuencial para ayudar a crear un nombre único
 * para el archivo temporal del heredoc.
 * @return Retorna 0 si tiene éxito. Podría expandirse para retornar
 * códigos de error.
 */
static int	heredoc_create(t_redir *redir, int hd_nbr, t_list **heredoc_tmp_f)
{
	char	*tmp_dir;
	int		fd_tmp;
	char	*line;

	tmp_dir = ft_strjoin("/tmp/heredoc", ft_itoa(hd_nbr));
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
	hd_nbr--;
	signals_interactive();
	return (0);
}

/*
 * Itera sobre todos los comandos y sus redirecciones para procesar
 * todos los "here documents" (<< DELIMITADOR) antes de la ejecución.
 * Para cada heredoc encontrado, llama a heredoc_create para leer la entrada
 * del usuario y almacenarla en un archivo temporal.
 *
 * @param cmd Puntero al primer comando de la lista de comandos.
 * @return Retorna 0. Podría expandirse para manejar errores y
 * retornar un estado diferente.
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
