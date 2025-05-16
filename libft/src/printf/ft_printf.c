/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <asans-go@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:28:46 by asans-go          #+#    #+#             */
/*   Updated: 2024/03/27 19:21:05 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/libft.h"
#include <stdarg.h>

int	print_special(char spec, va_list argptr)
{
	int	aux;

	aux = 0;
	if (spec == 'c')
		aux += ft_putchar(va_arg(argptr, int));
	else if (spec == 's')
		aux += ft_putstr(va_arg(argptr, const char *));
	else if (spec == 'd' || spec == 'i')
		aux += ft_putnbr(va_arg(argptr, int));
	else if (spec == 'u')
		aux += ft_putunsnbr(va_arg(argptr, unsigned int));
	else if (spec == 'x')
		aux += ft_puthxanbr(va_arg(argptr, unsigned int));
	else if (spec == 'X')
		aux += ft_puthxanbr_up(va_arg(argptr, unsigned int));
	else if (spec == 'p')
	{
		if (write(1, "0x", 2) == -1)
			return (-1);
		aux = ft_puthxaptr(va_arg(argptr, unsigned long));
		aux += 2;
	}
	else
		return (write(1, &spec, 1));
	return (aux);
}

int	ft_printf(const char *str, ...)
{
	int		aux;
	int		i;
	va_list	argptr;

	va_start(argptr, str);
	i = 0;
	aux = 0;
	while (str[i] != '\0')
	{
		if (str[i] == '%' && str[i + 1] != '\\')
		{
			i++;
			aux += print_special(str[i], argptr);
		}
		else
		{
			aux += write(1, &str[i], 1);
		}
		if (aux < 0)
			return (-1);
		i++;
	}
	va_end(argptr);
	return (aux);
}
/*
int	main(void)
{
	int		rslt;

	rslt = ft_printf("%p", (void *)14523);
	printf ("\n%d", rslt);
	return (0);
}*/
