/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dputhxanbr_up.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <asans-go@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:08:49 by asans-go          #+#    #+#             */
/*   Updated: 2024/03/27 19:17:49 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/ft_dprintf.h"

int	ft_dputhxanbr_up(int fd, unsigned int nb)
{
	char	*hex_val;
	int		count;

	count = 0;
	hex_val = "0123456789ABCDEF";
	if (nb > 15)
	{
		count = ft_dputhxanbr_up(fd, nb / 16);
		if (count == -1)
			return (-1);
	}
	if (ft_dputchar(fd, hex_val[nb % 16]) == -1)
		return (-1);
	count++;
	return (count);
}
