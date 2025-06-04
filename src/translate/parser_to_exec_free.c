/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_to_exec_free.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 13:03:55 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/29 13:03:58 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

static void	free_cmd_io(t_cmd_io_exe *io);
static void	free_pwd_oldpwd_in_data(t_data_env_exe *data);

/*
 * Libera la memoria de un array de estructuras t_arg_info_exe.
 * Itera sobre el array, liberando el campo 'value' de cada elemento
 * y luego el elemento mismo. Finalmente, libera el array de punteros.
 *
 * @param flp_args El array de punteros a t_arg_info_exe a liberar.
 * Si es NULL, la función no hace nada.
 */
void	free_arg_info_array(t_arg_info_exe **flp_args)
{
	int	i;

	if (!flp_args)
		return ;
	i = 0;
	while (flp_args[i])
	{
		if (flp_args[i]->value)
			free(flp_args[i]->value);
		free(flp_args[i]);
		flp_args[i] = NULL;
		i++;
	}
	free(flp_args);
}

/*
 * Libera la memoria de una estructura t_cmd_io_exe.
 * Libera las cadenas infile, outfile y heredoc_delimiter si existen.
 * Los descriptores de archivo (fd_in, fd_out) y los backups de stdio
 * no se gestionan aquí ya que no son memoria asignada dinámicamente
 * por esta estructura de la misma manera.
 *
 * @param io Puntero a la estructura t_cmd_io_exe a liberar.
 */
static void	free_cmd_io(t_cmd_io_exe *io)
{
	if (!io)
		return ;
	if (io->infile)
		free(io->infile);
	if (io->outfile)
		free(io->outfile);
	if (io->heredoc_delimiter)
		free(io->heredoc_delimiter);
	free(io);
}

/*
 * Libera una lista enlazada de estructuras t_cmd_exe.
 * Itera sobre la lista, liberando para cada nodo: el path, los argumentos
 * (usando free_arg_info_array), y la estructura de E/S (usando free_cmd_io).
 * Importante: El campo 'cmd' (puntero a t_cmd original del parser)
 * no se libera aquí, ya que su gestión de memoria corresponde al parser.
 *
 * @param flp_cmd_list_head Puntero al primer nodo de la lista a liberar.
 */
void	free_cmd_list(t_cmd_exe *flp_cmd_list_head)
{
	t_cmd_exe	*current;
	t_cmd_exe	*next_node;

	current = flp_cmd_list_head;
	while (current)
	{
		next_node = current->next;
		if (current->path)
			free(current->path);
		if (current->args)
			free_arg_info_array(current->args);
		if (current->io)
			free_cmd_io(current->io);
		free(current);
		current = next_node;
	}
}

/*
 * Libera las cadenas PWD y OLDPWD dentro de la estructura t_data_env_exe
 * si han sido asignadas dinámicamente.
 *
 * @param data Puntero a la estructura t_data_env_exe cuyo contenido se liberará.
 */
static void	free_pwd_oldpwd_in_data(t_data_env_exe *data)
{
	if (data->pwd)
	{
		free(data->pwd);
		data->pwd = NULL;
	}
	if (data->old_pwd)
	{
		free(data->old_pwd);
		data->old_pwd = NULL;
	}
}

/*
 * Libera el contenido dinámicamente asignado dentro de una estructura
 * t_data_env_exe. Esto incluye la lista de comandos del ejecutor,
 * el array de entorno (envp) preparado para execve, y las cadenas PWD y OLDPWD.
 * La estructura t_data_env_exe en sí no se libera aquí, asumiendo que
 * podría no ser un puntero (estar en la pila) o su gestión es externa.
 *
 * @param data Puntero a la estructura t_data_env_exe.
 */
void	free_data_env_exe(t_data_env_exe *data)
{
	if (!data)
		return ;
	if (data->cmds_head)
	{
		free_cmd_list(data->cmds_head);
		data->cmds_head = NULL;
	}
	if (data->env_for_execve)
	{
		free_arr2d(data->env_for_execve);
		data->env_for_execve = NULL;
	}
	free_pwd_oldpwd_in_data(data);
}
