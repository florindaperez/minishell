/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 16:27:34 by jocuni-p          #+#    #+#             */
/*   Updated: 2024/06/10 11:25:40 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# ifndef READLINE_LIBRARY
#  define READLINE_LIBRARY
# endif

/*--------Libraries---------*/
# include "libft.h"
# include <readline.h>
# include <history.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <limits.h>
# include <errno.h>
# include <sys/stat.h>
# include <stdbool.h>

// === INCLUSIÓN DEL NUEVO HEADER CON ESTRUCTURAS Y PROTOTIPOS DE FLPEREZ ===
//# include "minishell_executor.h" //comentada por  referencia circular 

/*-------- Defines para Rutas --------*/
// Fallback para PATH_MAX si no está definido por <limits.h>
#ifndef PATH_MAX
    #define MINISHELL_PATH_BUF_SIZE 4096 // Un tamaño común y generoso para rutas
#else
    #define MINISHELL_PATH_BUF_SIZE PATH_MAX
#endif

/*-----------------Defines-------------*/
# define EXIT_SUCCESS 0
# define EXIT_FAILURE 1

# define TRUE 1
# define FALSE 0

# define RED "\033[0;31m"
# define GREEN "\033[1;32m"
# define YELLOW "\033[0;33m"
# define BLUE "\033[1;34m"
# define END "\033[0m"
# define CYAN "\033[1;36m"
# define BLACK "\033[40m"
# define WHITE "\033[1;37m"
# define CLEAN_SCREEN "\033[2J"

/*--------------Signals-------------------*/
# define CTRL_C SIGINT
# define CTRL_D SIGQUIT // En muchos sistemas, Ctrl+D es EOF, Ctrl+\ es SIGQUIT
# define CTRL_SLASH SIGQUIT
# define CTRL_Z SIGTSTP


/*--------------------Error messages-------------------*/
# define PRINT_SYNTAX_ERR_1 "syntax error near unexpected token `|'\n"
# define PRINT_SYNTAX_ERR_2 "syntax error near unexpected token `newline'\n"
# define PRINT_SYNTAX_ERR_3 "syntax error\n"

/*------------------Quote status--------*/
# define CLOSED 0
# define OPEN 1

/*-----------global variable------------*/
extern int	g_get_signal;

/*--------------------------- Pipe ---------------------------*/
# define READ 0
# define WRITE 1

//Environment list struct
typedef struct s_env
{
	char			*key;
	char			*val;
	char			**tokens;
	char			**tokens2;
	struct s_env	*next;
}					t_env;

typedef enum e_type
{
	NULL_TYPE = 0,//para saber que se ha inicializado ya 
	WORD,
	PIPE,
	SMALLER,
	D_SMALLER,
	GREATER,
	D_GREATER
}	t_type;

// ANADIDO  PARA EL TRADUCTOR 
typedef enum e_quote_state
{
    Q_NONE,         // Sin comillas
    Q_SINGLE,       // Entrecomillado simple: '...'
    Q_DOUBLE        // Entrecomillado doble: "..."
} t_quote_state;


/*------contiene la str y el tipo de cada token-----*/
typedef struct s_tok
{
	char			*str;//contendrá string solo si es WORD, sino será NULLL.
	t_type			type;
	t_quote_state   quote_type; // NUEVO CAMPO! Indica el tipo de comillas originales.
	struct s_tok	*next;
}					t_tok;

typedef enum e_redir_type
{
	NULL_REDIR = 0,
	REDIR_INPUT = 1,//<
	HEREDOC_INPUT = 2,//<<
	REDIR_OUTPUT = 3,//>
	REDIR_OUTPUT_APPEND = 4,//>> 
}	t_redir_type;

/*----contiene los datos de cada redirección------*/
typedef struct s_redir
{
	char			*fname; // nombre de archivo o DELIMITADOR de heredoc
	t_redir_type	redir_type;
	struct s_redir	*next;
	bool            original_delimiter_had_quotes; // NUEVO CAMPO SUGERIDO para HEREDOC
                                                  // Necesario para flperez_core->io->heredoc_quotes
                                                  // Campo para almacenar si el delimitador original tenía comillas
}					t_redir;

/*------contiene los datos de cada pipe de la linea de comando es la salida de parser-*/
typedef struct s_cmd
{
	char			**commands;	// Comando y argumenos
	t_redir			*redir;		//lista de redirecciones
	struct s_cmd	*next;		//Siguiente comando en un pipeline
}					t_cmd;

// === PROBABLEMENTE OBSOLETAS O USO REDUCIDO SE COMENTAN, LOGICA DE FLPEREZ ===
/*---variables para expander---*/
typedef struct s_xpdr
{
	size_t	i;//iteradores
	size_t	j;
	size_t	k;
	size_t	len;
	bool	s_quote;//estado de las comillas
	bool	d_quote;
	char	*key;
	char	*val;
	char	*result;//token final expandido
}			t_xpdr;
/*
------variables para executor-------
typedef struct s_exe
{
	char			**paths;
	char			*cmd_fullpath;
	char			**new_array;
	pid_t			*pid;
	int				num_cmds;
	int				fd_input;
	int				fd_output;
	int				fd[2];
	int				dup_stdin;
	int				dup_stdout;
}					t_exe;
*/

/* ===== PROTOTIPOS DE FUNCIONES ===== */

/*--------------------------- minishell.c (Lógica principal de tu shell) -------*/
// (Estos prototipos se mantienen, asumiendo que minishell() llamará
//  a la secuencia: tokenizer -> parser -> capa_traduccion -> executor_flperez)

/*--------------------------- minishell.c ------ TODAS-------------------*/
int		set_signals(int mode);
void	minishell(t_env	*envlist);//Adaptar para e lnuevo flujp
void	tokenizer(t_tok **tok, char *line);
int		parser(t_cmd **cmd, t_tok *tok);
void	cleaner_envlist(t_env **lst);
void	control_and_d(char *line);

/*--------------------------- init_exe.c OBSOLETO-------------------------
//void	init_exe(t_exe *exe, t_cmd *cmd);	reemplazada por t_data_env_exe de flperez
void	exe_free(t_exe *exe);
*/

/*--------------------------- wellcome_msg.c -----------TODAS--------------*/
int		bg_color(void);
void	init_msg(void);
int		help_mini(void);

/*---------------------------array 2d -------- TODAS-> char ** ----------------*/
size_t	size_arr2d(char **arr2d);
char	**dup_arr2d(char **arr2d);
char	**add_one_arr2d(char **arr2d, char *new);
char	**rm_one_arr2d(char **arr2d, int index);
void	free_arr2d(char **arr2d);
void	print_arr2d(char **arr2d);//ELIMINAR ANTES DE ENTREGA

/*-------------------t_env-------- TODAS-> s_env ----------*/
t_env	*lstlast(t_env *lst);//Se puede cambiar por ft_lstlast de libft
void	lstadd_back(t_env **lst, t_env *new);//Se puede cambiar por ft_lstadd_back de libft
t_env	*lstnew(char *key, char *value);
void	env_init_list(char **envp, t_env **env);
void	env_delone(t_env **env, char **node_to_del, void (*del)(void*));//Se puede cambiar por ft_lstdelone de libft
//void	cleaner_envlist(t_env **lst); repetida en minishell.c

/*--------------------t_tok -------- TODAS-> s_tok ---------*/
t_tok	*tok_new_node(char *str, int type);
t_tok	*tok_last(t_tok *lst);
void	tok_add_back(t_tok **lst, t_tok *new);
void	tok_free(t_tok **lst);
int		tok_size(t_tok *lst);//ELIMINAR ANTES DE ENTREGA

/*--------------------t_cmd--------TODAS-> s_cmd generada del parser-----------*/
t_cmd	*cmd_new_node(void);
t_cmd	*cmd_last(t_cmd *lst);
void	cmd_add_back(t_cmd **lst, t_cmd *new);
void	cmd_free(t_cmd **lst);// IMPORTANTE para liberar la salida del parser
int		cmd_size(t_cmd *lst);//ELIMINAR ANTES DE ENTREGA

/*--------------------t_redir-------TODAS-> s_redir -----------*/
//t_redir	*redir_new_node(char *str, int redir_type);Adaptada
t_redir *redir_new_node(char *str, int redir_type, bool had_quotes);
t_redir	*redir_last(t_redir *lst);
void	redir_add_back(t_redir **lst, t_redir *new);
void	redir_free(t_redir **lst);
int		redir_size(t_redir *lst);//ELIMINAR ANTES DE ENTREGA

/*-------------------tokenizer-------TODAS -----------*/
int		init_operator_type(char *line, t_tok **new_tok);
int		tok_len(char *line, t_tok **new_tok);
void	init_word_str(size_t len, t_tok *new_tok, char *line, size_t i);

/*---------------------parser-------TODAS ES EL NUCLEO ------------*/
int		is_operator(t_tok *node);
int		is_redirection(t_tok *node);
void	handle_error(char *str, t_tok **tok);// del parser
size_t	commands_counter(t_tok *tok);
int		syntax_check_1(t_tok *tok);
int		syntax_check_2(t_tok *tok);
void	commands_creator(t_tok *tok, t_cmd *node);
void	commands_filler(t_tok **tok, t_cmd *node);

/*------------expander & quote removal----------*/
/* La expansión de argumentos de comando la hará flperez_core. Evalúa si los necesitas */
/* para expansiones tempranas en tu parser (ej. nombres de archivo de redirección).   */

void	should_expand(t_cmd *cmd, t_env *envlist);//comprobar si es rendundante
char	*expander(char *str, t_env *envlist);//comprobar si es redundante
void	init_xpdr(t_xpdr *xpdr);//comprobar si es rendundante
size_t	new_tok_len(char *str, t_xpdr *xpdr, t_env *envlist);//comprobar si es rendundante
void	init_xpdr_except_result(t_xpdr *xpdr);
void	get_dollar_len(char *str, t_xpdr *xpdr, t_env *envlist);
char	*get_env_key(char *str);
char	*get_env_val(char *env_key, t_env *envlist);
void	handle_quote_len(char c, t_xpdr *xpdr);
void	handle_quote_builder(char c, t_xpdr *xpdr);
void	handle_quote_after_dollar(char c, t_xpdr *xpdr);
size_t	get_len_and_free(char *str);
void	*new_tok_builder(char *str, t_xpdr *xpdr, t_env *envlist);
void	get_dollar_builder(char *str, t_xpdr *xpdr, t_env *envlist);
void	handle_dollar_question(t_xpdr *xpdr);
void	handle_dollar_invalid_syntax(char *str, t_xpdr *xpdr);

/*--------------------------utils t_env-------------------*/
/* las comentadas estan repetidas y ver si se puede usar las de libft */

//t_env	*lstlast(t_env *lst);
//void	lstadd_back(t_env **lst, t_env *new);
//t_env	*lstnew(char *key, char *value);
//void	env_init_list(char **envp, t_env **env);
//void	env_delone(t_env **env, char **node_to_del, void (*del)(void*));
//void	cleaner_envlist(t_env **lst);
//int		no_path_env(t_cmd *cmd, t_exe exe, t_env *env);
//int		no_path_env(t_cmd *cmd, t_env *env);// t_exe ya no existe


/*---------------------------executor.c -------------------------*/
//char	**get_paths(t_env *env);reemplazada por find_pah
//int		pre_executor(t_env **env, t_cmd *cmd, t_exe *exe, int size_pipe);
//int		search_command_path(t_cmd *cmd, t_exe *exe);reemplazado por find_path
//void	error_exe(int num); no se usara, el manejo de errores se realizara con execute de flperez
//int		list_to_array(t_env *env, t_exe *exe);USARE EL CONVERSOR para transformar el t_env del antiguo t_exe en t_data_env
//int		close_fd(t_exe	*exe); El manejo de descriptores lo realizara flperez
//int		executor(t_cmd *cmd, t_exe	*exe, t_env **env); FUNCIO PRINCIPAL, reempazada por exec_pipeline.c

/*---------------------------redirections.c -------------------------*/
//int		pre_redirections(t_cmd *cmd, t_exe *exe); reemplaza logica de flperez

/*---------------------------utils0.c --------EL VALOR LO ESTABLECE FLPEREZ-----------------*/
int		ft_msgs(int n, t_cmd *cmd);
void	set_exit_status(int n);

/*---------------------utils & utils1.c-------------------*/
int		ca_strchr(const char *s, int c);
char	*ft_strncpy(char *dest, char *src, unsigned int n);
void	*p_malloc(size_t size);
void	str_free_and_null(char **str);
void	free_ptr(void *ptr);
//void	free_data_env(t_data_env *data);
char	*ft_strjoin_free(char *s1, char const *s2);
void	safe_close(int *fd);
void	perror_exit(const char *context, int g_get_signal);


void	signals_noninteractive(void);
void	signals_interactive(void);



//void	signal_child(int sig);//coordinarla con flperez
//void	signal_parent(int sig);//Coordinala con flperez-


/*-------------------exit_status------Manejamos msg_error y $?------------*/
int		get_exit_status_len(void);
char	*get_exit_status_val(void);
// (Estas funciones de error podrían necesitar ser adaptadas o unificadas con las de flperez)
void	command_not_found(t_cmd *cmd, const char *prefix, size_t prefix_len);
void	no_file_or_dir(t_cmd *cmd, const char *prefix, size_t prefix_len);

/*--------------------------- builtins -------------------------*/
int		builtins(t_cmd *cmd, t_env **env);
int		builtin_exit(t_cmd *cmd, t_env **envlist);
int		builtin_pwd(t_env *env);
int		builtin_cd(t_cmd *cmd, t_env **env_list);
int     builtin_env(t_cmd *cmd, t_env *env);
int		builtin_echo(t_cmd *cmd, t_env *env); // Añadido t_env *env
int		builtin_export(t_cmd *cmd, t_env **env);
int		builtin_unset(t_cmd *cmd, t_env **env);
int		is_builtins(t_cmd *cmd);
int		exist_cwd(void);

/*--------------------------- utils builtins ---------------*/

char	*util_cd_get_env_val(t_env *env_list, const char *var_name);
void	util_cd_set_env_val(t_env **env_list, const char *var_name,
	const char *value);
bool	util_cd_capture_old_work_dir(char *buffer, size_t buf_size,
	t_env *env_list);
long long	ft_atoll_for_exit(const char *str, bool *error);



/*--------------------------- builtin export -------------------------*/


int		variable_exists_op3(t_env *env, char *variable);
t_env	*variable_exists_op4(t_env *env, const char *key);
t_env	*update_env(t_env *env, char *key, char *val);


/*-----------------------oldpwd---------------------*/
/* 			CODIGO MUERTO 

char	*update_pwd(t_env *env);
int		set_old_pwd(void);
int		get_old_pwd(char *current_wd, t_env *env);
int		go_path(t_cmd *cmd);
int		update_oldpwd(t_env *env, char *current_wd);
*/
/*----------------------prints--------------------*/
void	print_arr2d(char **arr2d);//ELIMINAR ANTES DE ENTREGA
void	print_tok(t_tok *lst);//ELIMINAR ANTES DE ENTREGA
void	print_cmd(t_cmd *list);//ELIMINAR ANTES DE ENTREGA
void	print_redir(t_redir *lst);//ELIMINAR ANTES DE ENTREGA
void	ft_print_keys(t_env *env_struct);//ELIMINAR ANTES DE ENTREGA
void	ft_print_values(t_env *env_struct);//ELIMINAR ANTES DE ENTREGA
void	ft_printstack(t_env *env_struct);//ELIMINAR ANTES DE ENTREGA
void	print_cmd_para_executor(t_cmd *lst);//ELIMINAR ANTES DE ENTREGA

/*------------------redirections---------------*/
int		exist_redirections(t_cmd *cmd);// Puede ser útil para el parser/traductor


/*--- Manejo de Errores (Se mantiene, unificar con flperez_core si es posible) ---*/
int     msg_error_cmd(char *arg_cmd, char *descrip, char *err_msg, int nb_err);

/*-------------------heredoc (ROL CAMBIA SIGNIFICATIVAMENTE) -----------------*/
// Si flperez_core maneja la creación del archivo temporal del heredoc usando el delimitador:
// - `heredoc_create` de bonus ya no crearía el archivo para `dup2`.
//   Su rol se transformaría en:
//   1. Leer el delimitador.
//   2. Determinar si el delimitador original tenía comillas (para `heredoc_quotes`).
//   3. Quizás realizar la entrada del usuario para validación o si hay que hacer algo
//      con ella ANTES de que flperez la lea (poco probable si flperez la expande).
// - `heredoc` de bonus, si orquestaba lo anterior para el antiguo executor, también cambia.
//int     heredoc_create(t_redir *redir_bonus /* que contiene el delimitador */,
//                        int hd_nbr /*, bool *out_had_quotes - SUGERIDO */);
//int     heredoc(t_cmd *cmd_bonus); // Probablemente reorganizar esta lógica


/*-------------------heredoc-------------------*/
int		heredoc_create(t_redir *redir, int hd_nbr);
int		heredoc(t_cmd *cmd);

#endif