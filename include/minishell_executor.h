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

# include <stdbool.h> // Para bool
# include <stddef.h>  // Para size_t
# include "libft.h"    // Para t_list (si se usa para la lista de comandos) y otras utilidades
#include "minishell.h"

/*
** ========================================================================== **
** >> ESTRUCTURAS ADAPTADAS DE MINISHELL_FLPEREZ (CON SUFIJO _EXE) <<
** Estas son las estructuras que el código del ejecutor de flperez (copiado
** a tu proyecto) utilizará. Han sido renombradas para evitar conflictos
** con las estructuras de minishell_bonus.
** ========================================================================== **
*/

/* --- Originalmente de minishell_flperez/inc/minishell_parser.h --- */

// Estado de las comillas (usado en t_arg_info_exe)
typedef enum e_quote_type_exec
{
    NO_QUOTE_EXEC,
    SINGLE_QUOTE_EXEC,
    DOUBLE_QUOTE_EXEC
}                           t_quote_type_exec;

// Información de argumento (usado en t_cmd_exe)
typedef struct s_arg_info_exec
{
    char                *value;
    t_quote_type_exec   q_status;
}                           t_arg_info_exe;

// Estructura para I/O de un comando (usado en t_cmd_exe)
// Se declara aquí porque t_cmd_exe la necesita. Su definición completa está más abajo.
struct s_cmd_io_exe;

// Estructura de comando (adaptada de s_cmd de flperez)
typedef struct s_cmd_exe
{
    char                    *path;      // Path al ejecutable
    t_arg_info_exe          **args;     // Argumentos del comando
    struct s_cmd_exe        *next;      // Siguiente comando en un pipeline
    struct s_cmd_exe        *prev;      // Comando anterior
    struct s_cmd_io_exe     *io;        // Información de I/O
    bool                    is_builtin;
    bool                    redir_error;
    bool                    was_literal_empty_command;
    pid_t                   pid;
}                           t_cmd_exe;

/* --- Originalmente de minishell_flperez/inc/minishell.h --- */

// Definición completa de la estructura de I/O (adaptada de s_cmd_io de flperez)
typedef struct s_cmd_io_exe
{
    char    *infile;
    char    *outfile;
    char    *heredoc_delimiter;
    bool    heredoc_quotes;
    bool    append_mode;
    int     fd_in;
    int     fd_out;
    int     stdin_backup;
    int     stdout_backup;
}                               t_cmd_io_exe;

// Estructura de datos global/de entorno (adaptada de s_data_env de flperez)
typedef struct s_data_env_exe
{
    char            **env;              // Entorno como array de strings
    char            *pwd;
    char            *old_pwd;
    t_cmd_exe       *cmds_head;         // Cabeza de la lista de t_cmd_exe
                                        // (flperez usa t_cmd *cmd, asumiendo que es la cabeza)
                                        // Si flperez usaba t_list de libft, sería: t_list *commands_list;
    int             last_exit_status;
}                               t_data_env_exe;


// Estado para la gestión interna de pipelines (de minishell.h de flperez)
// Usada por la lógica de execute_pipeline.c
typedef struct s_pipeline_state_exe
{
    int     current_pipe_fds[2];        // Descriptores para la tubería actual [READ, WRITE]
    int     *prev_pipe_read_fd_ptr;     // Puntero al extremo de lectura de la tubería anterior
                                        // (para encadenar la entrada del comando actual)
    bool    is_next_command;            // Flag para saber si hay más comandos en el pipeline
}                               t_pipeline_state_exe;

// Enum para modos de restauración de FDs (de minishell.h de flperez)
// Usada por la lógica de redirecciones.
typedef enum e_restore_mode_exe
{
	RESTORE_STDIN_ONLY_EXEC,
	RESTORE_STDOUT_ONLY_EXEC,
	RESTORE_BOTH_EXEC
}								t_restore_mode_exe;



/*
** ========================================================================== **
** >> PROTOTIPOS DE LA CAPA DE TRADUCCIÓN <<
** (Implementadas en parser_to_flperez_bridge.c)
** Toman estructuras de minishell_bonus (prefijo 'b_') y devuelven
** estructuras compatibles con el ejecutor de flperez (sufijo '_exe').
** ========================================================================== **
*/

/**
 * @brief Convierte la lista de t_cmd (de bonus) a una lista enlazada
 * de t_cmd_exe (para flperez).
 * @param b_cmd_list_head Puntero a la cabeza de la lista s_cmd de bonus.
 * @return Puntero a la cabeza de la lista de t_cmd_exe, o NULL si falla.
 */
t_cmd_exe   *convert_bonus_cmd_list_to_flperez_cmd_list(
                struct s_cmd *b_cmd_list_head); // b_cmd_list_head es del tipo t_cmd de bonus

/**
 * @brief Convierte la t_env (lista de bonus) a char **envp.
 * @param b_env_list Puntero a la cabeza de la lista s_env de bonus.
 * @return char** para t_data_env_exe->env, o NULL si falla.
 */
char        **convert_bonus_envlist_to_flperez_envp(
                struct s_env *b_env_list); // b_env_list es del tipo t_env de bonus

/**
 * @brief Libera la memoria de una lista de t_cmd_exe y todo su contenido.
 * @param flp_cmd_list_head Cabeza de la lista de t_cmd_exe a liberar.
 */
void        free_flperez_cmd_list(t_cmd_exe *flp_cmd_list_head);
// En minishell_executor.h, junto a otros prototipos de la capa de traducción
void free_flperez_arg_info_array(t_arg_info_exe **flp_args);

/*
** ========================================================================== **
** >> PROTOTIPOS DE FUNCIONES PÚBLICAS DEL EJECUTOR DE FLPEREZ <<
** (Estas funciones son parte del código copiado de flperez y ahora usarán
** las estructuras con sufijo _exe definidas arriba).
** ========================================================================== **
*/
void	child_handle_empty_or_not_found(t_cmd_exe *cmd, char **argv);
int		get_path_execution_errors(const char *exec_path, \
										const char *cmd_name_for_err);
void	execute_child_process(t_cmd_exe *cmd, t_data_env_exe *data,
								int pipe_in_fd, int pipe_out_fd);

void	parent_pipeline_handle_fds(int *pipe_fds, int *prev_pipe_fd_ptr,
									pid_t current_pid,
									pid_t *last_spawned_pid_ptr);
int		get_specific_child_exit_status(pid_t child_pid);
bool	handle_pipeline_preliminaries(t_cmd_exe *cmds, t_data_env_exe *data);
int		wait_for_all_children(pid_t last_pid);
void 	execute_pipeline(t_cmd_exe *cmds, t_data_env_exe *data);
void	setup_child_redirections(t_cmd_exe *cmd, int pipe_in_fd,
									int pipe_out_fd);
char	**build_argv_from_args(t_arg_info_exe **args);
bool	find_command_path(t_cmd_exe *cmd, t_data_env_exe *data);


int		redir_create_path_if_needed(const char *filepath);
char	*redir_determine_cmd_name(t_cmd_exe *cmd);
bool	redir_backup_fds(t_cmd_io_exe *io);
bool	redir_handle_input(t_cmd_exe *cmd, t_cmd_io_exe *io, char *cmd_name_for_err);
bool	redir_handle_output(t_cmd_exe *cmd, t_cmd_io_exe *io, char *cmd_name_for_err);
void	redir_restore_fds_on_fail(t_cmd_io_exe *io, t_restore_mode_exe mode);
bool	open_and_dup_infile(t_cmd_io_exe *io, char *cmd_name_for_err);
void	init_io_fds(t_cmd_io_exe *io);
bool	redirect_io(t_cmd_exe *cmd);
bool	restore_io(t_cmd_io_exe *io);



/* --- De builtins/exec_builtins.c de flperez --- */
// Esta función es llamada por el ejecutor de flperez si cmd_node_exe->is_builtin es true.
// Ya has integrado los builtins, esta es la interfaz que el ejecutor de flperez usará.
int         execute_builtin(t_cmd_exe *cmd_node_exe, t_data_env_exe *data_exe,
                    char **expanded_args_exe); // expanded_args_exe serían los cmd_node_exe->args->value ya expandidos si flperez lo hace así

#endif /* MINISHELL_EXECUTOR_H */