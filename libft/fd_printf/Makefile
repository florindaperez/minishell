# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: asans-go <asans-go@student.42barcel>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/27 15:54:14 by asans-go          #+#    #+#              #
#    Updated: 2024/03/27 19:19:24 by asans-go         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libftdprintf.a

SOURCES = \
	ft_dprintf.c ft_dputchar.c ft_dputstr.c ft_dputnbr.c ft_dputunsnbr.c \
	ft_dputhxanbr.c ft_dputhxanbr_up.c ft_dputhxaptr.c

HEADER = ../../inc/ft_dprintf.h
INCLUDE = -I

OBJECTS = $(SOURCES:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -Werror
AR = ar -rcs

all: $(NAME)

$(NAME): $(OBJECTS) $(HEADER) Makefile
	$(AR) $(NAME) $(OBJECTS) 

%.o: %.c $(HEADER) Makefile
	$(CC) $(CFLAGS) $(INCLUDE)/$(HEADER) -c $<

clean: Makefile
	rm -f $(OBJECTS)

fclean: clean Makefile
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
