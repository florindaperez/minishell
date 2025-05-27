/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_exists.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 11:18:59 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/27 11:19:05 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* Actualiza el valor de una variable de entorno si existe (key).
* Si no existe, la crea y la añade al final de la lista.
* Devuelve el puntero a la cabeza de la lista (puede cambiar si era NULL).
*/
t_env	*update_env(t_env *env, char *key, char *val)
{
	t_env	*current;
	t_env	*new_node;

	current = env;
	while (current != NULL)
	{
		if (ft_strcmp(key, current->key) == 0)
		{
			free(current->val);
			current->val = ft_strdup(val);
			return (env);
		}
		current = current->next;
	}
	new_node = lstnew(key, val);
	lstadd_back(&env, new_node);
	return (env);
}

/*
* Comprueba si la variable (solo la parte de la clave antes de '=')
* existe en la lista de entorno.
* 'variable' puede ser "KEY=value" o "KEY".
*/
int	variable_exists_op3(t_env *env, char *variable)
{
	char	**var_ent;
	int		flag;
	t_env	*current;

	flag = 0;
	current = env;
	var_ent = ft_split(variable, '=');
	if (!var_ent)
		return (0);
	while (current != NULL)
	{
		if (var_ent[0] && ft_strcmp(var_ent[0], current->key) == 0)
		{
			flag = 1;
			break ;
		}
		current = current->next;
	}
	free_arr2d(var_ent);
	return (flag);
}

/*
* Comprueba si una variable con la 'key' dada existe en la lista de entorno.
* Devuelve un puntero al nodo si se encuentra, o NULL si no.
*/
t_env	*variable_exists_op4(t_env *env, const char *key)
{
	t_env	*current;

	current = env;
	while (current != NULL)
	{
		if (ft_strcmp(key, current->key) == 0)
			return (current);
		current = current->next;
	}
	return (NULL);
}
