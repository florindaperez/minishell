/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:08:37 by flperez-          #+#    #+#             */
/*   Updated: 2025/04/15 14:18:22 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* Comprueba si un carácter es numérico.
*/
int	ft_is_numeric(const char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (0);
	if (str[0] == '-' || str[0] == '+')
		i++;
	if (str[i] == '\0')
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

/*
* Duplica los primeros n caracteres de src.
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

/*
* ft_isspace: Comprueba si el carácter es un espacio en blanco.
* Espacio (' '), tab ('\t'), newline ('\n'), vertical tab ('\v'),
* form feed ('\f'), carriage return ('\r').
*/
int	ft_isspace(int c)
{
	if (c == ' ' || (c >= '\t' && c <= '\r'))
		return (1);
	return (0);
}

/*
 * Cierra de forma segura un descriptor de archivo y lo marca como -1.
 * Solo intenta cerrar si el puntero es válido y el fd no es -1.
 *
 * fd => Puntero al descriptor de archivo a cerrar.
 */
void	safe_close(int *fd)
{
	if (fd != NULL && *fd != -1)
	{
		close(*fd);
		*fd = -1;
	}
}
