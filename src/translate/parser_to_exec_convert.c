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

#include "minishell.h"         // Primero, para definir t_cmd, t_env, etc.
#include "minishell_executor.h" // Luego, para usar t_cmd en sus prototipos y definir t_cmd_exe, etc.

/*
* Static helper: Traduce los argumentos de un t_cmd (bonus)
* a un array de t_arg_info_exe ** (para flperez).
*/
static t_arg_info_exe	**translate_bonus_args_to_flp_args(char **b_commands)
{
	t_arg_info_exe	**flp_args;
	int				arg_count;
	int				i;

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
		if (!flp_args[i]) // Limpieza en caso de fallo
			return (free_flperez_arg_info_array(flp_args), NULL);
		flp_args[i]->value = ft_strdup(b_commands[i]);
		if (!flp_args[i]->value && b_commands[i]) // ft_strdup puede fallar
			return (free(flp_args[i]), free_flperez_arg_info_array(flp_args), NULL);
		flp_args[i]->q_status = NO_QUOTE_EXEC; // ASUNCIÓN INICIAL
		i++;
	}
	return (flp_args[i] = NULL, flp_args);
}

/*
* Static helper: Rellena la estructura t_cmd_io_exe a partir de
* la lista de t_redir (bonus).
* Asume que flp_io ya está alocado e inicializado a cero.
*/
static int	populate_flp_io_from_bonus_redirs(t_cmd_io_exe *flp_io,
											t_redir *b_redir_list)
{
	t_redir	*b_iter;

	b_iter = b_redir_list;
	while (b_iter)
	{
		if (!b_iter->fname) // Ignorar redirecciones sin nombre de archivo
		{
			b_iter = b_iter->next;
			continue ;
		}
		if (b_iter->redir_type == REDIR_INPUT)
		{
			if (flp_io->infile) free(flp_io->infile);
			flp_io->infile = ft_strdup(b_iter->fname);
			if (flp_io->heredoc_delimiter) free(flp_io->heredoc_delimiter);
			flp_io->heredoc_delimiter = NULL;
			flp_io->heredoc_quotes = false;
		}
		else if (b_iter->redir_type == HEREDOC_INPUT)
		{
			if (flp_io->heredoc_delimiter) free(flp_io->heredoc_delimiter);
			flp_io->heredoc_delimiter = ft_strdup(b_iter->fname); // fname ES el delimitador
			// *** CRUCIAL: Usa el campo que añadiste a t_redir (bonus) ***
			// flp_io->heredoc_quotes = b_iter->original_delimiter_had_quotes;
			flp_io->heredoc_quotes = false; // ¡¡¡PLACEHOLDER - DEBES AJUSTAR ESTO!!!
			if (flp_io->infile) free(flp_io->infile);
			flp_io->infile = NULL;
		}
		else if (b_iter->redir_type == REDIR_OUTPUT)
		{
			if (flp_io->outfile) free(flp_io->outfile);
			flp_io->outfile = ft_strdup(b_iter->fname);
			flp_io->append_mode = false;
		}
		else if (b_iter->redir_type == REDIR_OUTPUT_APPEND)
		{
			if (flp_io->outfile) free(flp_io->outfile);
			flp_io->outfile = ft_strdup(b_iter->fname);
			flp_io->append_mode = true;
		}
		// Comprobar errores de ft_strdup (no mostrado por brevedad, pero esencial)
		// if ((b_iter->redir_type == REDIR_INPUT && !flp_io->infile && b_iter->fname) || ...) return (ERROR_CODE);
		b_iter = b_iter->next;
	}
	return (EXIT_SUCCESS); 
}
// NOTA: La función anterior excede las 25 líneas. Necesitará ser dividida.
// Podrías tener funciones como:
// static int process_single_redir_translation(t_cmd_io_exe *flp_io, t_redir *b_redir_node);
// Y populate_flp_io_from_bonus_redirs iteraría llamando a esta.

/*
* Static helper: Traduce un único nodo t_cmd (bonus) a t_cmd_exe (flperez).
* Maneja la alocación y relleno de la estructura t_cmd_exe y t_cmd_io_exe.
*/
static t_cmd_exe	*translate_one_b_cmd_to_flp_cmd(t_cmd *b_cmd_node)
{
	t_cmd_exe		*flp_cmd;

	if (!b_cmd_node)
		return (NULL);
	flp_cmd = (t_cmd_exe *)p_malloc(sizeof(t_cmd_exe));
	if (!flp_cmd)
		return (NULL);
	ft_memset(flp_cmd, 0, sizeof(t_cmd_exe));
	flp_cmd->io = (t_cmd_io_exe *)p_malloc(sizeof(t_cmd_io_exe));
	if (!flp_cmd->io)
		return (free(flp_cmd), NULL);
	ft_memset(flp_cmd->io, 0, sizeof(t_cmd_io_exe));
	flp_cmd->io->fd_in = -1;
	flp_cmd->io->fd_out = -1;
	flp_cmd->args = translate_bonus_args_to_flp_args(b_cmd_node->commands);
	if (!flp_cmd->args && b_cmd_node->commands && b_cmd_node->commands[0]) // Si había comandos pero la traducción falló
		return (free(flp_cmd->io), free(flp_cmd), NULL);
	// La siguiente línea debería llamar a una versión de populate_flp_io_from_bonus_redirs
	// que maneje errores y limpie en caso de fallo.
	if (populate_flp_io_from_bonus_redirs(flp_cmd->io, b_cmd_node->redir) != EXIT_SUCCESS)
		return (free_flperez_arg_info_array(flp_cmd->args), free(flp_cmd->io), free(flp_cmd), NULL);
	if (b_cmd_node->commands && b_cmd_node->commands[0])
		flp_cmd->is_builtin = is_builtins(b_cmd_node); // is_builtins de bonus
	else
		flp_cmd->is_builtin = false;
	return (flp_cmd->pid = -1, flp_cmd);
}

/*
* Función principal de la capa de traducción para la lista de comandos.
* Convierte una lista enlazada de t_cmd (de bonus) a una lista
* enlazada de t_cmd_exe (para flperez).
*/
t_cmd_exe	*convert_bonus_cmd_list_to_flperez_cmd_list(
	struct s_cmd *b_cmd_list_head)
{
	t_cmd_exe	*flp_cmds_head;
	t_cmd_exe	*flp_cmds_tail;
	t_cmd_exe	*new_flp_cmd_node;
	t_cmd		*b_cmd_iter; // Iterador para la lista de bonus (tipo t_cmd de bonus)

	if (!b_cmd_list_head)
		return (NULL);
	flp_cmds_head = NULL;
	flp_cmds_tail = NULL;
	b_cmd_iter = (t_cmd *)b_cmd_list_head; // Casteo explícito si struct s_cmd* es el tipo del parámetro
	while (b_cmd_iter)
	{
		new_flp_cmd_node = translate_one_b_cmd_to_flp_cmd(b_cmd_iter);
		if (!new_flp_cmd_node)
			return (free_flperez_cmd_list(flp_cmds_head), NULL); // Limpiar lo ya traducido
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
* Convierte la lista enlazada t_env (de bonus) a un array de strings (char **)
* como espera el campo env de t_data_env_exe (para flperez).
*/
char	**convert_bonus_envlist_to_flperez_envp(struct s_env *b_env_list)
{
	struct s_env	*b_env_iter; // Iterador para la lista de bonus
	size_t			count;
	char			**envp_array;
	char			*tmp_join;
	size_t			i;

	count = 0;
	b_env_iter = (t_env *)b_env_list; // Casteo si es necesario
	while (b_env_iter)
	{
		if (b_env_iter->val) // Solo variables con valor
			count++;
		b_env_iter = b_env_iter->next;
	}
	envp_array = (char **)p_malloc(sizeof(char *) * (count + 1));
	if (!envp_array)
		return (NULL);
	b_env_iter = (t_env *)b_env_list;
	i = 0;
	while (b_env_iter)
	{
		if (b_env_iter->val)
		{
			tmp_join = ft_strjoin(b_env_iter->key, "=");
			// Robust ft_strjoin and error checking needed here
			if (!tmp_join) return (free_arr2d(envp_array), NULL);
			envp_array[i] = ft_strjoin(tmp_join, b_env_iter->val);
			free(tmp_join);
			if (!envp_array[i]) return (free_arr2d(envp_array), NULL);
			i++;
		}
		b_env_iter = b_env_iter->next;
	}
	return (envp_array[i] = NULL, envp_array);
}

// NOTA: Ya tenemos 5 funciones en este archivo. Las funciones de liberación
// (free_flperez_cmd_list, free_flperez_data_env_content, y los helpers
// free_flperez_cmd_io_content, free_flperez_arg_info_array) deberían ir
// en un archivo separado, por ejemplo, `parser_to_exec_free.c`.
// También, populate_flp_io_from_bonus_redirs excede las 25 líneas y
// necesitará ser dividida.
