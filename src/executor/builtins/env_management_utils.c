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
