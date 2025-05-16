/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putunsnbr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <asans-go@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:32:14 by asans-go          #+#    #+#             */
/*   Updated: 2024/03/27 19:23:18 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/libft.h"
#include <stdarg.h>

int	ft_putunsnbr(unsigned int nb)
{
	int	count;

	count = 0;
	if (nb > 9)
	{
		count += ft_putunsnbr(nb / 10);
		if (count == -1)
			return (-1);
	}
	if (ft_putchar((nb % 10) + '0') == -1)
		return (-1);
	count++;
	return (count);
}
