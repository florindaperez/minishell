/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_to_exec_convert.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 13:02:59 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/29 13:03:04 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* parser_to_exec_convert.c                           :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/05/29 13:02:59 by flperez-          #+#    #+#             */
/* Updated: 2025/05/29 13:03:04 by flperez-         ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * Inicializa una nueva estructura t_cmd_exe y su subestructura t_cmd_io_exe.
 * Asigna memoria y establece valores iniciales (punteros a NULL, pids/fds a -1).
 * @return Puntero a la t_cmd_exe inicializada, o NULL si falla la asignación.
 */
static t_cmd_exe	*init_struct_exec_cmd(void)
{
	t_cmd_exe	*exec_cmd;

	exec_cmd = (t_cmd_exe *)p_malloc(sizeof(t_cmd_exe));
	if (!exec_cmd)
		return (NULL);
	ft_memset(exec_cmd, 0, sizeof(t_cmd_exe));
	exec_cmd->io = (t_cmd_io_exe *)p_malloc(sizeof(t_cmd_io_exe));
	if (!exec_cmd->io)
	{
		free(exec_cmd);
		return (NULL);
	}
	ft_memset(exec_cmd->io, 0, sizeof(t_cmd_io_exe));
	exec_cmd->io->fd_in = -1;
	exec_cmd->io->fd_out = -1;
	exec_cmd->pid = -1;
	return (exec_cmd);
}

/*
 * Libera los recursos de un t_cmd_exe en caso de error durante la inicialización
 * de la traducción y retorna NULL.
 * @param exec_cmd El comando parcialmente inicializado cuyos recursos se 
 * 	liberarán.
 * @param args_were_set Indica si el campo args de exec_cmd ya había sido 
 * asignado y necesita ser liberado.
 * @return Siempre retorna NULL.
 */
static t_cmd_exe	*handle_translation_error(t_cmd_exe *exec_cmd, \
	bool args_were_set)
{
	if (exec_cmd)
	{
		if (args_were_set && exec_cmd->args)
		{
			free_arg_info_array(exec_cmd->args);
			exec_cmd->args = NULL;
		}
		if (exec_cmd->io)
			free(exec_cmd->io);
		free(exec_cmd);
	}
	return (NULL);
}

/*
 * Traduce un único nodo de comando del formato "parseo" (t_cmd) al formato
 * del ejecutor executor (t_cmd_exe).
 *
 * @param b_cmd_node Puntero al nodo t_cmd del formato "parseo" a traducir.
 * @return Un puntero al nuevo nodo t_cmd_exe traducido, o NULL si
 * b_cmd_node es NULL o si ocurre un error de asignación de memoria.
 */
static t_cmd_exe	*trans_cmd_pa_to_cmd_exe(t_cmd *b_cmd_node)
{
	t_cmd_exe	*exec_cmd;

	if (!b_cmd_node)
		return (NULL);
	exec_cmd = init_struct_exec_cmd();
	if (!exec_cmd)
		return (NULL);
	exec_cmd->args = transl_args_p_to_args_e(
			b_cmd_node->commands);
	if (!exec_cmd->args && b_cmd_node->commands
		&& b_cmd_node->commands[0])
		return (handle_translation_error(exec_cmd, false));
	if (fill_exec_io_from_redirections(exec_cmd->io,
			b_cmd_node->redir) != EXIT_SUCCESS)
		return (handle_translation_error(exec_cmd, true));
	if (b_cmd_node->commands && b_cmd_node->commands[0])
		exec_cmd->is_builtin = is_builtins(b_cmd_node);
	else
		exec_cmd->is_builtin = false;
	return (exec_cmd);
}

/*
 * Añade un nodo t_cmd_exe al final de una lista doblemente enlazada.
 * Actualiza los punteros head y tail de la lista según sea necesario.
 *
 * @param head Puntero al puntero de la cabeza de la lista.
 * @param tail Puntero al puntero de la cola de la lista.
 * @param new_node Puntero al nuevo nodo t_cmd_exe a añadir.
 */
static void	add_node_exec_cmd(t_cmd_exe **head, t_cmd_exe **tail,
	t_cmd_exe *new_node)
{
	if (!*head)
	{
		*head = new_node;
		*tail = new_node;
	}
	else
	{
		(*tail)->next = new_node;
		new_node->prev = *tail;
		*tail = new_node;
	}
}

/*
 * Función principal que convierte una lista enlazada de comandos del formato
 * "parseo" (t_cmd) a una lista enlazada de comandos para el ejecutor executor
 * (t_cmd_exe).
 *
 * @param cmd_list_head Puntero a la cabeza de la lista t_cmd.
 * @return Un puntero a la cabeza de la nueva lista t_cmd_exe,
 * o NULL si la lista original es vacía o si ocurre un error.
 */
t_cmd_exe	*convert_cmd_list_to_cms_list_exec(
	struct s_cmd *cmd_list_head)
{
	t_cmd_exe	*exec_cmds_head;
	t_cmd_exe	*exec_cmds_tail;
	t_cmd_exe	*new_exec_cmd_node;
	t_cmd		*b_cmd_iter;

	if (!cmd_list_head)
		return (NULL);
	exec_cmds_head = NULL;
	exec_cmds_tail = NULL;
	b_cmd_iter = cmd_list_head;
	while (b_cmd_iter)
	{
		new_exec_cmd_node = trans_cmd_pa_to_cmd_exe(b_cmd_iter);
		if (!new_exec_cmd_node)
		{
			free_cmd_list(exec_cmds_head);
			return (NULL);
		}
		add_node_exec_cmd(&exec_cmds_head, &exec_cmds_tail,
			new_exec_cmd_node);
		b_cmd_iter = b_cmd_iter->next;
	}
	return (exec_cmds_head);
}
