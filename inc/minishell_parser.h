/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_parser.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:58:16 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/12 17:58:29 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_PARSER_H
# define MINISHELL_PARSER_H

// Incluir cabeceras estándar necesarias directamente
# include <stdbool.h> // Para bool
# include <stddef.h>  // Para size_t

/*
 * NO incluir "minishell.h" aquí para evitar dependencias circulares.
 * En su lugar, este archivo debe ser más autónomo:
 * 1. Definir constantes como MAX_TOKEN_LENGTH si las usa.
 * 2. Usar declaraciones anticipadas para tipos de minishell.h
 * si solo necesita punteros a ellos (ej. t_cmd_io*, t_data_env*).
 */

/*
 * Constante necesaria para t_lexer_state.
 * Si esta constante es global al proyecto, podría estar en un
 * "common_constants.h" o definirse aquí si es específica del parser
 * y minishell.h simplemente la usa también.
 * Para este ejemplo, la defino aquí.
 */
# ifndef MAX_TOKEN_LENGTH
#  define MAX_TOKEN_LENGTH 1024
# endif

/*
 * Declaraciones anticipadas para tipos definidos en minishell.h,
 * a los que minishell_parser.h necesita referirse como punteros.
 */
struct						s_cmd_io;
typedef struct s_cmd_io		t_cmd_io;

struct						s_data_env;
typedef struct s_data_env	t_data_env;

/*============================================================================*/
/* TYPE DEFINITIONS for Parser module                                         */
/*============================================================================*/

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_OUT,
	TOKEN_APPEND,
	TOKEN_HEREDOC,
	TOKEN_SEMICOLON, /* Si lo soportas */
	TOKEN_EOF
}							t_token_type;

// t_quote_type ahora se define aquí
typedef enum e_quote_type
{
	NO_QUOTE,
	SINGLE_QUOTE,
	DOUBLE_QUOTE
}							t_quote_type;

typedef struct s_token
{
	char			*args;
	t_token_type	type;
	t_quote_type	quote_status;
	bool			no_space_after;
	struct s_token	*prev;
	struct s_token	*next;
}							t_token;

typedef struct s_lexer_state
{
	char			buffer[MAX_TOKEN_LENGTH]; /* Usa MAX_TOKEN_LENGTH */
	int				buf_idx;
	t_quote_type	current_quote_state;
	t_token			*head_node;
	t_token			*current_tail_node;
	const char		*input;
	int				input_len;
	int				i;
	bool			error_flag;
}							t_lexer_state;

// t_arg_info ahora se define aquí
typedef struct s_arg_info
{
	char			*value;
	t_quote_type	q_status;
}							t_arg_info;

// t_cmd ahora se define aquí, usando t_cmd_io (declarada anticipadamente)
// y t_arg_info
typedef struct s_cmd
{
	char			*path;
	t_arg_info		**args;
	struct s_cmd	*next;
	struct s_cmd	*prev;
	t_cmd_io		*io; /* Usa t_cmd_io* (tipo opaco en este punto) */
	bool			is_builtin;
	bool			redir_error;
	bool			was_literal_empty_command;
}							t_cmd;

// Para strip_quotes.c
struct s_strip_q_data
{
	const char	*raw;
	char		*res;
	size_t		len;
};

/* Estructura de contexto para pasar datos entre funciones del parser */
typedef struct s_parser_ctx
{
	t_cmd		**cmd_ptr;
	t_token		**tk_iter_ptr;
	t_data_env	*d_env; /* Usa t_data_env* (tipo opaco en este punto) */
	t_cmd		**first_cmd_ptr;
	t_token		*curr_tk;
}							t_parser_ctx;

/*============================================================================*/
/* FUNCTION PROTOTYPES for Parser module                                      */
/*============================================================================*/

/* --- Lexer --- */
t_token				*lexer(char *input);

/* --- Funciones de lexer_utils.c --- */
t_token				*new_token(char *str_val, t_token_type type,
						t_quote_type quote_status);
void				add_token_to_list_state(t_lexer_state *ls,
						t_token *new_node);
void				free_tokens(t_token *head);
void				flush_buffer(t_lexer_state *ls,
						t_quote_type quote_status_of_token);

/* --- Funciones de lexer_token_type_utils.c --- */
bool				is_shell_operator_char(char c);
bool				is_op_type(t_token_type type);
bool				is_redir_type(t_token_type type);
bool				check_no_space_after(const char *input, int check_idx,
						int input_len);

/* --- Funciones de lexer_grammar.c --- */
bool				check_grammar(t_token *head);
bool				validate_token_sequence(t_token *current);

/* --- Funciones de lexer_operator_logic.c --- */
void				get_operator_details(t_lexer_state *ls, char *op_str_out,
						t_token_type *type_out, int *len_out);
void				create_and_add_operator_token(t_lexer_state *ls);

/* --- Funciones de lexer_processing.c --- */
void				process_char_in_quote_state(t_lexer_state *ls);
void				process_char_no_quote_state(t_lexer_state *ls);

/*----------------------------------------------------------------------------*/
/* --- Parser Core (parser.c) --- */
/* Prototipo para parse_tokens, usado por main.c (a través de minishell.h) */
t_cmd				*parse_tokens(t_token *tokens, t_data_env *data_env);

/* --- Parser Token Handlers (parser_token_handlers.c) --- */
t_cmd				*handle_operator_token(
						t_cmd *curr_cmd,
						t_token **tk_iter_ptr,
						t_cmd *first_cmd_list);
int					handle_unhandled_token(t_token *token,
						t_cmd *first_cmd_list);
int					process_redir_token(t_cmd *cmd, t_token **tk_iter_ptr,
						t_data_env *d_env, t_cmd *first_cmd_list);

/* --- Parser Command Handling (parser_command.c) --- */
t_cmd				*new_command(void);
void				add_arg_info_to_cmd(t_cmd *cmd,
						t_arg_info *new_arg_info_struct);

/* --- Parser Filename Utilities (parser_filename_utils.c) --- */
char				*assemble_filename(t_token **current_part_ptr,
						t_data_env *data_env,
						t_quote_type *initial_q_status,
						t_cmd *cmd);
bool				check_ambiguous_redirect(char *filename,
						t_quote_type initial_q_status,
						t_token *first_filename_token,
						t_cmd *cmd);

/* --- Parser Argument Assembly (parser_argument_assembly.c) --- */
char				*assemble_arg_segments(
						t_token **curr_seg_node_ptr,
						t_data_env *d_env,
						t_cmd *first_cmd_to_free
						);

/* --- Parser Redirection Handling (parser_redirection.c) --- */
int					handle_redirection(t_cmd *cmd, t_token **tokens_ptr,
						t_data_env *data_env);

/* --- Parser File Apply (parser_file_apply.c) --- */
int					handle_output_redir_open(t_cmd *cmd, t_token_type type,
						char *fname);
int					apply_file_redirection(t_cmd *cmd, t_token *redir_tk,
						char *fname);

/* --- Parser Word Token Processing (parser_word.c) --- */
int					process_word_token(t_cmd *current_cmd,
						t_token **token_iter_ptr,
						t_data_env *data_env,
						t_cmd *first_cmd);

/* --- Parser Utilities (parser_utils.c) --- */
bool				is_redirection_token(t_token_type type);
void				cleanup_previous_io(t_cmd_io *io, bool is_infile);
char				*cmd_name_for_error_in_parser(t_cmd *cmd);
/*----------------------------------------------------------------------------*/
/* --- Heredoc --- */
bool				detect_heredoc_quotes(const char *delimiter_raw);
char				*remove_quotes(const char *str_raw);
char				*generate_heredoc_filename(void);
char				*expand_heredoc_line(char *line, t_data_env *data_env);
bool				fill_heredoc(t_data_env *data_env, t_cmd_io *io,
						int tmp_fd);

/* --- Heredoc Utils --- */
bool				check_heredoc_termination(char *line_read, t_cmd_io *io,
						bool *overall_success);
bool				expand_and_write_line(char *line_read, t_cmd_io *io,
						t_data_env *data_env, int tmp_fd);

/*----------------------------------------------------------------------------*/
/* --- Argument Expansion & Quote Stripping --- */
char				*strip_quotes_from_arg(const char *raw_token_content);

#endif /* MINISHELL_PARSER_H */
