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

#include "minishell.h"         // Para t_cmd, t_env, EXIT_SUCCESS, EXIT_FAILURE, is_builtins, etc.
#include "minishell_executor.h" // Para t_cmd_exe, t_data_env_exe, t_arg_info_exe, t_cmd_io_exe,
                               // y prototipos como free_flperez_arg_info_array, free_flperez_cmd_list.

// Prototipos de funciones estáticas
static t_arg_info_exe   **translate_bonus_args_to_flp_args(char **b_commands);
static int              populate_flp_io_from_bonus_redirs(t_cmd_io_exe *flp_io,
                                            t_redir *b_redir_list);
static t_cmd_exe        *translate_one_b_cmd_to_flp_cmd(t_cmd *b_cmd_node);

/*
* Static helper: Traduce los argumentos de un t_cmd (bonus)
* a un array de t_arg_info_exe ** (para flperez).
*/
static t_arg_info_exe   **translate_bonus_args_to_flp_args(char **b_commands)
{
    t_arg_info_exe  **flp_args;
    int             arg_count;
    int             i;

    arg_count = 0;
    if (b_commands)
        while (b_commands[arg_count])
            arg_count++;
    flp_args = (t_arg_info_exe **)p_malloc(sizeof(t_arg_info_exe *)
            * (arg_count + 1));
    if (!flp_args)
        return (NULL);
    i = 0;
    while (i < arg_count)
    {
        flp_args[i] = (t_arg_info_exe *)p_malloc(sizeof(t_arg_info_exe));
        if (!flp_args[i])
        {
            free_flperez_arg_info_array(flp_args);
            return (NULL);
        }
        flp_args[i]->value = ft_strdup(b_commands[i]);
        if (!flp_args[i]->value && b_commands[i])
        {
            free(flp_args[i]); 
            flp_args[i] = NULL; 
            free_flperez_arg_info_array(flp_args); 
            return (NULL);
        }
        flp_args[i]->q_status = NO_QUOTE_EXEC; // ASUNCIÓN: El parser/expander de bonus ya manejó comillas
                                            // para el contenido de b_commands. flperez_core recibe args "limpios".
        i++;
    }
    flp_args[i] = NULL; 
    return (flp_args);
}

/*
* Static helper: Rellena la estructura t_cmd_io_exe a partir de
* la lista de t_redir (bonus).
* Asume que flp_io ya está alocado e inicializado.
*/
static int  populate_flp_io_from_bonus_redirs(t_cmd_io_exe *flp_io,
                                            t_redir *b_redir_list)
{
    t_redir *b_iter;

    b_iter = b_redir_list;
    while (b_iter)
    {
        if (!b_iter->fname)
        {
            b_iter = b_iter->next;
            continue ;
        }
        if (b_iter->redir_type == REDIR_INPUT)
        {
            if (flp_io->infile) free(flp_io->infile);
            flp_io->infile = ft_strdup(b_iter->fname);
            if (!flp_io->infile && b_iter->fname) return (EXIT_FAILURE);
            if (flp_io->heredoc_delimiter) { free(flp_io->heredoc_delimiter); flp_io->heredoc_delimiter = NULL; }
            flp_io->heredoc_quotes = false;
        }
        else if (b_iter->redir_type == HEREDOC_INPUT)
        {
            if (flp_io->heredoc_delimiter) free(flp_io->heredoc_delimiter);
            flp_io->heredoc_delimiter = ft_strdup(b_iter->fname); // fname ES el delimitador
            if (!flp_io->heredoc_delimiter && b_iter->fname) return (EXIT_FAILURE);
            // Asume que t_redir (bonus) tiene 'original_delimiter_had_quotes'
            // y que redir_new_node (bonus) lo rellena correctamente.
            flp_io->heredoc_quotes = b_iter->original_delimiter_had_quotes; 
            if (flp_io->infile) { free(flp_io->infile); flp_io->infile = NULL; }
        }
        else if (b_iter->redir_type == REDIR_OUTPUT)
        {
            if (flp_io->outfile) free(flp_io->outfile);
            flp_io->outfile = ft_strdup(b_iter->fname);
            if (!flp_io->outfile && b_iter->fname) return (EXIT_FAILURE);
            flp_io->append_mode = false;
        }
        else if (b_iter->redir_type == REDIR_OUTPUT_APPEND)
        {
            if (flp_io->outfile) free(flp_io->outfile);
            flp_io->outfile = ft_strdup(b_iter->fname);
            if (!flp_io->outfile && b_iter->fname) return (EXIT_FAILURE);
            flp_io->append_mode = true;
        }
        b_iter = b_iter->next;
    }
    return (EXIT_SUCCESS); 
}

/*
* Static helper: Traduce un único nodo t_cmd (bonus) a t_cmd_exe (flperez).
*/
static t_cmd_exe    *translate_one_b_cmd_to_flp_cmd(t_cmd *b_cmd_node)
{
    t_cmd_exe       *flp_cmd;

    if (!b_cmd_node)
        return (NULL);
    flp_cmd = (t_cmd_exe *)p_malloc(sizeof(t_cmd_exe));
    if (!flp_cmd)
        return (NULL);
    ft_memset(flp_cmd, 0, sizeof(t_cmd_exe));
    // No se asigna flp_cmd->cmd si se decide no usarlo.

    flp_cmd->io = (t_cmd_io_exe *)p_malloc(sizeof(t_cmd_io_exe));
    if (!flp_cmd->io)
        return (free(flp_cmd), NULL);
    ft_memset(flp_cmd->io, 0, sizeof(t_cmd_io_exe));
    flp_cmd->io->fd_in = -1; 
    flp_cmd->io->fd_out = -1;

    flp_cmd->args = translate_bonus_args_to_flp_args(b_cmd_node->commands);
    if (!flp_cmd->args && b_cmd_node->commands && b_cmd_node->commands[0])
    {
        free(flp_cmd->io);
        free(flp_cmd);
        return (NULL);
    }

    if (populate_flp_io_from_bonus_redirs(flp_cmd->io, b_cmd_node->redir) != EXIT_SUCCESS)
    {
        if (flp_cmd->args)
             free_flperez_arg_info_array(flp_cmd->args);
        free(flp_cmd->io);
        free(flp_cmd);
        return (NULL);
    }

    // Usa is_builtins de "bonus" para determinar si el comando original parecía un builtin.
    // El prototipo de is_builtins(t_cmd *cmd) debe estar en minishell.h.
    if (b_cmd_node->commands && b_cmd_node->commands[0])
        flp_cmd->is_builtin = is_builtins(b_cmd_node); 
    else
        flp_cmd->is_builtin = false;
    
    flp_cmd->pid = -1; // Inicializar PID
    return (flp_cmd);
}

/*
* Función principal de la capa de traducción para la lista de comandos.
*/
t_cmd_exe   *convert_cmd_list_to_cms_list_exec(
    struct s_cmd *b_cmd_list_head)
{
    t_cmd_exe   *flp_cmds_head;
    t_cmd_exe   *flp_cmds_tail;
    t_cmd_exe   *new_flp_cmd_node;
    t_cmd       *b_cmd_iter;

    if (!b_cmd_list_head)
        return (NULL);
    flp_cmds_head = NULL;
    flp_cmds_tail = NULL;
    b_cmd_iter = b_cmd_list_head;
    while (b_cmd_iter)
    {
        new_flp_cmd_node = translate_one_b_cmd_to_flp_cmd(b_cmd_iter);
        if (!new_flp_cmd_node)
        {
            free_flperez_cmd_list(flp_cmds_head);
            return (NULL);
        }
        if (!flp_cmds_head)
        {
            flp_cmds_head = new_flp_cmd_node;
            flp_cmds_tail = new_flp_cmd_node;
        }
        else
        {
            flp_cmds_tail->next = new_flp_cmd_node;
            new_flp_cmd_node->prev = flp_cmds_tail;
            flp_cmds_tail = new_flp_cmd_node;
        }
        b_cmd_iter = b_cmd_iter->next;
    }
    return (flp_cmds_head);
}

/*
* Crea una cadena "KEY=VALUE" a partir de un nodo t_env.
* Devuelve la cadena alocada, o NULL si falla malloc o si el nodo es inválido.
*/
static char	*assemble_env_string(t_env *b_env_node)
{
	char	*tmp_key_eq;
	char	*env_str;

	// Esta comprobación es por si acaso, aunque el bucle principal ya filtra.
	if (!b_env_node || !b_env_node->key || !b_env_node->val)
		return (NULL);
	tmp_key_eq = ft_strjoin(b_env_node->key, "=");
	if (!tmp_key_eq)
		return (NULL);
	env_str = ft_strjoin(tmp_key_eq, b_env_node->val);
	free(tmp_key_eq); // Liberar tmp_key_eq en cualquier caso después de su uso.
	if (!env_str)
		return (NULL); // Fallo en el segundo ft_strjoin
	return (env_str);
}

/*
* Convierte la lista enlazada t_env (de bonus) a un array de strings (char **)
* terminado en NULL, apto para execve.
* Devuelve NULL si falla alguna asignación de memoria.
*/
char	**convert_envlist_to_envp_exe(struct s_env *b_env_list)
{
	t_env	*b_env_iter;
	size_t	count;
	char	**envp_array;
	size_t	i;

	count = 0;
	b_env_iter = b_env_list;
	while (b_env_iter) // Contar elementos con valor
	{
		if (b_env_iter->val)
			count++;
		b_env_iter = b_env_iter->next;
	}
	envp_array = (char **)p_malloc(sizeof(char *) * (count + 1));
	if (!envp_array)
		return (NULL);
	b_env_iter = b_env_list;
	i = 0;
	while (i < count && b_env_iter) // Llenar el array
	{
		if (b_env_iter->key && b_env_iter->val) // Procesar solo si key y val existen
		{
			envp_array[i] = assemble_env_string(b_env_iter);
			if (!envp_array[i])
				return (free_arr2d(envp_array), NULL); // Fallo en assemble_env_string
			i++;
		}
		b_env_iter = b_env_iter->next;
	}
	envp_array[i] = NULL; // Terminador NULL
	return (envp_array);
}
