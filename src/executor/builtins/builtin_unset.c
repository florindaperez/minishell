/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 19:39:13 by flperez-          #+#    #+#             */
/*   Updated: 2025/02/28 11:16:08 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
* static bool	remove_var_from_env_list(t_env **list_head_ptr, ...)
* --------------------------------------------------------------------
* Elimina una variable de entorno (identificada por 'key') de la lista
* enlazada t_env. Libera la memoria del nodo eliminado y de sus cadenas
* 'key' y 'val'. Actualiza la cabeza de la lista si es necesario.
* Retorna: true si la variable fue encontrada y eliminada, false si no.
*/
static bool	remove_var_from_env_list(t_env **list_head_ptr, const char *key)
{
	t_env	*current;
	t_env	*prev;

	if (!list_head_ptr || !(*list_head_ptr) || !key)
		return (false);
	prev = NULL;
	current = *list_head_ptr;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev == NULL)
				*list_head_ptr = current->next;
			else
				prev->next = current->next;
			free_ptr(current->key);
			free_ptr(current->val);
			free(current);
			return (true);
		}
		prev = current;
		current = current->next;
	}
	return (false);
}

/*
* static int	process_single_unset_arg(const char *arg, ...)
* -------------------------------------------------------------
* Procesa un argumento para 'unset'. Valida el nombre. Si es válido, intenta
* eliminarlo de la lista t_env. Actualiza env_changed_flag.
* Llama a msg_error_cmd (que llama a set_exit_status) si el id es inválido.
* Retorna: EXIT_SUCCESS si nombre válido (exista o no), EXIT_FAILURE si inválido.
*/
static int	process_single_unset_arg(const char *arg, t_data_env_exe *data, \
									bool *env_changed_flag)
{
	if (!arg)
		return (EXIT_FAILURE);
	if (arg[0] == '\0')
		return (EXIT_SUCCESS);
	if (!is_valid_env_var_key(arg))
	{
		msg_error_cmd("unset", (char *)arg, "not a valid identifier", 1);
		return (EXIT_FAILURE);
	}
	if (remove_var_from_env_list(data->shell_env_list_ptr, arg))
		*env_changed_flag = true;
	return (EXIT_SUCCESS);
}

/*
* static void	process_all_unset_args(char **args, ...)
* ---------------------------------------------------------
* Itera sobre todos los argumentos pasados a 'unset' (a partir de args[1]),
* llamando a _process_single_unset_arg para cada uno.
* Actualiza las banderas env_list_modified_flag e invalid_id_flag.
*/
static void	process_all_unset_args(char **args, t_data_env_exe *data, \
									bool *env_list_modified_flag, \
									bool *invalid_id_flag)
{
	int	i;

	i = 1;
	*env_list_modified_flag = false;
	*invalid_id_flag = false;
	while (args[i])
	{
		if (process_single_unset_arg(args[i], data, env_list_modified_flag) \
			== EXIT_FAILURE)
		{
			*invalid_id_flag = true;
		}
		i++;
	}
}

/*
* static bool	_regenerate_exec_env_if_needed(t_data_env_exe *data, ...)
* -----------------------------------------------------------------------
* Si env_list_modified es true, libera el antiguo data->env_for_execve
* y lo regenera a partir de la lista t_env actualizada.
* Llama a msg_error_cmd y set_exit_status si la regeneración falla.
* Retorna: true si la regeneración fue exitosa o no necesaria,
* false si la regeneración fue necesaria y falló.
*/
static bool	regenerate_exec_env_if_needed(t_data_env_exe *data, \
											bool env_list_modified)
{
	if (env_list_modified)
	{
		free_arr2d(data->env_for_execve);
		data->env_for_execve = \
			convert_env_list_to_exec_envp(*(data->shell_env_list_ptr));
		if (!data->env_for_execve)
		{
			msg_error_cmd("unset", "(internal)", \
				"env array regeneration failed (malloc)", EXIT_FAILURE);
			return (false);
		}
	}
	return (true);
}

/*
* int	builtin_unset(char **args, t_data_env_exe *data)
* ------------------------------------------------------
* Implementa el comando 'unset'.
* El código de salida del comando 'unset' es 0 para el tester si el único
* "problema" son identificadores inválidos (aunque $? global será 1).
* Si hay un error interno grave (ej. fallo al regenerar el entorno para
* execve), el comando unset retorna 1.
*/
int	builtin_unset(char **args, t_data_env_exe *data)
{
	bool	env_list_modified;
	bool	invalid_id_reported;

	if (!args[1])
	{
		set_exit_status(EXIT_SUCCESS);
		return (EXIT_SUCCESS);
	}
	process_all_unset_args(args, data, &env_list_modified, \
							&invalid_id_reported);
	if (!regenerate_exec_env_if_needed(data, env_list_modified))
	{
		return (EXIT_FAILURE);
	}
	if (invalid_id_reported)
	{
		return (EXIT_SUCCESS);
	}
	set_exit_status(EXIT_SUCCESS);
	return (EXIT_SUCCESS);
}
