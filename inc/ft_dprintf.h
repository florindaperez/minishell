/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dprintf.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <asans-go@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:06:00 by asans-go          #+#    #+#             */
/*   Updated: 2024/03/27 18:53:08 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_DPRINTF_H
# define FT_DPRINTF_H

# include "../inc/libft.h"
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>

int		ft_dprintf(int fd, const char *str, ...);
int		ft_dputchar(int fd, int c);
int		ft_dputstr(int fd, const char *str);
int		ft_dputnbr(int fd, int nb);
int		ft_dputunsnbr(int fd, unsigned int nb);
int		ft_dputhxanbr(int fd, unsigned int n);
int		ft_dputhxanbr_up(int fd, unsigned int n);
int		ft_dputhxaptr(int fd, unsigned long n);

#endif
