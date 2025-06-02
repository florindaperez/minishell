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

#include "minishell.h"          // Para free_arr2d, p_free (si se usa), y otros includes básicos
#include "minishell_executor.h" // Para t_cmd_exe, t_arg_info_exe, t_cmd_io_exe, t_data_env_exe
#include <stdlib.h>             // Para free

// Prototipo estático si free_flperez_cmd_io solo se usa aquí
static void free_flperez_cmd_io(t_cmd_io_exe *io);

// Libera un array de estructuras t_arg_info_exe
void free_flperez_arg_info_array(t_arg_info_exe **flp_args)
{
    int i;

    if (!flp_args)
        return;
    i = 0;
    while (flp_args[i])
    {
        if (flp_args[i]->value)
            free(flp_args[i]->value);
        free(flp_args[i]);
        flp_args[i] = NULL; // Buena práctica
        i++;
    }
    free(flp_args);
}

// Libera una única estructura t_cmd_io_exe
static void free_flperez_cmd_io(t_cmd_io_exe *io)
{
    if (!io)
        return;
    if (io->infile)
        free(io->infile);
    if (io->outfile)
        free(io->outfile);
    if (io->heredoc_delimiter)
        free(io->heredoc_delimiter);
    // fd_in, fd_out, stdin_backup, stdout_backup no son punteros a memoria alocada aquí.
    free(io);
}

// Libera una lista de estructuras t_cmd_exe
void free_flperez_cmd_list(t_cmd_exe *flp_cmd_list_head)
{
    t_cmd_exe *current;
    t_cmd_exe *next_node;

    current = flp_cmd_list_head;
    while (current)
    {
        next_node = current->next;
        if (current->path) // Path puede no estar siempre asignado
            free(current->path);
        if (current->args)
            free_flperez_arg_info_array(current->args);
        if (current->io)
            free_flperez_cmd_io(current->io);
        // current->cmd es un puntero a la estructura original de "bonus",
        // que debe ser liberada por la lógica de "bonus" (cmd_free). No se libera aquí.
        free(current);
        current = next_node;
    }
}

// Libera el contenido de la estructura t_data_env_exe
// (Anteriormente llamada free_data_env_exe o free_data_env_exe  en la conversación)
// Asegúrate de que el nombre usado aquí coincida con el prototipo en minishell_executor.h
// y con las llamadas en builtin_exit.c
void free_data_env_exe (t_data_env_exe *data)
{
    if (!data)
        return;
    
    // Liberar la lista de comandos del ejecutor
    if (data->cmds_head)
    {
        free_flperez_cmd_list(data->cmds_head); // Esta es la llamada que fallaba si free_flperez_cmd_list no estaba definida
        data->cmds_head = NULL;
    }

    // Liberar el array del entorno (envp)
    if (data->env_for_execve)
    {
        free_arr2d(data->env_for_execve); // free_arr2d de tu parte "bonus"
        data->env_for_execve = NULL;
    }

    // Liberar las cadenas PWD y OLDPWD si fueron alocadas
    if (data->pwd)
    {
        free(data->pwd); // Usar free directamente
        data->pwd = NULL;
    }
    if (data->old_pwd)
    {
        free(data->old_pwd); // Usar free directamente
        data->old_pwd = NULL;
    }
    // No se libera 'data' en sí mismo aquí, ya que podría estar en la pila.
    // Esta función libera los *contenidos* de la estructura.
}