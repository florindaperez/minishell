/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 20:39:40 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/31 20:39:46 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h" 

/*
* static bool	copy_strings_to_new_array(...)
* Auxiliar de realloc_env_array. Copia y duplica cadenas.
* (Nombre sin _ al inicio según tu preferencia).
*/
static bool	copy_strings_to_new_array(char **new_arr, char **old_arr, \
									int old_cnt, int new_cap)
{
	int	i;

	i = 0;
	while (i < old_cnt && i < new_cap)
	{
		if (old_arr[i])
		{
			new_arr[i] = ft_strdup(old_arr[i]);
			if (!new_arr[i])
			{
				free_str_tab(new_arr);
				return (false);
			}
		}
		else
			new_arr[i] = NULL;
		i++;
	}
	return (true);
}

/*
* bool	realloc_env_array(char ***old_array_ptr, int new_element_capacity)
* Redimensiona un array de strings (char **), duplicando las cadenas.
* new_element_capacity es el número de strings que el nuevo array podrá tener,
* sin contar el NULL terminador.
* Esta función es NO ESTÁTICA y su prototipo debe estar en una cabecera.
*/
bool	realloc_env_array(char ***old_array_ptr, int new_element_capacity)
{
	char	**new_array;
	int		old_element_count;

	if (!old_array_ptr)
		return (false);
	old_element_count = 0;
	if (*old_array_ptr != NULL)
	{
		old_element_count = size_arr2d(*old_array_ptr);
	}
	new_array = (char **)ft_calloc(new_element_capacity + 1, sizeof(char *));
	if (!new_array)
		return (false);
	if (*old_array_ptr != NULL)
	{
		if (!copy_strings_to_new_array(new_array, *old_array_ptr, \
									old_element_count, new_element_capacity))
			return (false);
	}
	free_str_tab(*old_array_ptr);
	*old_array_ptr = new_array;
	return (true);
}
