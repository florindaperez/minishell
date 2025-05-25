NAME =  minishell

# Compiler
GCC := cc

# Compiler flags
# Added -Wunused-function, -Wunused-label, -Wunreachable-code
# -Wunused-parameter can be useful but also noisy; enable if desired.
FLAGS := -Wall -Werror -Wextra -MMD -g -Wunused-function -Wunused-label -Wunreachable-code #-Wunused-parameter

# Coverage Flags (for gcov)
COVERAGE_FLAGS := -fprofile-arcs -ftest-coverage
COVERAGE_LIBS := -lgcov # Linker flag for gcov

# Remove
RM  :=  rm -rf

# Makefile
MKF :=  Makefile

# Root Folders
SRC_ROOT := src/
DEP_ROOT := .dep/
OBJ_ROOT := .objs/
INC_ROOT := include/
LIB_ROOT := lib/

# Coverage-specific folders
OBJ_COVERAGE_ROOT := .objs_coverage/
DEP_COVERAGE_ROOT := .dep_coverage/


INC_DIRS += ${INC_ROOT}

# Lib readline
READLINE_MK_ROOT := ${LIB_ROOT}readline/Makefile
READLINE_ROOT := ${LIB_ROOT}readline/
READLINE_INC := ${READLINE_ROOT} # Assuming include files are in READLINE_ROOT

# READLINE_LIB_DIR: Directory where readline's .a files are installed.
# Readline's ./configure --prefix=$(CURDIR)/$(READLINE_ROOT) typically installs into $(prefix)/lib/
# If your readline's Makefile installs .a files directly into $(READLINE_ROOT), then use:
READLINE_LIB_DIR := $(READLINE_ROOT)
# If it installs into a 'lib' subdirectory of the prefix (e.g. lib/readline/lib/):
# READLINE_LIB_DIR := $(READLINE_ROOT)lib/
# If you are certain readline installs directly into $(READLINE_ROOT), uncomment the line below and comment the one above.
# READLINE_LIB_DIR := $(READLINE_ROOT)

FIRST_READLINE_LIB := $(READLINE_LIB_DIR)libreadline.a
SECOND_READLINE_LIB := $(READLINE_LIB_DIR)libhistory.a
READLINE_LIBS_FILES := $(FIRST_READLINE_LIB) $(SECOND_READLINE_LIB)


# Libft
LIBFT_ROOT := ${LIB_ROOT}libft/
LIBFT_INC := $(LIBFT_ROOT)
LIBFT := $(LIBFT_ROOT)libft.a

INC_DIRS += ${LIBFT_INC}
LIBS := -L$(LIBFT_ROOT) -lft # Base LIBS

# Add readline includes and library paths
INC_DIRS += ${READLINE_INC}
LIBS += -L$(READLINE_LIB_DIR) # Add readline lib path for linker (e.g. for -ltermcap if it also comes from there)


# Source Files
################################################################################

FILES = main.c \
		minishell.c \
		arr2d/add_one_arr2d.c \
		arr2d/dup_arr2d.c \
		arr2d/free_arr2d.c \
		arr2d/rm_one_arr2d.c \
		arr2d/size_arr2d.c \
		builtins/builtin_cd.c \
		builtins/builtin_echo.c \
		builtins/builtin_env.c \
		builtins/builtin_exit.c \
		builtins/builtin_export.c \
		builtins/builtin_pwd.c \
		builtins/builtin_unset.c \
		builtins/builtins.c \
		builtins/utils_pwd/oldpwd.c \
		builtins/utils_export/var_exists.c \
		builtins/utils_export/checks.c \
		cmd/cmd_add_back.c \
		cmd/cmd_free.c \
		cmd/cmd_last.c \
		cmd/cmd_new_node.c \
		cmd/cmd_size.c \
		env_list/env_cleaner_list.c \
		env_list/env_delone_lst.c \
		env_list/env_init_list.c \
		env_list/env_lstadd_back.c \
		env_list/env_lstlast.c \
		env_list/env_lstnew.c \
		executor/executor.c \
		executor/redirections.c \
		executor/path.c \
		executor/heredoc.c \
		exit_status/error_msgs.c \
		exit_status/error_msgs2.c \
		exit_status/get_exit_status_len.c \
		exit_status/get_exit_status_val.c \
		exit_status/set_exit_status.c \
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
		expander/should_expand.c\
		parser/commands_counter.c \
		parser/commands_creator.c \
		parser/commands_filler.c \
		parser/handle_error.c \
		parser/is_operator.c \
		parser/is_redirection.c \
		parser/parser.c \
		parser/syntax_check_1.c \
		parser/syntax_check_2.c \
		prints/print_arr2d.c \
		prints/print_cmd_para_executor.c \
		prints/print_cmd.c \
		prints/print_envlist.c \
		prints/print_redir.c \
		prints/print_tok.c \
		redirs/redir_add_back.c \
		redirs/redir_free.c \
		redirs/redir_last.c \
		redirs/redir_new_node.c \
		redirs/redir_size.c \
		tokenizer/tokenizer.c \
		tokenizer/init_operator_type.c \
		tokenizer/tok_len.c \
		tokenizer/init_word_str.c \
		tokens/tok_add_back.c \
		tokens/tok_free.c \
		tokens/tok_last.c \
		tokens/tok_new_node.c \
		tokens/tok_size.c \
		utils/p_malloc.c \
		utils/utils1.c \
		utils/utils3.c \
		utils/signals_utils.c \
		utils/wellcome_msg.c \
		init_exe.c \
		signals.c

SRC     := $(addprefix $(SRC_ROOT), $(FILES))
OBJS    := $(addprefix $(OBJ_ROOT), $(FILES:.c=.o))
DEPS    := $(addprefix $(DEP_ROOT), $(FILES:.c=.d))

# Coverage objects and dependencies
OBJS_COVERAGE := $(addprefix $(OBJ_COVERAGE_ROOT), $(FILES:.c=.o))
DEPS_COVERAGE := $(addprefix $(DEP_COVERAGE_ROOT), $(FILES:.c=.d))

INCS    := $(addprefix -I, $(INC_DIRS))


# Colors
################################################################################

DEF_COLOR =     \033[0;39m
DARK_YELLOW =   \033[38;5;143m
DARK_GREEN  =   \033[1m\033[38;2;75;179;82m
GREEN       =   \033[0;32m
RED         =   \033[0;31m

# Rules
################################################################################

# The 'all' target now simply depends on the final executable $(NAME).
# The dependencies of $(NAME) will ensure everything is built in order.
all: $(NAME)
		@echo "                              $(DEF_COLOR)"
		@echo "$(GREEN)▶  MINISHELL BUILD COMPLETED!$(DEF_COLOR)"
		@echo "                              $(DEF_COLOR)"
		@echo "$(DARK_GREEN)--> Now you can run ./$(NAME)$(DEF_COLOR)"

# Rule to configure readline (creates its Makefile)
$(READLINE_MK_ROOT):
		@if [ ! -f "$(READLINE_MK_ROOT)" ]; then \
			echo "$(DARK_YELLOW)Configuring GNU Readline...$(DEF_COLOR)"; \
			(cd ./${READLINE_ROOT} && ./configure --prefix=$(CURDIR)/$(READLINE_ROOT) && echo "$(GREEN)Readline configured.$(DEF_COLOR)"); \
		else \
			echo "$(GREEN)GNU Readline already configured.$(DEF_COLOR)"; \
		fi

# Rule to build Libft
$(LIBFT):
		@echo "$(DARK_YELLOW)Building Libft...$(DEF_COLOR)"
		@$(MAKE) -C  $(LIBFT_ROOT)
		@echo "$(GREEN)Libft build complete.$(DEF_COLOR)"

# Rule to build readline libraries
# This target represents the successful compilation of readline libraries.
# It depends on the readline Makefile being configured.
$(FIRST_READLINE_LIB): $(READLINE_MK_ROOT)
		@echo "$(DARK_GREEN)Building GNU Readline libraries... $(DEF_COLOR)"
		@$(MAKE) -sC $(READLINE_ROOT)
		@if [ ! -f "$(FIRST_READLINE_LIB)" ]; then \
			echo "$(RED)Error: Readline build did not produce $(FIRST_READLINE_LIB)$(DEF_COLOR)"; \
			echo "$(RED)Please check if readline's Makefile installs to '$(READLINE_LIB_DIR)' (current setting).$(DEF_COLOR)"; \
			echo "$(RED)If it installs directly to '$(READLINE_ROOT)', adjust READLINE_LIB_DIR definition.$(DEF_COLOR)"; \
			exit 1; \
		fi
		@# Check for history library, can be optional or part of libreadline in some configs
		@if [ ! -f "$(SECOND_READLINE_LIB)" ]; then \
			echo "$(DARK_YELLOW)Warning: $(SECOND_READLINE_LIB) not found after readline build. It might be optional or included in libreadline.a.$(DEF_COLOR)"; \
		fi

# This rule ensures that if make tries to build libhistory.a explicitly,
# it first ensures libreadline.a (and thus the readline 'make' command) is run.
# No recipe needed here if libhistory.a is built by the same 'make' command as libreadline.a
$(SECOND_READLINE_LIB): $(FIRST_READLINE_LIB)
		@if [ ! -f "$(SECOND_READLINE_LIB)" ]; then \
			echo "$(DARK_YELLOW)Note: $(SECOND_READLINE_LIB) was not found. Ensure it's built if required by your project.$(DEF_COLOR)"; \
		fi


# $(NAME) depends on the actual library files being present.
# $^ expands to all prerequisites: $(OBJS) $(LIBFT) $(FIRST_READLINE_LIB) $(SECOND_READLINE_LIB)
$(NAME): $(OBJS) $(LIBFT) $(FIRST_READLINE_LIB) $(SECOND_READLINE_LIB)
		@echo "$(DARK_GREEN)Linking $(NAME)...$(DEF_COLOR)"
		@$(GCC) $(FLAGS) $^ $(LIBS) -ltermcap -o $(NAME)
		@echo "$(GREEN)$(NAME) linked successfully.$(DEF_COLOR)"


$(OBJ_ROOT)%.o: $(SRC_ROOT)%.c $(MKF)
		@mkdir -p $(dir $@) $(dir $(patsubst $(OBJ_ROOT)%.o,$(DEP_ROOT)%.d,$@))
		@echo "$(GREEN)Compiling:$(DEF_COLOR) $<"
		@$(GCC) $(FLAGS) $(INCS) -c $< -o $@
		@$(GCC) $(FLAGS) $(INCS) -MM $< -MT $@ -MF $(patsubst $(OBJ_ROOT)%.o,$(DEP_ROOT)%.d,$@)

-include $(DEPS)


# --- Dead Code Detection Targets ---

.PHONY: cppcheck
cppcheck:
		@echo "$(DARK_YELLOW)Running Cppcheck for static analysis...$(DEF_COLOR)"
		@cppcheck --enable=unusedFunction --enable=style --error-exitcode=1 \
			--inline-suppr --suppress=missingIncludeSystem \
			$(addprefix -I, $(INC_DIRS)) $(SRC_ROOT) \
			|| (echo "$(RED)Cppcheck found issues.$(DEF_COLOR)" && exit 1)
		@echo "$(GREEN)Cppcheck analysis complete.$(DEF_COLOR)"

# --- Coverage Targets ---
.PHONY: clean_objects
clean_objects:
		@echo "$(DARK_YELLOW)Cleaning project object files...$(DEF_COLOR)"
		@$(RM) $(OBJ_ROOT) $(DEP_ROOT)
		@$(RM) $(OBJ_COVERAGE_ROOT) $(DEP_COVERAGE_ROOT)

.PHONY: coverage_build
coverage_build: clean_objects $(LIBFT) $(FIRST_READLINE_LIB) $(SECOND_READLINE_LIB) # Ensure libs are ready
		@echo "$(DARK_GREEN)LIBFT COMPILING (for coverage)... $(DEF_COLOR)"
		@$(MAKE) -C $(LIBFT_ROOT) CFLAGS="$(COVERAGE_FLAGS) $(FLAGS)" # Pass coverage and standard flags
		@echo "$(DARK_GREEN)MINISHELL COMPILING (for coverage)... $(DEF_COLOR)"
		@$(MAKE) $(NAME)_coverage
		@echo "                              $(DEF_COLOR)"
		@echo "$(GREEN)▶  MINISHELL COVERAGE BUILD COMPLETED! Target: $(NAME)_coverage$(DEF_COLOR)"

# Rule for compiling coverage objects
$(OBJ_COVERAGE_ROOT)%.o: $(SRC_ROOT)%.c $(MKF)
		@mkdir -p $(dir $@) $(dir $(patsubst $(OBJ_COVERAGE_ROOT)%.o,$(DEP_COVERAGE_ROOT)%.d,$@))
		@echo "$(GREEN)Compiling for coverage:$(DEF_COLOR) $<"
		@$(GCC) $(FLAGS) $(COVERAGE_FLAGS) $(INCS) -c $< -o $@
		@$(GCC) $(FLAGS) $(COVERAGE_FLAGS) $(INCS) -MM $< -MT $@ -MF $(patsubst $(OBJ_COVERAGE_ROOT)%.o,$(DEP_COVERAGE_ROOT)%.d,$@)

-include $(DEPS_COVERAGE)

# $(NAME)_coverage depends on its objects and the pre-built libraries.
# $^ expands to all prerequisites.
$(NAME)_coverage: $(OBJS_COVERAGE) $(LIBFT) $(FIRST_READLINE_LIB) $(SECOND_READLINE_LIB)
		@echo "$(DARK_GREEN)Linking $(NAME)_coverage...$(DEF_COLOR)"
		@$(GCC) $(FLAGS) $(COVERAGE_FLAGS) $^ $(LIBS) -ltermcap $(COVERAGE_LIBS) -o $(NAME)_coverage
		@echo "$(GREEN)$(NAME)_coverage linked successfully.$(DEF_COLOR)"


.PHONY: coverage_run_example
coverage_run_example: $(NAME)_coverage
		@echo "$(DARK_YELLOW)To generate coverage data, run ./$(NAME)_coverage with your inputs/tests.$(DEF_COLOR)"
		@echo "$(DARK_YELLOW)Example: ./$(NAME)_coverage < my_test_script.txt$(DEF_COLOR)"
		@echo "$(DARK_YELLOW)Or run interactively and then exit minishell to write .gcda files.$(DEF_COLOR)"

.PHONY: coverage_report
coverage_report:
		@echo "$(DARK_YELLOW)Generating LCOV HTML report...$(DEF_COLOR)"
		@lcov --capture --directory . --output-file coverage.info --gcov-tool gcov \
			--base-directory $(CURDIR) \
			--include '$(CURDIR)/$(SRC_ROOT)*' \
			|| (echo "$(RED)LCOV capture failed. Did you run the instrumented program? (.gcda files missing?)$(DEF_COLOR)" && exit 1)
		@lcov --remove coverage.info '/usr/*' --output-file coverage.info # Remove system headers
		@lcov --remove coverage.info '*/$(LIB_ROOT)/*' --output-file coverage.info # Remove libft and readline if their source is there
		@genhtml coverage.info --output-directory coverage_html_report \
			|| (echo "$(RED)genhtml failed. Is lcov data valid?$(DEF_COLOR)" && exit 1)
		@echo "$(GREEN)▶ LCOV HTML report generated in ./coverage_html_report/index.html$(DEF_COLOR)"
		@echo "$(GREEN)  Open it with: xdg-open ./coverage_html_report/index.html (on Linux)$(DEF_COLOR)"

.PHONY: coverage_clean
coverage_clean:
		@echo "$(DARK_YELLOW)Cleaning coverage files...$(DEF_COLOR)"
		@$(RM) $(OBJ_COVERAGE_ROOT) $(DEP_COVERAGE_ROOT)
		@$(RM) *.gcda *.gcno # Remove from project root
		@find $(SRC_ROOT) \( -name '*.gcda' -o -name '*.gcno' -o -name '*.gcov' \) -delete # Remove from src tree
		@find $(OBJ_COVERAGE_ROOT) \( -name '*.gcda' -o -name '*.gcno' -o -name '*.gcov' \) -delete # Remove from obj_coverage tree
		@$(RM) coverage.info coverage_html_report
		@$(RM) $(NAME)_coverage
		@echo "$(DARK_YELLOW)Note: If libft was compiled for coverage, clean it separately if needed.$(DEF_COLOR)"


# --- Standard Clean Targets ---
clean:
		@echo "$(DARK_YELLOW)Cleaning project object files and dependency files...$(DEF_COLOR)"
		@$(RM) $(OBJ_ROOT) $(DEP_ROOT)
		@$(RM) $(OBJ_COVERAGE_ROOT) $(DEP_COVERAGE_ROOT) # Also clean coverage objects
		@echo "$(DARK_YELLOW)Cleaning Libft...$(DEF_COLOR)"
		@$(MAKE) clean -C $(LIBFT_ROOT)
		@echo "$(GREEN)Clean complete.$(DEF_COLOR)"

fclean: clean
		@echo "$(DARK_YELLOW)Performing full clean...$(DEF_COLOR)"
		@$(RM) $(NAME)
		@$(RM) $(NAME)_coverage # Also remove coverage executable
		@echo "$(DARK_YELLOW)Full cleaning Libft...$(DEF_COLOR)"
		@$(MAKE) fclean -C $(LIBFT_ROOT)
		@echo "$(DARK_YELLOW)Cleaning coverage report files...$(DEF_COLOR)"
		@$(RM) *.gcda *.gcno *.gcov # General cleanup
		@find $(SRC_ROOT) \( -name '*.gcda' -o -name '*.gcno' -o -name '*.gcov' \) -delete
		@$(RM) -r coverage.info coverage_html_report # Remove lcov outputs
		@echo "$(GREEN)Full clean complete.$(DEF_COLOR)"

re:
		@$(MAKE) fclean
		@$(MAKE) all

# --- Readline Specific ---
readline_force_configure:
		@echo "$(DARK_YELLOW)Forcing re-configure of GNU Readline...$(DEF_COLOR)"
		(cd ./${READLINE_ROOT} && ./configure --prefix=$(CURDIR)/$(READLINE_ROOT))

# This target is more for explicit invocation if needed, actual build is handled by $(FIRST_READLINE_LIB) rule
readline_build: $(FIRST_READLINE_LIB)
		@echo "$(GREEN)GNU Readline build process ensured by dependency.$(DEF_COLOR)"


cleanrl:
		@echo "$(DARK_YELLOW)Cleaning Readline build files...$(DEF_COLOR)"
		@if [ -f "$(READLINE_MK_ROOT)" ]; then \
			$(MAKE) clean -sC $(READLINE_ROOT); \
		else \
			echo "$(DARK_YELLOW)Readline Makefile not found, skipping clean.$(DEF_COLOR)"; \
		fi
		# Consider also removing the configure artifacts if you want a deeper clean:
		# $(RM) $(READLINE_ROOT)config.status $(READLINE_ROOT)config.log $(READLINE_ROOT)config.h

.PHONY: all bonus update clean fclean re cppcheck coverage_build coverage_run_example coverage_report coverage_clean clean_objects help readline_build readline_force_configure cleanrl

# --- Help Target ---
.PHONY: help
help:
		@echo ""
		@echo "$(DARK_GREEN)Available targets for Minishell Makefile:$(DEF_COLOR)"
		@echo "  ------------------------------------------------------------------"
		@echo "  $(GREEN)Build Targets:$(DEF_COLOR)"
		@echo "    all                  Build the project ($(NAME))"
		@echo "    $(NAME)              Build the project executable"
		@echo "    re                   Force clean and rebuild the project"
		@echo ""
		@echo "  $(GREEN)Cleaning Targets:$(DEF_COLOR)"
		@echo "    clean                Remove object files and libft objects"
		@echo "    fclean               Remove all generated files (incl. executables, libs, reports)"
		@echo ""
		@echo "  $(GREEN)Static Analysis & Dead Code:$(DEF_COLOR)"
		@echo "    cppcheck             Run Cppcheck for static code analysis"
		@echo "                         (Detects unused functions, style issues, etc.)"
		@echo "                         (Compiler warnings also help: -Wunused-function, etc.)"
		@echo ""
		@echo "  $(GREEN)Code Coverage (gcov/lcov):$(DEF_COLOR)"
		@echo "    coverage_build       Build $(NAME)_coverage with instrumentation for code coverage"
		@echo "    coverage_run_example Show example command to run the coverage-instrumented binary"
		@echo "                         (You need to run ./$(NAME)_coverage with your inputs/tests)"
		@echo "    coverage_report      Generate HTML code coverage report (after running $(NAME)_coverage)"
		@echo "                         Report will be in ./coverage_html_report/"
		@echo "    coverage_clean       Clean all coverage-related files and build artifacts"
		@echo ""
		@echo "  $(GREEN)Library Specific:$(DEF_COLOR)"
		@echo "    readline_build       Ensure GNU Readline library is built"
		@echo "    readline_force_configure Force re-configuration of GNU Readline"
		@echo "    cleanrl              Clean readline build files from its directory"
		@echo "  ------------------------------------------------------------------"
		@echo ""
