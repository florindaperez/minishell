/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 18:46:19 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/12 17:02:59 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef MINISHELL_H
# define MINISHELL_H

/*============================================================================*/
/* INCLUDES DEL SISTEMA                                                       */
/*============================================================================*/
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <fcntl.h>
# include <errno.h>
# include <signal.h>
# include <stdbool.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <limits.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/resource.h>
# include <stddef.h>

/*============================================================================*/
/* INCLUDES PERSONALIZADOS (Libft, etc.)                                      */
/*============================================================================*/
# include "libft.h"
# include "ft_dprintf.h"

/*============================================================================*/
/* INCLUDE DEL PARSER (MOVIDO ARRIBA PARA NORMINETTE: INCLUDE_START_FILE)     */
/*============================================================================*/
/*
 * minishell_parser.h AHORA DEBE:
 * 1. Definir MAX_TOKEN_LENGTH si lo necesita directamente.
 * 2. Declarar anticipadamente (forward declare) t_cmd_io y t_data_env
 * (ej. struct s_data_env; typedef struct s_data_env t_data_env;)
 * si usa punteros a estos tipos, ya que sus definiciones completas
 * están más abajo en este archivo (minishell.h).
 * Se espera que minishell_parser.h defina las estructuras base como
 * struct s_cmd, enum e_quote_type, struct s_arg_info.
 */
# include "minishell_parser.h"

/*============================================================================*/
/* CONSTANTES ESENCIALES DE MINISHELL                                         */
/* (MAX_TOKEN_LENGTH ahora debe ser manejado en minishell_parser.h si lo usa) */
/*============================================================================*/
/* Para errores generales como malloc, token muy largo */
# define GENERAL_ERROR_STATUS 1
/* Para errores de sintaxis específicos de la shell */
# define SYNTAX_ERROR_STATUS 2
/* MAX_TOKEN_LENGTH ya no se define aquí si el parser lo necesita antes. */
/* Si es una constante global de minishell y el parser NO la usa, puede */
/* definirse aquí. Si el parser SÍ la usa, el parser debe definirla o   */
/* incluirla de un common_constants.h                                   */

/*============================================================================*/
/* TYPEDEFS PARA TIPOS CUYA ESTRUCTURA BASE SE DEFINE EN MINISHELL_PARSER.H   */
/*============================================================================*/
typedef struct s_cmd		t_cmd;
typedef enum e_quote_type	t_quote_type;
typedef struct s_arg_info	t_arg_info;

/*============================================================================*/
/* DEFINICIONES COMPLETAS DE TIPOS DE MINISHELL.H                             */
/* (minishell_parser.h usaría declar. anticipadas para estos si los necesita) */
/*============================================================================*/

/* Definición completa de t_cmd_io */
typedef struct s_cmd_io
{
	char	*infile;
	char	*outfile;
	char	*heredoc_delimiter;
	bool	heredoc_quotes;
	bool	append_mode;
	int		fd_in;
	int		fd_out;
	int		stdin_backup;	/* Backup de stdin, si se gestiona aquí */
	int		stdout_backup;	/* Backup de stdout, si se gestiona aquí */
}								t_cmd_io;

/* Definición completa de t_data_env */
typedef struct s_data_env
{
	char	**env;
	char	*pwd;
	char	*old_pwd;
	t_cmd	*cmd;			/* Usa t_cmd ( s_cmd fue definida en parser.h) */
	int		last_exit_status;
	/* Otros campos pueden ir aquí */
}								t_data_env;

/*============================================================================*/
/* OTRAS CONSTANTES, GLOBALES, ESTRUCTURAS Y ENUMS DE MINISHELL               */
/* Corregido LINE_TOO_LONG (line: 107)                                        */
/*============================================================================*/
# undef MAX_INPUT
# ifndef MAX_INPUT
#  define MAX_INPUT 16384
# endif

# ifndef PROG_NAME
#  define PROG_NAME "minishell"
# endif

/* Variable global para el estado de salida. */

extern int					g_exit_status;

typedef enum e_restore_mode
{
	RESTORE_STDIN_ONLY,
	RESTORE_STDOUT_ONLY,
	RESTORE_BOTH
}								t_restore_mode;

typedef struct s_pipeline_state
{
	int		current_pipe_fds[2];
	int		*prev_pipe_read_fd_ptr;
	bool	is_next_command;
}								t_pipeline_state;

typedef struct s_buffer_state
{
	char	*buffer;
	size_t	buf_size;
	size_t	buf_idx;
}								t_buffer_state;

/*============================================================================*/
/* PROTOTIPOS DE FUNCIONES (para main.c, executor, builtins, etc.)            */
/*============================================================================*/

/* main.c */
void		process_command_line(char *line, t_data_env *data);

/*--- Inicialización / Liberación de Memoria ---*/
bool		start_data(t_data_env *data, char **env);
void		free_data_env(t_data_env *data);
/*
 * free_commands y free_tokens prototipados en minishell_parser.h
 * si son parte del módulo parser. Si son utilidades generales,
 * pueden quedar aquí. Por coherencia, si manipulan t_cmd y
 * t_token (definidos por el parser), mejor en minishell_parser.h.
 */
void		free_commands(t_cmd *cmds_list_head);
/* void		free_tokens(t_token *head); */

void		free_ptr(void *ptr);
void		free_str_tab(char **tab);
char		*ft_strjoin_free(char *s1, const char *s2);

/*--- Manejo de Señales ---*/
void		signals_interactive(void);
void		signals_noninteractive(void);

/*--- Expansión (Variables, Argumentos, Buffer) ---*/
char		**expand_command_args(char **raw_args,
				t_data_env *data_env);
char		*expand_argument(const char *raw_token_content,
				t_quote_type original_quote_status,
				t_data_env *data_env);
bool		init_buffer_state(t_buffer_state *b_state);
bool		append_char_to_buffer(t_buffer_state *b_state,
				char c);
bool		append_string_to_buffer(t_buffer_state *b_state,
				const char *str);
bool		exp_utils_handle_sq_no_quote(t_buffer_state *b_s,
				const char *raw_token_content, size_t *i);
bool		exp_utils_handle_dq_no_quote(t_buffer_state *b_s,
				const char *raw_token_content, size_t *i,
				t_data_env *d_env);

/*--- Redirecciones ---*/
void		init_io_fds(t_cmd_io *io);
bool		redirect_io(t_cmd *cmd);
bool		restore_io(t_cmd_io *io);
void		setup_child_redirections(t_cmd *cmd, int pipe_in_fd,
				int pipe_out_fd);
bool		redir_handle_input(t_cmd *cmd, t_cmd_io *io, char *cmd_name);
bool		redir_handle_output(t_cmd *cmd, t_cmd_io *io, char *cmd_name);
char		*redir_determine_cmd_name(t_cmd *cmd);
bool		redir_backup_fds(t_cmd_io *io);
bool		open_and_dup_infile(t_cmd_io *io,
				char *cmd_name_for_err);
void		redir_restore_fds_on_fail(t_cmd_io *io,
				t_restore_mode mode);
int			redir_create_path_if_needed(const char *filepath);

/*--- Ejecución ---*/
void		execute_pipeline(t_cmd *cmds, t_data_env *data);
void		parent_pipeline_handle_fds(int *pipe_fds,
				int *prev_pipe_fd, pid_t current_pid,
				pid_t *last_spawned_pid);
int			get_specific_child_exit_status(pid_t child_pid);
bool		handle_pipeline_preliminaries(t_cmd *cmds,
				t_data_env *data);
int			wait_for_all_children(pid_t last_pid);
void		execute_child_process(t_cmd *cmd, t_data_env *data,
				int pipe_in_fd, int pipe_out_fd);
void		child_handle_empty_or_not_found(t_cmd *cmd,
				char **argv);
int			get_path_execution_errors(const char *exec_path,
				const char *cmd_name_for_err);
bool		find_command_path(t_cmd *cmd, t_data_env *data);
char		**build_argv_from_args(t_arg_info **args);

/*--- Builtins ---*/
int			execute_builtin(t_cmd *cmd, t_data_env *data,
				char **expanded_args);
bool		is_parent_builtin(t_cmd *cmd);
int			execute_parent_builtin(t_cmd *cmd,
				t_data_env *data);
int			builtin_cd(char **args, t_data_env *data);
int			builtin_echo(char **args);
int			builtin_env(t_data_env *data, char **args);
int			builtin_exit(char **args, t_data_env *data);
int			builtin_export(char **args, t_data_env *data);
int			builtin_pwd(t_data_env *data);
int			builtin_unset(char **args, t_data_env *data);
bool		perform_directory_change(t_data_env *data,
				char *path);
bool		is_valid_env_var_key(const char *key);
char		*build_env_entry(char *key, char *value);
bool		realloc_env_array(char ***old_env_array_ptr,
				int new_total_count);
bool		add_env_var(t_data_env *data, char *key,
				char *value);
long long	ft_atoll_with_error_check(const char *str,
				bool *error);
char		*get_env_var_val(char **env, char *var_name);
char		*get_env_var(char **envp, const char *var_name);
int			get_env_var_id(char **env, char *key);
int			num_var_env(char **env);

/*--- Manejo de Errores ---*/
int			msg_error_cmd(char *arg_cmd, char *descrip,
				char *err_msg, int nb_err);
void		perror_exit(const char *context, int exit_code);

/*--- Funciones de Utilidad Generales ---*/
int			ft_is_numeric(const char *str);
char		*ft_strndup(const char *src, size_t n);
int			ft_isspace(int c);
void		safe_close(int *fd);

#endif
