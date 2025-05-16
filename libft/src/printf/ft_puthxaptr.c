/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthxaptr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <asans-go@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:30:50 by asans-go          #+#    #+#             */
/*   Updated: 2024/03/27 19:22:20 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/libft.h"
#include <stdarg.h>

int	ft_puthxaptr(unsigned long nb)
{
	char	*pointer_val;
	int		count;

	count = 0;
	pointer_val = "0123456789abcdef";
	if (!nb)
		return (ft_putchar('0'));
	if (nb > 15)
	{
		count = ft_puthxaptr(nb / 16);
		if (count == -1)
			return (-1);
	}
	if (ft_putchar(pointer_val[nb % 16]) == -1)
		return (-1);
	count++;
	return (count);
}
