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
* read_heredoc_loop
* Lee la entrada del heredoc línea por línea. Distingue entre Ctrl+C y Ctrl+D
* al comprobar el valor de la variable global g_get_signal.
*
* tmp_fd:    Descriptor de fichero del archivo temporal donde escribir.
* delimiter: La cadena que finaliza la entrada del heredoc.
*/
static void	read_heredoc_loop(int tmp_fd, char *delimiter)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (line == NULL)
		{
			if (g_get_signal != 130)
				msg_error_heredoc(delimiter);
			break ;
		}
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write(tmp_fd, line, ft_strlen(line));
		write(tmp_fd, "\n", 1);
		free(line);
	}
}

/*
* read_heredoc
* Maneja la lectura de un 'here document'. Guarda el estado de la entrada
* estándar, configura las señales, ejecuta el bucle de lectura y finalmente
* restaura el estado original, gestionando correctamente la memoria.
*
* redir:          Puntero a la estructura de redirección actual.
* heredoc_tmp_f:  Puntero a la lista de nombres de ficheros temporales.
*
* Retorna: 0 en éxito, 1 si ocurre un error o una interrupción.
*/
static int	read_heredoc(t_redir *redir, t_list **heredoc_tmp_f)
{
	char	*tmp_filename;
	int		tmp_fd;
	int		original_stdin;

	tmp_filename = NULL;
	original_stdin = dup(STDIN_FILENO);
	if (original_stdin == -1)
		return (msg_error_cmd("heredoc", "dup", "failed", 1), 1);
	tmp_fd = create_tmp_file(&tmp_filename);
	if (tmp_fd == -1)
		return (close(original_stdin), 1);
	signals_heredoc();
	read_heredoc_loop(tmp_fd, redir->fname);
	close(tmp_fd);
	dup2(original_stdin, STDIN_FILENO);
	close(original_stdin);
	signals_interactive();
	if (g_get_signal == 130)
		return (free(tmp_filename), 1);
	add_heredoc_tmp_f(heredoc_tmp_f, tmp_filename);
	free(redir->fname);
	redir->fname = ft_strdup(tmp_filename);
	return (0);
}

/*
* heredoc
* Función principal que itera a través de todos los comandos y redirecciones
* para procesar cualquier heredoc antes de la ejecución.
*
* cmd_list:       La cabeza de la lista de comandos.
* heredoc_tmp_f:  Puntero a la lista que almacenará los nombres de los
* ficheros temporales creados.
*
* Retorna: 0 si todos los heredocs se procesan con éxito, 1 si alguno falla.
*/
int	heredoc(t_cmd *cmd_list, t_list **heredoc_tmp_f)
{
	t_cmd	*cmd;
	t_redir	*redir;

	g_get_signal = 0;
	cmd = cmd_list;
	while (cmd)
	{
		redir = cmd->redir;
		while (redir)
		{
			if (redir->redir_type == HEREDOC_INPUT)
			{
				if (read_heredoc(redir, heredoc_tmp_f) != 0)
					return (1);
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (0);
}
