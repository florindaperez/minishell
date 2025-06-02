/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 18:20:14 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/31 18:20:17 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h" 

/*
* static bool	update_existing_env_var(t_env *curr_node, const char *val)
* --------------------------------------------------------------------------
* Auxiliar de add_or_update_env_list_var. Actualiza el valor de un nodo
* t_env existente. Libera el valor antiguo y duplica el nuevo.
* Retorna true si éxito, false si falla ft_strdup.
*/
static bool	update_existing_env_var(t_env *curr_node, const char *val)
{
	char	*val_dup;

	val_dup = NULL;
	if (val)
	{
		val_dup = ft_strdup(val);
		if (!val_dup)
			return (false);
	}
	free_ptr(curr_node->val);
	curr_node->val = val_dup;
	return (true);
}

/*
* static bool	add_new_env_var(t_env **head, const char *key, const char *val)
* ------------------------------------------------------------------------------
* Auxiliar de add_or_update_env_list_var. Crea y añade un nuevo nodo t_env
* a la lista.
* Retorna true si éxito, false si falla lstnew.
*/
static bool	add_new_env_var(t_env **head, const char *key, const char *val)
{
	t_env	*new_node;

	new_node = lstnew((char *)key, (char *)val);
	if (!new_node)
		return (false);
	lstadd_back(head, new_node);
	return (true);
}

/*
* bool	add_or_update_env_list_var(t_env **list_head_ptr, ...)
* ----------------------------------------------------------------
* Añade o actualiza una variable en la lista enlazada t_env.
* Esta función es NO ESTÁTICA para ser llamada desde builtin_export.c.
* Duplica 'key' y 'value' para almacenarlos. Si 'value' es NULL,
* la variable se guarda con un valor NULL.
* Retorna: true si éxito, false si falla alocación de memoria.
*/
bool	add_or_update_env_list_var(t_env **list_head_ptr, \
									const char *key, const char *value)
{
	t_env	*current;

	if (!list_head_ptr || !key)
		return (false);
	current = *list_head_ptr;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
			return (update_existing_env_var(current, value));
		current = current->next;
	}
	return (add_new_env_var(list_head_ptr, key, value));
}

/*
* bool	parse_export_argument_pair(const char *arg, ...)
* --------------------------------------------------------
* Parsea un argumento de export (ej. "KEY=value" o "KEY") en sus componentes
* key y value. Aloca memoria para key_out y value_out.
* El llamador es responsable de liberar key_out y value_out.
* Esta función es NO ESTÁTICA para ser llamada desde builtin_export.c.
* Retorna true si el parseo y las alocaciones son exitosas, false si no.
*/
bool	parse_export_argument_pair(const char *arg, char **key_out, \
										char **value_out)
{
	char	*equal_sign_pos;

	*key_out = NULL;
	*value_out = NULL;
	equal_sign_pos = ft_strchr(arg, '=');
	if (equal_sign_pos)
	{
		*key_out = ft_substr(arg, 0, equal_sign_pos - arg);
		if (*(equal_sign_pos + 1) == '\0')
			*value_out = ft_strdup("");
		else
			*value_out = ft_strdup(equal_sign_pos + 1);
	}
	else
		*key_out = ft_strdup(arg);
	if (!(*key_out) || (equal_sign_pos && !(*value_out)))
	{
		free_ptr(*key_out);
		*key_out = NULL;
		free_ptr(*value_out);
		*value_out = NULL;
		return (false);
	}
	return (true);
}

/*
* void	print_export_formatted_env(...)
* ----------------------------------------------
* Imprime vars en formato 'declare -x KEY="VALUE"'. Implementación SIMPLIFICADA.
*/
void	print_export_formatted_env(t_env *env_list)
{
	t_env	*curr;

	curr = env_list;
	while (curr)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(curr->key, STDOUT_FILENO);
		if (curr->val)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(curr->val, STDOUT_FILENO);
			ft_putstr_fd("\"", STDOUT_FILENO);
		}
		ft_putchar_fd('\n', STDOUT_FILENO);
		curr = curr->next;
	}
}
