# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: asans-go <asans-go@student.42barcel>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/24 19:11:38 by asans-go          #+#    #+#              #
#    Updated: 2024/07/07 15:52:42 by asans-go         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Standard
NAME			= libft.a

# Directories
INC_DIR			= inc/
SRC_DIR			= src/
OBJ_DIR			= obj/

# Compiler and Flags
CC				= gcc
CFLAGS			= -Wall -Wextra -Werror -I$(INC_DIR)
RM				= rm -f
AR				= ar rcs

# VPATH: Tell make where to find source files
VPATH = $(SRC_DIR):$(SRC_DIR)ft_is:$(SRC_DIR)ft_mem:$(SRC_DIR)ft_put:$(SRC_DIR)ft_to:$(SRC_DIR)ft_str:$(SRC_DIR)ft_lst:$(SRC_DIR)get_next_line:$(SRC_DIR)printf:$(SRC_DIR)fd_printf

# Source Files (.c) - Base names only, VPATH will find them
FTIS_SRC		=	ft_isalnum.c \
					ft_isalpha.c \
					ft_isascii.c \
					ft_isdigit.c \
					ft_isprint.c

FTMEM_SRC		=	ft_bzero.c \
					ft_calloc.c \
					ft_memchr.c \
					ft_memcmp.c \
					ft_memmove.c \
					ft_memcpy.c \
					ft_memset.c

FTPUT_SRC		=	ft_putchar_fd.c \
					ft_putendl_fd.c \
					ft_putnbr_fd.c \
					ft_putstr_fd.c

FTTO_SRC		=	ft_atoi.c \
					ft_itoa.c \
					ft_tolower.c \
					ft_toupper.c

FTSTR_SRC		=	ft_split.c \
					ft_strchr.c \
					ft_strdup.c \
					ft_striteri.c \
					ft_strjoin.c \
					ft_strlcat.c \
					ft_strlcpy.c \
					ft_strlen.c \
					ft_strmapi.c \
					ft_strncmp.c \
					ft_strncpy.c \
					ft_strnstr.c \
					ft_strrchr.c \
					ft_strtrim.c \
					ft_substr.c \
					ft_strcmp.c

FTLST_SRC		=	ft_lstadd_back.c \
					ft_lstadd_front.c \
					ft_lstclear.c \
					ft_lstdelone.c \
					ft_lstiter.c \
					ft_lstlast.c \
					ft_lstmap.c \
					ft_lstnew.c \
					ft_lstsize.c

#GNL_SRC			=	get_next_line.c

PRINTF_SRC		=	ft_printf.c \
					ft_putchar.c \
					ft_puthxanbr.c \
					ft_puthxanbr_up.c \
					ft_puthxaptr.c \
					ft_putnbr.c \
					ft_putstr.c \
					ft_putunsnbr.c

FDPRINTF_SRC	=	ft_dprintf.c \
					ft_dputchar.c \
					ft_dputhxanbr_up.c \
					ft_dputhxanbr.c \
					ft_dputhxaptr.c \
					ft_dputnbr.c \
					ft_dputstr.c \
					ft_dputunsnbr.c

# Object Files (.o) - Construct paths in OBJ_DIR preserving subdirs
FTIS_OBJ		= $(addprefix $(OBJ_DIR)ft_is/, $(FTIS_SRC:.c=.o))
FTMEM_OBJ		= $(addprefix $(OBJ_DIR)ft_mem/, $(FTMEM_SRC:.c=.o))
FTPUT_OBJ		= $(addprefix $(OBJ_DIR)ft_put/, $(FTPUT_SRC:.c=.o))
FTTO_OBJ		= $(addprefix $(OBJ_DIR)ft_to/, $(FTTO_SRC:.c=.o))
FTSTR_OBJ		= $(addprefix $(OBJ_DIR)ft_str/, $(FTSTR_SRC:.c=.o))
FTLST_OBJ		= $(addprefix $(OBJ_DIR)ft_lst/, $(FTLST_SRC:.c=.o))
#GNL_OBJ			= $(addprefix $(OBJ_DIR)get_next_line/, $(GNL_SRC:.c=.o))
PRINTF_OBJ		= $(addprefix $(OBJ_DIR)printf/, $(PRINTF_SRC:.c=.o))
FDPRINTF_OBJ	= $(addprefix $(OBJ_DIR)fd_printf/, $(FDPRINTF_SRC:.c=.o))

# Concatenate all object files
OBJ 			= $(FTIS_OBJ) $(FTMEM_OBJ) $(FTPUT_OBJ) $(FTTO_OBJ) $(FTSTR_OBJ) $(FTLST_OBJ) $(PRINTF_OBJ) $(FDPRINTF_OBJ)

# Build rules
all: 			$(NAME)

$(NAME): 		$(OBJ)
				@echo "--- Libft OBJ files ---"
				@echo $(OBJ)
				@echo "-----------------------"
				@$(AR) $(NAME) $(OBJ)

# Pattern rule using VPATH to find sources (%.c) and build objects in OBJ_DIR
$(OBJ_DIR)%.o: %.c
				@mkdir -p $(@D)
				$(CC) $(CFLAGS) -c $< -o $@

clean:
				@$(RM) -r $(OBJ_DIR)
				@$(RM) .cache_exists

fclean: 		clean
				@$(RM) $(NAME)

re: fclean all

# Include dependency files if they exist (optional, for advanced dependency tracking)
# -include $(DEP)

.PHONY: fclean all clean re
# .SILENT: # Removed to show commands
