/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:03:29 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/13 17:03:32 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h"

/*
* Crea y rellena una nueva estructura t_arg_info.
* En caso de error de malloc, libera la memoria y los comandos.
*/
static t_arg_info	*create_new_arg_info(char *val_str, t_quote_type qs,
										t_cmd *cmd_list)
{
	t_arg_info	*new_arg_s;

	new_arg_s = (t_arg_info *)malloc(sizeof(t_arg_info));
	if (!new_arg_s)
	{
		msg_error_cmd("parser", "malloc for t_arg_info",
			strerror(errno), 1);
		free_ptr(val_str);
		g_exit_status = 1;
		free_commands(cmd_list);
		return (NULL);
	}
	new_arg_s->value = val_str;
	new_arg_s->q_status = qs;
	return (new_arg_s);
}

/*
* Comprueba si un argumento dado es un comando builtin.
*/
static bool	is_arg_a_builtin(const char *arg_val)
{
	if (!arg_val)
		return (false);
	if (ft_strcmp(arg_val, "echo") == 0)
		return (true);
	if (ft_strcmp(arg_val, "cd") == 0)
		return (true);
	if (ft_strcmp(arg_val, "pwd") == 0)
		return (true);
	if (ft_strcmp(arg_val, "export") == 0)
		return (true);
	if (ft_strcmp(arg_val, "unset") == 0)
		return (true);
	if (ft_strcmp(arg_val, "env") == 0)
		return (true);
	if (ft_strcmp(arg_val, "exit") == 0)
		return (true);
	return (false);
}

/*
* Actualiza metadatos del comando basados en el primer argumento.
* Esto incluye si fue un comando vacío literal y si es un builtin o tiene una 
* ruta.
*/
static int	update_command_details(
	t_cmd *cmd,
	t_arg_info *arg1,
	t_token *tk1,
	t_token *tk_last)
{
	char	*arg_v;
	bool	is_empty_lit;

	arg_v = arg1->value;
	is_empty_lit = (tk1->args && tk1->args[0] == '\0'
			&& tk_last == tk1 && arg_v[0] == '\0');
	if (is_empty_lit)
		cmd->was_literal_empty_command = true;
	if (!arg_v || arg_v[0] == '\0')
		return (1);
	if (is_arg_a_builtin(arg_v))
		cmd->is_builtin = true;
	else if (!cmd->path && ft_strchr(arg_v, '/'))
	{
		cmd->path = ft_strdup(arg_v);
		if (!cmd->path)
		{
			msg_error_cmd("parser", "strdup failed for path",
				strerror(errno), 1);
			g_exit_status = 1;
			return (0);
		}
	}
	return (1);
}

/*
* Ensambla y crea una estructura t_arg_info para un argumento.
* Actualiza last_tk_p al último token consumido para el argumento.
* tk1: Primer token del argumento.
* last_tk_p: Puntero al token que se actualizará al último consumido.
* d_env: Puntero a la estructura de datos del entorno.
* cmd_list: Puntero al primer comando para liberar en error de malloc.
* Retorno:
* Puntero al t_arg_info creado, o NULL en error crítico.
*/
static t_arg_info	*build_arg_struct_for_cmd(
	t_token *tk1,
	t_token **last_tk_p,
	t_data_env *d_env,
	t_cmd *cmd_list)
{
	char			*asm_val;
	t_quote_type	qs;
	t_arg_info		*new_arg_s;

	qs = tk1->quote_status;
	*last_tk_p = tk1;
	asm_val = NULL;
	new_arg_s = NULL;
	asm_val = assemble_arg_segments(last_tk_p, d_env, cmd_list);
	if (!asm_val)
		return (NULL);
	new_arg_s = create_new_arg_info(asm_val, qs, cmd_list);
	if (!new_arg_s)
		return (NULL);
	return (new_arg_s);
}

/*
* Procesa un token de tipo TOKEN_WORD. Ensambla el argumento, crea una
* estructura t_arg_info, la añade al comando actual y actualiza metadatos
* del comando si es el primer argumento.
* cmd: Comando actual.
* tk_iter_p: Puntero al iterador de tokens. Se actualiza.
* d_env: Puntero a la estructura de datos del entorno.
* cmd_list: Primer comando de la lista (para liberar en error).
* Retorno:
* 0 en error crítico (comandos ya liberados o se liberan aquí), 1 en éxito.
*/
int	process_word_token(
	t_cmd *cmd,
	t_token **tk_iter_p,
	t_data_env *d_env,
	t_cmd *cmd_list)
{
	t_token			*tk1_arg;
	t_token			*tk_last_arg;
	t_arg_info		*arg_info;

	tk1_arg = *tk_iter_p;
	tk_last_arg = NULL;
	arg_info = NULL;
	arg_info = build_arg_struct_for_cmd(tk1_arg, &tk_last_arg,
			d_env, cmd_list);
	if (!arg_info)
		return (0);
	add_arg_info_to_cmd(cmd, arg_info);
	if (g_exit_status != 0 && cmd->args == NULL)
	{
		free_commands(cmd_list);
		return (0);
	}
	if (cmd->args && cmd->args[0] == arg_info
		&& !update_command_details(cmd, arg_info, tk1_arg, tk_last_arg))
	{
		free_commands(cmd_list);
		return (0);
	}
	*tk_iter_p = tk_last_arg->next;
	return (1);
}
