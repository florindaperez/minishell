/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dputstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <asans-go@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:11:10 by asans-go          #+#    #+#             */
/*   Updated: 2024/03/27 19:16:28 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/ft_dprintf.h"

int	ft_dputstr(int fd, const char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (ft_dputstr(fd, "(null)"));
	while (str[i])
	{
		if (write(fd, &str[i], 1) < 0)
			return (-1);
		i++;
	}
	return (i);
}
