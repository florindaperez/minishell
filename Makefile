# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/06 17:59:33 by castorga          #+#    #+#              #
#    Updated: 2024/06/10 11:33:10 by jocuni-p         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME =	minishell

# Compiler
GCC := cc

# Compiler flags
FLAGS := -Wall -Werror -Wextra -MMD -g #-fsanitize=address

# Remove
RM 	:=	rm -rf

# Makefile
MKF :=	Makefile

# Root Folders
SRC_ROOT := src/
DEP_ROOT := .dep/
OBJ_ROOT := .objs/
INC_ROOT := include/
LIB_ROOT := lib/

INC_DIRS += ${INC_ROOT}

# Lib readline
READLINE_MK_ROOT := ${LIB_ROOT}readline/Makefile
READLINE_ROOT := ${LIB_ROOT}readline/
READLINE := ${READLINE_ROOT}libreadline.a ${READLINE_ROOT}libhistory.a

# Libft
LIBFT_ROOT := ${LIB_ROOT}libft/
LIBFT_INC := $(LIBFT_ROOT) 
LIBFT := $(LIBFT_ROOT)libft.a

INC_DIRS += ${LIBFT_INC}
LIBS += -L${LIBFT_ROOT} -lft

INC_DIRS += ${READLINE_ROOT}
LIBS += -L${READLINE_ROOT} -ltermcap

# Source Files
################################################################################

FILES = \
    main.c \
    minishell.c \
	minishell_parser.c \
	minishell_exec.c \
    signals.c \
	signals_exec.c \
    \
    arr2d/free_arr2d.c \
    arr2d/size_arr2d.c \
    \
    cmd/cmd_add_back.c \
    cmd/cmd_free.c \
    cmd/cmd_last.c \
    cmd/cmd_new_node.c \
    \
    env_list/env_cleaner_list.c \
    env_list/env_init_list.c \
    env_list/env_lstadd_back.c \
    env_list/env_lstlast.c \
    env_list/env_lstnew.c \
    \
    exit_status/get_exit_status_len.c \
    exit_status/get_exit_status_val.c \
    exit_status/set_exit_status.c \
    \
    expander/expand_quote_rm.c \
    expander/get_dollar_builder.c \
    expander/get_dollar_len.c \
    expander/get_env_key.c \
    expander/get_env_val.c \
    expander/get_len_and_free.c \
    expander/handle_dollar_invalid_syntax.c \
    expander/handle_dollar_question.c \
    expander/handle_quote_after_dollar.c \
    expander/handle_quote_builder.c \
    expander/handle_quote_len.c \
    expander/init_xpdr_except_result.c \
    expander/init_xpdr.c \
    expander/new_tok_builder.c \
    expander/new_tok_len.c \
    expander/should_expand.c \
    \
    parser/commands_counter.c \
    parser/commands_creator.c \
    parser/commands_filler.c \
    parser/handle_error.c \
    parser/is_operator.c \
    parser/is_redirection.c \
    parser/parser.c \
    parser/syntax_check_1.c \
    parser/syntax_check_2.c \
    \
    redirs/redir_add_back.c \
    redirs/redir_free.c \
    redirs/redir_last.c \
    redirs/redir_new_node.c \
    \
    tokenizer/tokenizer.c \
    tokenizer/init_operator_type.c \
    tokenizer/tok_len.c \
    tokenizer/init_word_str.c \
    \
    tokens/tok_add_back.c \
    tokens/tok_free.c \
    tokens/tok_last.c \
    tokens/tok_new_node.c \
    \
    utils/p_malloc.c \
    utils/utils1.c \
    utils/message_error.c \
	utils/utils.c \
	\
    translate/parser_to_exec_args.c \
    translate/parser_to_exec_convert.c \
    translate/parser_to_exec_free.c \
    translate/parser_to_exec_redir.c \
	translate/parser_to_exec_env.c \
    \
    executor/execute/exec_child.c \
    executor/execute/exec_child_utils.c \
    executor/execute/exec_pipeline.c \
    executor/execute/exec_pipeline_utils.c \
	executor/execute/exec_wait.c \
    executor/execute/exec_redirects.c \
    executor/execute/exec_utils.c \
    executor/execute/find_path.c \
	executor/execute/find_path_utils.c \
    executor/execute/heredoc.c \
	executor/execute/heredoc_utils.c \
    \
    executor/redirections/ft_io.c \
    executor/redirections/redir_process.c \
    executor/redirections/redir_process_utils.c \
    executor/redirections/redir_utils.c\
	\
	executor/builtins/builtin_cd.c \
    executor/builtins/builtin_cd_utils.c \
	executor/builtins/builtin_cd_update_env.c \
	executor/builtins/builtin_cd_path_utils.c \
    executor/builtins/builtin_echo.c \
    executor/builtins/builtin_env.c \
    executor/builtins/builtin_exit.c \
	executor/builtins/builtin_exit_utils.c \
    executor/builtins/builtin_export.c \
	executor/builtins/builtin_export_utils.c \
    executor/builtins/builtin_pwd.c \
    executor/builtins/builtin_unset.c \
    executor/builtins/builtin_utils.c \
	executor/builtins/env_management_utils.c \
	executor/builtins/exec_builtins.c \



SRC 	:= $(addprefix $(SRC_ROOT), $(FILES))
OBJS 	:= $(addprefix $(OBJ_ROOT), $(FILES:.c=.o))
DEPS 	:= $(addprefix $(DEP_ROOT), $(FILES:.c=.d))
INCS 	:= $(addprefix -I, $(INC_DIRS))


# Colors
################################################################################

DEF_COLOR =		\033[0;39m
DARK_YELLOW =	\033[38;5;143m
DARK_GREEN 	=	\033[1m\033[38;2;75;179;82m
GREEN 		=	\033[0;32m

# Rules
################################################################################

all:	$(READLINE_MK_ROOT)
		@echo "$(DARK_GREEN)GNU READLINE 8.2 COMPILING... $(DEF_COLOR)"
		@$(MAKE) -sC $(READLINE_ROOT)
		@echo "$(DARK_GREEN)LIBFT COMPILING... $(DEF_COLOR)"
		@$(MAKE) -C  $(LIBFT_ROOT)
		@echo "$(DARK_GREEN)MINISHELL COMPILING... $(DEF_COLOR)"
		@$(MAKE) $(NAME)
		@echo "                              $(DEF_COLOR)"
		@echo "$(GREEN)▶  MINISHELL BUILD COMPLETED!$(DEF_COLOR)"
		@echo "                              $(DEF_COLOR)"
		@echo "$(DARK_GREEN)-->	Now you can run ./minishell$(DEF_COLOR)"

$(READLINE_MK_ROOT):
		pwd ${BLOCK}
		cd ./${READLINE_ROOT} && ./configure
		cd ${BLOCK}

$(LIBFT):
		@$(MAKE) -C  $(LIBFT_ROOT)

$(NAME): $(OBJS)
		@$(GCC) $(FLAGS) $(OBJS) $(READLINE) $(LIBS) -o $(NAME)

#$(OBJ_ROOT)%.o: $(SRC_ROOT)%.c $(READLINE) $(MKF) $(LIBFT)
$(OBJ_ROOT)%.o: $(SRC_ROOT)%.c $(MKF)
		@mkdir -p $(dir $@) $(dir $(subst $(OBJ_ROOT), $(DEP_ROOT), $@))
		@echo "▶ Compiling minishell file: <$(notdir $<)>"
		@$(GCC) $(FLAGS) $(INCS) -c $< -o $@
		@mv $(patsubst %.o, %.d, $@) $(dir $(subst $(OBJ_ROOT), $(DEP_ROOT), $@))

-include $(DEPS)

readline:
		@$(MAKE) $(READLINE_MK_ROOT)

clean:
		@$(RM) $(OBJ_ROOT)
		@$(RM) $(DEP_ROOT)
		@$(MAKE) clean -C $(LIBFT_ROOT)

fclean:	clean
		@$(RM) $(NAME)
		@$(RM) $(LIBFT)

re:
		@$(MAKE) fclean
		@$(MAKE) all


cleanrl:
		@$(MAKE) clean -sC $(READLINE_ROOT)


.PHONY:	all bonus update clean fclean re
