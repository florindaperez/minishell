/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 19:02:11 by flperez-          #+#    #+#             */
/*   Updated: 2025/06/10 19:02:14 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
* msg_error_heredoc
* Imprime un mensaje de advertencia estándar cuando un heredoc es
* interrumpido por un EOF (Ctrl+D) en lugar de por su delimitador.
*
* delimiter: El delimitador que se esperaba para cerrar el heredoc.
*/
void	msg_error_heredoc(char *delimiter)
{
	ft_putstr_fd("minishell: warning: here-document delimited by end-of-file",
		2);
	ft_putstr_fd(" (wanted `", 2);
	ft_putstr_fd(delimiter, 2);
	ft_putstr_fd("')\n", 2);
}

/*
* create_tmp_file
* Crea un fichero temporal único en /tmp para ser usado por un heredoc.
* El nombre del fichero se genera de forma única en cada llamada.
*
* filename_ptr: Puntero a un char* donde se almacenará el nombre del
* fichero temporal creado.
*
* Retorna: El descriptor de fichero del nuevo archivo, o -1 en caso de error.
*/
int	create_tmp_file(char **filename_ptr)
{
	static unsigned int	i = 0;
	char				*num;
	char				*path;
	int					fd;

	num = ft_itoa(i++);
	if (!num)
		return (-1);
	path = ft_strjoin("/tmp/.minishell_heredoc_", num);
	free(num);
	if (!path)
		return (-1);
	fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
	{
		free(path);
		return (-1);
	}
	*filename_ptr = path;
	return (fd);
}

/*
* add_heredoc_tmp_f
* Añade el nombre de un fichero temporal a una lista. Esta lista se usa
* para poder eliminar todos los ficheros temporales al final.
*
* list:     Puntero al puntero de la cabeza de la lista (t_list).
* filename: El nombre del fichero a añadir.
*/
void	add_heredoc_tmp_f(t_list **list, char *filename)
{
	t_list	*new_node;

	new_node = ft_lstnew(filename);
	if (!new_node)
	{
		free(filename);
		return ;
	}
	ft_lstadd_back(list, new_node);
}
