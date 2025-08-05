/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_executor.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 17:43:13 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/27 17:43:16 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef MINISHELL_EXECUTOR_H
# define MINISHELL_EXECUTOR_H

# include "minishell.h"

/*
** ========================================================================== **
** >> ESTRUCTURAS DEL EJECUTOR (ADAPTADAS DE FLPEREZ CON SUFIJO _EXE) <<
** ========================================================================== **
*/

/* Estado de las comillas */
typedef enum e_quote_type_exec
{
	NO_QUOTE_EXEC,
	SINGLE_QUOTE_EXEC,
	DOUBLE_QUOTE_EXEC
}	t_quote_type_exec;

typedef enum e_path_status
{
	PATH_FOUND_EXECUTABLE,
	PATH_ERR_NOT_FOUND,
	PATH_ERR_IS_DIRECTORY,
	PATH_ERR_NO_PERMISSION,
	PATH_ERR_STAT_FAILED,
	PATH_ERR_ACCESS_FAILED,
	PATH_ERR_MALLOC,
	PATH_NOT_EXPLICIT,
	PATH_NOT_IN_ENV,
	PATH_IS_EMPTY_CMD
}	t_path_status;

/* Información de argumento */
typedef struct s_arg_info_exec
{
	char				*value;
	t_quote_type_exec	q_status;
}	t_arg_info_exe;

struct						s_cmd_io_exe; /* Declaración adelantada */

/* Estructura de comando */
typedef struct s_cmd_exe
{
	char				*path;
	t_arg_info_exe		**args;
	struct s_cmd_exe	*next;
	struct s_cmd_exe	*prev;
	struct s_cmd_io_exe	*io;
	bool				is_builtin;
	bool				redir_error;
	bool				was_literal_empty_command;
	pid_t				pid;
	struct s_cmd		*cmd;
}	t_cmd_exe;

/* Estructura de I/O */
typedef struct s_cmd_io_exe
{
	char	*infile;
	char	*outfile;
	char	*heredoc_delimiter;
	bool	heredoc_quotes;
	bool	append_mode;
	int		fd_in;
	int		fd_out;
	int		stdin_backup;
	int		stdout_backup;
}	t_cmd_io_exe;

/* Estructura de datos del entorno para el ejecutor */
typedef struct s_data_env_exe
{
	char			**env_for_execve;
	char			*pwd;
	char			*old_pwd;
	t_cmd_exe		*cmds_head;
	int				last_exit_status;
	t_env			**shell_env_list_ptr;
}	t_data_env_exe;

/* Estado para pipelines */
typedef struct s_pipeline_state_exe
{
	int		current_pipe_fds[2];
	int		*prev_pipe_read_fd_ptr;
	bool	is_next_command;
}	t_pipeline_state_exe;

/* Modos de restauración de FDs */
typedef enum e_restore_mode_exe
{
	RESTORE_STDIN_ONLY,
	RESTORE_STDOUT_ONLY,
	RESTORE_BOTH
}	t_restore_mode_exe;

/*
** ========================================================================== **
** >> PROTOTIPOS DE LA CAPA DE TRADUCCIÓN (parser_to_exec_bridge.c) <<
** ========================================================================== **
*/
t_arg_info_exe	**transl_args_p_to_args_e(char **parser_commands);
t_cmd_exe		*convert_cmd_list_to_cms_list_exec(struct s_cmd *cmd_list_head);
char			**convert_env_list_to_exec_envp(struct s_env *env_list);
void			free_arg_info_array(t_arg_info_exe **flp_args);
void			free_cmd_list(t_cmd_exe *flp_cmd_list_head);
void			free_data_env_exe(t_data_env_exe *data);
int				fill_exec_io_from_redirections(t_cmd_io_exe *exec_io,
					t_redir *redir_list);

/*
** ========================================================================== **
** >> PROTOTIPOS DE FUNCIONES PÚBLICAS DEL EJECUTOR <<
** ========================================================================== **
*/
void			execute_pipeline_logic(t_cmd *cmd_list, t_data_env_exe *data);

/* --- Procesos y Ejecución --- */
void			execute_pipeline(t_cmd_exe *cmds, t_data_env_exe *data);
bool			find_command_path(t_cmd_exe *cmd, t_data_env_exe *data);
char			**build_argv_from_args(t_arg_info_exe **args);
void			execute_child_process(t_cmd_exe *cmd, t_data_env_exe *data,
					int pipe_in_fd, int pipe_out_fd);
void			child_handle_empty_or_not_found(t_cmd_exe *cmd, char **argv);
int				get_path_execution_errors(const char *exec_path,
					const char *cmd_name_for_err);
void			parent_pipeline_handle_fds(int *pipe_fds,
					int *prev_pipe_fd_ptr, pid_t current_pid,
					pid_t *last_spawned_pid_ptr);
bool			handle_pipeline_preliminaries(t_cmd_exe *cmds,
					t_data_env_exe *data);
int				wait_for_all_children(pid_t last_pid);

char			*ft_strjoin_path(char const *s1, char const *s2);
bool			validate_basic_cmd_input(t_cmd_exe *cmd, char **cmd_name_out);
void			handle_path_search_error(t_path_status status,
					const char *cmd_name, t_data_env_exe *data);

/* --- Redirecciones --- */
bool			redirect_io(t_cmd_exe *cmd);
bool			restore_io(t_cmd_io_exe *io);
void			setup_child_redirections(t_cmd_exe *cmd,
					int pipe_in_fd, int pipe_out_fd);
bool			open_and_dup_infile(t_cmd_io_exe *io,
					char *cmd_name_for_err);
bool			redir_handle_input(t_cmd_exe *cmd, t_cmd_io_exe *io,
					char *cmd_name_for_err);
bool			redir_handle_output(t_cmd_exe *cmd, t_cmd_io_exe *io,
					char *cmd_name_for_err);
void			redir_restore_fds_on_fail(t_cmd_io_exe *io,
					t_restore_mode_exe mode);
int				redir_create_path_if_needed(const char *filepath);
char			*redir_determine_cmd_name(t_cmd_exe *cmd);
bool			redir_backup_fds(t_cmd_io_exe *io);

/*
** ========================================================================== **
** >> PROTOTIPOS DE BUILTINS Y SUS UTILIDADES <<
** ========================================================================== **
*/

/* --- Interfaz Principal de Builtins --- */
int				execute_builtin(t_cmd_exe *cmd_node_exe,
					t_data_env_exe *data_exe, char **expanded_args_exe);
bool			is_parent_builtin(t_cmd_exe *cmd);
int				execute_parent_builtin(t_cmd_exe *cmd, t_data_env_exe *data);

/* --- Implementaciones de Builtins Específicos --- */
int				builtin_cd(char **args, t_data_env_exe *data);
int				builtin_echo(char **args);
int				builtin_env(t_data_env_exe *data, char **args);
int				builtin_exit(char **args, t_data_env_exe *data);
int				builtin_export(char **args, t_data_env_exe *data);
int				builtin_pwd(t_data_env_exe *data);
int				builtin_unset(char **args, t_data_env_exe *data);

/* --- Utilidades para CD --- */
bool			perform_directory_change(t_data_env_exe *data, char *path);
char			*get_cd_current_abs_path(t_data_env_exe *data, char *path_arg);
bool			update_all_pwd_vars(t_data_env_exe *data, char *new_dir_path);

/* --- Utilidades para EXPORT (y potencialmente UNSET/CD para lista t_env) - */
bool			parse_export_argument_pair(const char *arg,
					char **key_out, char **value_out);
void			print_export_formatted_env(t_env *env_list);
bool			add_or_update_env_list_var(t_env **list_head_ptr,
					const char *key, const char *value);

/* --- Utilidades Generales de Entorno --- */
bool			is_valid_env_var_key(const char *key);

/* --- Otras Utilidades --- */
long long		ft_atoll_with_error_check(const char *str, bool *error);
char			*get_env_var_val(char **env, char *var_name);

#endif /* MINISHELL_EXECUTOR_H */