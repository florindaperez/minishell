/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strndup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 12:17:59 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/30 12:19:55 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
* Duplica los primeros n caracteres de src.
* Necesaria proyecto minishell 20250530
*/
char	*ft_strndup(const char *src, size_t n)
{
	size_t	i;
	char	*dest;
	size_t	src_len;

	if (!src)
		return (NULL);
	src_len = 0;
	while (src[src_len] && src_len < n)
		src_len++;
	dest = (char *)malloc(sizeof(char) * (src_len + 1));
	if (!dest)
		return (NULL);
	i = 0;
	while (i < src_len)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}
