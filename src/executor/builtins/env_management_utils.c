/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_management_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 17:57:36 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/09 17:57:45 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
* bool	is_valid_env_var_key(const char *key)
* ---------------------------------------------
* Función que verifica si un nombre de variable es válido para el entorno.
* - El primer carácter debe ser alfabético o '_'.
* - Los caracteres siguientes pueden ser alfanuméricos o '_'.
*/
bool	is_valid_env_var_key(const char *key)
{
	int	i;

	if (!key || !key[0])
		return (false);
	if (!(ft_isalpha(key[0]) || key[0] == '_'))
		return (false);
	i = 1;
	while (key[i])
	{
		if (!(ft_isalnum(key[i]) || key[i] == '_'))
			return (false);
		i++;
	}
	return (true);
}

/*
* char	*build_env_entry(char *key, char *value)
* ------------------------------------------------
* Construye la cadena "KEY=VALUE" o "KEY=" si value es NULL/vacío.
* Devuelve la nueva cadena alocada o NULL si falla.
*/
char	*build_env_entry(char *key, char *value)
{
	char	*key_eq;
	char	*new_entry;

	if (!key)
		return (NULL);
	key_eq = ft_strjoin(key, "=");
	if (!key_eq)
		return (NULL);
	if (value == NULL || value[0] == '\0')
		new_entry = ft_strdup(key_eq);
	else
		new_entry = ft_strjoin(key_eq, value);
	free_ptr(key_eq);
	return (new_entry);
}

/*
* static bool	env_array_update_entry(...)
* Auxiliar de add_env_var: modifica una entrada existente en el array char**.
*/
static bool	env_array_update_entry(t_data_env_exe *data, int id, \
									char *new_entry_str)
{
	free_ptr(data->env_for_execve[id]);
	data->env_for_execve[id] = new_entry_str;
	return (true);
}

/*
* static bool	env_array_add_new_entry(...)
* Auxiliar de add_env_var: añade una nueva entrada al array char**,
* redimensionando con realloc_env_array (externa).
*/
static bool	env_array_add_new_entry(t_data_env_exe *data, \
											char *new_entry_str)
{
	int	current_env_size;

	current_env_size = num_var_env(data->env_for_execve);
	if (!realloc_env_array(&(data->env_for_execve), current_env_size + 1))
	{
		free_ptr(new_entry_str);
		return (false);
	}
	data->env_for_execve[current_env_size] = new_entry_str;
	return (true);
}

/*
* bool	add_env_var(t_data_env_exe *data, char *key, char *value)
* Agrega o modifica una variable en data->env_for_execve (char **).
* NOTA: Para export/unset, modificar data->shell_env_list_ptr (t_env*)
* y luego regenerar env_for_execve.
*/
bool	add_env_var(t_data_env_exe *data, char *key, char *value)
{
	int		id;
	char	*new_entry_str;

	if (!data)
		return (false);
	new_entry_str = build_env_entry(key, value);
	if (!new_entry_str)
		return (false);
	id = get_env_var_id(data->env_for_execve, key);
	if (id != -1)
	{
		if (!env_array_update_entry(data, id, new_entry_str))
		{
			free_ptr(new_entry_str);
			return (false);
		}
	}
	else
	{
		if (!env_array_add_new_entry(data, new_entry_str))
			return (false);
	}
	return (true);
}
