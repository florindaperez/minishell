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

#include "minishell.h"

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
}							t_quote_type_exec;

typedef enum e_path_status
{
	PATH_FOUND_EXECUTABLE,      // 0: Ruta válida y ejecutable
	PATH_ERR_NOT_FOUND,         // 1: No existe (ENOENT de stat)
	PATH_ERR_IS_DIRECTORY,      // 2: Es un directorio (S_ISDIR)
	PATH_ERR_NO_PERMISSION,     // 3: Sin permiso (EACCES de access X_OK o stat)
	PATH_ERR_STAT_FAILED,       // 4: Otro error de stat
	PATH_ERR_ACCESS_FAILED,     // 5: Otro error de access (no EACCES, raro)
	PATH_ERR_MALLOC,            // 6: Fallo de memoria (strdup, ft_split, ft_strjoin_path)
	PATH_NOT_EXPLICIT,          // 7: No era una ruta explícita (para lógica interna)
	PATH_NOT_IN_ENV,            // 8: No se encontró en PATH (o PATH no existe/vacío)
	PATH_IS_EMPTY_CMD           // 9: El nombre del comando es una cadena vacía ""
}	t_path_status;

/* Información de argumento */
typedef struct s_arg_info_exec
{
	char				*value;
	t_quote_type_exec	q_status;
}							t_arg_info_exe;

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
}							t_cmd_exe;

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
}								t_cmd_io_exe;

/* Estructura de datos del entorno para el ejecutor */
typedef struct s_data_env_exe
{
	char			**env_for_execve;	/* Para execve, generado de t_env* */
	char			*pwd;				/* Cache del PWD actual */
	char			*old_pwd;			/* Cache del OLDPWD anterior */
	t_cmd_exe		*cmds_head;			/* Cabeza de la lista de t_cmd_exe */
	int				last_exit_status;	/* Último código de salida */
	t_env			**shell_env_list_ptr; /* Ptr a la lista t_env principal */
}								t_data_env_exe;

/* Estado para pipelines */
typedef struct s_pipeline_state_exe
{
	int		current_pipe_fds[2];
	int		*prev_pipe_read_fd_ptr;
	bool	is_next_command;
}								t_pipeline_state_exe;

/* Modos de restauración de FDs */
typedef enum e_restore_mode_exe
{
	RESTORE_STDIN_ONLY,
	RESTORE_STDOUT_ONLY,
	RESTORE_BOTH
}								t_restore_mode_exe;

/*
** ========================================================================== **
** >> PROTOTIPOS DE LA CAPA DE TRADUCCIÓN (parser_to_exec_bridge.c) <<
** ========================================================================== **
*/
t_cmd_exe					*convert_cmd_list_to_cms_list_exec(\
								struct s_cmd *b_cmd_list_head);
char						**convert_envlist_to_envp_exe(\
								t_env *b_env_list); /* Renombrado */
void						free_flperez_cmd_list(t_cmd_exe *flp_cmd_list_head);
void						free_flperez_arg_info_array(\
								t_arg_info_exe **flp_args);
void						free_data_env_exe(t_data_env_exe *data);

/*
** ========================================================================== **
** >> PROTOTIPOS DE FUNCIONES PÚBLICAS DEL EJECUTOR <<
** ========================================================================== **
*/
/* --- Procesos y Ejecución --- */
void						execute_pipeline(t_cmd_exe *cmds, \
								t_data_env_exe *data);
bool						find_command_path(t_cmd_exe *cmd, \
								t_data_env_exe *data);
char						**build_argv_from_args(t_arg_info_exe **args);
void						execute_child_process(t_cmd_exe *cmd, \
								t_data_env_exe *data, int pipe_in_fd, \
								int pipe_out_fd);
void						child_handle_empty_or_not_found(t_cmd_exe *cmd, \
								char **argv);
int							get_path_execution_errors(const char *exec_path, \
								const char *cmd_name_for_err);
void						parent_pipeline_handle_fds(int *pipe_fds, \
								int *prev_pipe_fd_ptr, pid_t current_pid, \
								pid_t *last_spawned_pid_ptr);
int							get_specific_child_exit_status(pid_t child_pid);
bool						handle_pipeline_preliminaries(t_cmd_exe *cmds, \
								t_data_env_exe *data);
int							wait_for_all_children(pid_t last_pid);

char						*ft_strjoin_path(char const *s1, char const *s2);


/* --- Redirecciones --- */
void						init_io_fds(t_cmd_io_exe *io);
bool						redirect_io(t_cmd_exe *cmd);
bool						restore_io(t_cmd_io_exe *io);
void						setup_child_redirections(t_cmd_exe *cmd, \
								int pipe_in_fd, int pipe_out_fd);
bool						open_and_dup_infile(t_cmd_io_exe *io, \
								char *cmd_name_for_err);
bool						redir_handle_input(t_cmd_exe *cmd, \
								t_cmd_io_exe *io, char *cmd_name_for_err);
bool						redir_handle_output(t_cmd_exe *cmd, \
								t_cmd_io_exe *io, char *cmd_name_for_err);
void						redir_restore_fds_on_fail(t_cmd_io_exe *io, \
								t_restore_mode_exe mode);
int							redir_create_path_if_needed(const char *filepath);
char						*redir_determine_cmd_name(t_cmd_exe *cmd);
bool						redir_backup_fds(t_cmd_io_exe *io);


/*
** ========================================================================== **
** >> PROTOTIPOS DE BUILTINS Y SUS UTILIDADES <<
** ========================================================================== **
*/

/* --- Interfaz Principal de Builtins --- */
int							execute_builtin(t_cmd_exe *cmd_node_exe, \
								t_data_env_exe *data_exe, \
								char **expanded_args_exe);
bool						is_parent_builtin(t_cmd_exe *cmd);
int							execute_parent_builtin(t_cmd_exe *cmd, \
								t_data_env_exe *data);

/* --- Implementaciones de Builtins Específicos --- */
int							builtin_cd(char **args, t_data_env_exe *data);
int							builtin_echo(char **args);
int							builtin_env(t_data_env_exe *data, char **args);
int							builtin_exit(char **args, t_data_env_exe *data);
int							builtin_export(char **args, t_data_env_exe *data);
int							builtin_pwd(t_data_env_exe *data);
int							builtin_unset(char **args, t_data_env_exe *data);

/* --- Utilidades para CD --- */
/* De builtin_cd_utils.c (o builtin_cd_core.c) */
bool						perform_directory_change(t_data_env_exe *data, \
								char *path);
/* De builtin_cd_path_utils.c */
char						*get_cd_current_abs_path(t_data_env_exe *data, \
								char *path_arg);
/* De builtin_cd_env_update_utils.c o builtin_cd_env.c */
bool						update_all_pwd_vars(t_data_env_exe *data, \
								char *new_dir_path);

/* --- Utilidades para EXPORT (y potencialmente UNSET/CD para lista t_env) --- */
/* De builtin_export_utils.c */
bool						parse_export_argument_pair(const char *arg, \
								char **key_out, char **value_out);
void						print_export_formatted_env(t_env *env_list);
/* Funciones clave para modificar la lista t_env* (Fuente Única de Verdad) */
/* Estas podrían estar en env_list_utils.c o similar */
bool						add_or_update_env_list_var(t_env **list_head_ptr, \
								const char *key, const char *value);
/* bool remove_var_from_env_list(t_env **list_head_ptr, const char *key); // Para unset */


/* --- Utilidades Generales de Entorno (principalmente para char **env_for_execve) --- */
/* De env_management_utils.c */
bool						is_valid_env_var_key(const char *key);
char						*build_env_entry(char *key, char *value);
bool						add_env_var(t_data_env_exe *data, char *key, \
								char *value);


int		num_var_env(char **env);
int		get_env_var_id(char **env, char *key);

/* --- Otras Utilidades --- */
long long					ft_atoll_with_error_check(const char *str, \
								bool *error);
/* Decide cuál es la principal o si ambas son necesarias. */
 char						*get_env_var_val(char **env, char *var_name);


#endif /* MINISHELL_EXECUTOR_H */