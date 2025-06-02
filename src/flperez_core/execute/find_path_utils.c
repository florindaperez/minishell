/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_path_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 00:12:14 by flperez-          #+#    #+#             */
/*   Updated: 2025/06/02 00:12:24 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
* Une dos componentes de ruta con un '/'.
* Maneja el caso donde s1 ya termina con '/'.
* Devuelve una nueva cadena alocada, o NULL si falla malloc.
*/
char	*ft_strjoin_path(char const *s1, char const *s2)
{
	char	*result;
	size_t	len1;
	size_t	len2;
	bool	s1_ends_with_slash;

	if (!s1 || !s2)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	s1_ends_with_slash = (len1 > 0 && s1[len1 - 1] == '/');
	result = (char *)malloc(sizeof(char) * (len1 + len2 + \
				(1 - s1_ends_with_slash) + 1));
	if (!result)
		return (NULL);
	ft_memcpy(result, s1, len1);
	if (!s1_ends_with_slash)
	{
		result[len1] = '/';
		len1++;
	}
	ft_memcpy(result + len1, s2, len2);
	result[len1 + len2] = '\0';
	return (result);
}
