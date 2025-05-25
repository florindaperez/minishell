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

/*
* Función que verifica si un nombre de variable es válido para el entorno.
* - El primer carácter debe ser alfabético o '_'.
* - Los caracteres siguientes pueden ser alfanuméricos o '_'.
* - No debe contener '='.
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
* Redimensiona un array de entorno (char **).
* Crea un nuevo array, copia las cadenas antiguas (duplicándolas),
* y luego libera el array antiguo y sus cadenas originales.
*/
bool	realloc_env_array(char ***old_env_array_ptr, int new_total_count)
{
	char	**new_env_array;
	int		i;
	int		old_current_count;

	old_current_count = 0;
	if (*old_env_array_ptr)
		while ((*old_env_array_ptr)[old_current_count])
			old_current_count++;
	new_env_array = (char **)ft_calloc(new_total_count + 1, sizeof(char *));
	if (!new_env_array)
		return (false);
	i = 0;
	while (i < old_current_count && i < new_total_count)
	{
		new_env_array[i] = ft_strdup((*old_env_array_ptr)[i]);
		if (!new_env_array[i])
		{
			free_str_tab(new_env_array);
			return (false);
		}
		i++;
	}
	free_str_tab(*old_env_array_ptr);
	*old_env_array_ptr = new_env_array;
	return (true);
}

/*
* Agrega o modifica una variable de entorno en data->env.
*/
bool	add_env_var(t_data_env *data, char *key, char *value)
{
	int		id;
	char	*new_entry_str;
	int		current_env_size;

	id = get_env_var_id(data->env, key);
	new_entry_str = build_env_entry(key, value);
	if (!new_entry_str)
		return (false);
	if (id != -1 && data->env && data->env[id])
	{
		free_ptr(data->env[id]);
		data->env[id] = new_entry_str;
	}
	else
	{
		current_env_size = num_var_env(data->env);
		if (!realloc_env_array(&(data->env), current_env_size + 1))
		{
			free_ptr(new_entry_str);
			return (false);
		}
		data->env[current_env_size] = new_entry_str;
	}
	return (true);
}
