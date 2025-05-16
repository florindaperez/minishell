# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: asans-go <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/19 17:23:36 by asans-go          #+#    #+#              #
#    Updated: 2025/02/19 17:23:40 by asans-go         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# --- Nombre del Ejecutable ---
NAME = minishell 

# --- Compilador y Banderas ---
CC = gcc
# *** CAMBIO: Add -MMD -MP para generar dependencias ***
CFLAGS = -Wall -Wextra -Werror -g -MMD -MP #-fsanitize=address
RM = rm -rf

# --- Directorios ---
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc
LIBFT_DIR = libft
LIBFT_INC = $(LIBFT_DIR)/inc

# --- Archivos Fuente (.c) ---
SRCS =  main/main.c \
        signals/signal.c \
        execute/exec_child.c \
		execute/exec_child_utils.c \
		execute/exec_pipeline.c \
		execute/exec_pipeline_utils.c \
		execute/exec_redirects.c \
		execute/exec_utils.c \
        execute/find_path.c \
        parser/detect_token_type.c \
        parser/heredoc_logic.c \
		parser/heredoc_fill_utils.c \
        parser/lexer.c \
		parser/lexer_utils.c \
		parser/lexer_token_type_utils.c \
		parser/lexer_grammar.c \
		parser/lexer_processing.c \
		parser/lexer_operator_logic.c \
        parser/parser.c\
		parser/parser_token_handlers.c \
		parser/parser_utils.c\
		parser/parser_command.c\
		parser/parser_redirection.c\
		parser/parser_file_apply.c \
		parser/parser_filename_utils.c\
		parser/parser_word.c\
		parser/parser_argument_assembly.c\
        parser/strip_quotes.c\
        builtins/builtin_cd.c \
        builtins/builtin_cd_utils.c \
        builtins/builtin_echo.c \
        builtins/builtin_env.c \
        builtins/builtin_exit.c \
		builtins/builtin_exit_utils.c \
        builtins/builtin_export.c \
        builtins/builtin_pwd.c \
        builtins/builtin_unset.c \
        builtins/builtin_utils.c \
		builtins/env_management_utils.c \
		builtins/exec_builtins.c \
        redirections/ft_io.c \
		redirections/redir_utils.c \
		redirections/redir_process.c \
		redirections/redir_process_utils.c \
		expand/expand_argument.c\
		expand/expand_buffer.c\
		expand/expand_utils.c\
		expand/expand_command_args.c\
        utils/error_utils.c \
        utils/free.c \
        utils/ft_free.c \
        utils/message_error.c \
        utils/start_data_env.c \
        utils/utils.c

# --- Archivos Objeto (.o) ---
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

# --- Archivos de Dependencia (.d) ---
# *** NUEVO: Lista de archivos .d generados automáticamente ***
DEPS = $(OBJS:.o=.d)

# --- Archivos de Cabecera (Informativo, no usado directamente en reglas patrón) ---
HEADERS = $(INC_DIR)/minishell.h $(INC_DIR)/minishell_parser.h $(LIBFT_INC)/libft.h $(INC_DIR)/ft_dprintf.h

# Includes para el compilador (-I flags)
INC_DIRS = -I$(INC_DIR) -I$(LIBFT_INC)

# --- Bibliotecas ---
LIBFT_A = $(LIBFT_DIR)/libft.a
LIB_DIRS = -L$(LIBFT_DIR)
LIBS = -lft -lreadline

# --- Regla Principal (Default Goal) ---
.PHONY: all
all: $(NAME)

# --- Regla para Crear el Ejecutable Final ---
$(NAME): $(OBJS) $(LIBFT_A)
	@echo "Enlazando $(NAME)..."
	$(CC) $(CFLAGS) $(OBJS) $(LIB_DIRS) $(LIBS) -o $(NAME)
	@echo "$(NAME) compilado exitosamente!"

# --- Regla para Compilar Libft ---
$(LIBFT_A):
	@echo "Compilando Libft..."
	@$(MAKE) -sC $(LIBFT_DIR)

# --- Regla Patrón para Crear Archivos Objeto ---
# La dependencia de las cabeceras se maneja automáticamente con los archivos .d
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

# --- Regla de Limpieza ---
.PHONY: clean
clean:
	@echo "Limpiando archivos objeto y dependencias..."
	@$(MAKE) clean -sC $(LIBFT_DIR)
	@$(RM) $(OBJ_DIR) # Elimina el directorio obj y su contenido (.o y .d)

# --- Regla de Limpieza Profunda ---
.PHONY: fclean
fclean: clean
	@echo "Limpiando ejecutable $(NAME)..."
	@$(MAKE) fclean -sC $(LIBFT_DIR)
	@$(RM) $(NAME)

# --- Regla para Recompilar Todo ---
.PHONY: re
re: fclean all

# --- Ayuda (Opcional) ---
.PHONY: help
help:
	@echo "Uso: make [TARGET]"
	@echo "TARGETS:"
	@echo "  all     Compila todo el proyecto (por defecto)"
	@echo "  clean   Elimina los archivos objeto y de dependencia generados"
	@echo "  fclean  Elimina archivos objeto, dependencias y el ejecutable final"
	@echo "  re      Fuerza una recompilación completa (fclean + all)"
	@echo "  help    Muestra este mensaje de ayuda"

# --- Incluir Archivos de Dependencia ---
# El '-' inicial evita errores si los archivos .d no existen (la primera vez)
-include $(DEPS)

