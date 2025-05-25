/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: castorga <castorga@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 15:51:57 by castorga          #+#    #+#             */
/*   Updated: 2024/05/27 15:52:00 by castorga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
// --- Funciones auxiliares estáticas adaptadas ---

// Función para mostrar errores de 'cd' de forma consistente
static void cd_error_jocuni(const char *context, const char *arg, const char *message, int status_code) {
    ft_putstr_fd("minishell: cd", STDERR_FILENO);
    if (context && *context) {
        ft_putstr_fd(": ", STDERR_FILENO);
        ft_putstr_fd((char *)context, STDERR_FILENO);
    }
    if (arg && *arg) {
        ft_putstr_fd(": ", STDERR_FILENO);
        ft_putstr_fd((char *)arg, STDERR_FILENO);
    }
    if ((context && *context) || (arg && *arg)) {
         ft_putstr_fd(": ", STDERR_FILENO);
    } else {
         ft_putstr_fd(": ", STDERR_FILENO);
    }
    ft_putendl_fd((char *)message, STDERR_FILENO);
    set_exit_status(status_code); // set_exit_status es de jocuni
}

// Adaptación de perform_directory_change de flperez
// Esta función ahora es responsable de cambiar el directorio y actualizar PWD y OLDPWD.
static bool perform_directory_change_jocuni(t_env **env, const char *path) {
    char previous_pwd_buffer[PATH_MAX]; // Para guardar el PWD actual antes de chdir
    bool previous_pwd_set = false;
    t_env *pwd_node_current;

    // 1. Intentar obtener el PWD actual para guardarlo como OLDPWD
    // Primero, intentar desde la variable de entorno PWD si existe.
    pwd_node_current = variable_exists_op4(*env, "PWD");
    if (pwd_node_current && pwd_node_current->val) {
        ft_strlcpy(previous_pwd_buffer, pwd_node_current->val, PATH_MAX);
        previous_pwd_set = true;
    } else {
        // Si PWD no está en el env, intentar con getcwd()
        if (getcwd(previous_pwd_buffer, sizeof(previous_pwd_buffer)) != NULL) {
            previous_pwd_set = true;
        }
        // Si getcwd también falla, OLDPWD podría no establecerse o heredar un valor antiguo.
    }

    // 2. Cambiar directorio
	fflush(stdout); // Para asegurar que se imprima inmediatamente
    if (chdir(path) != 0) {
        cd_error_jocuni(NULL, path, strerror(errno), EXIT_FAILURE);
        return false;
    }

    // 3. Actualizar OLDPWD si teníamos un PWD anterior válido
    if (previous_pwd_set) {
        *env = update_env(*env, "OLDPWD", previous_pwd_buffer);
         // Se asume que update_env maneja la duplicación de previous_pwd_buffer
    }

    // 4. Obtener el nuevo directorio de trabajo actual y actualizar PWD
    char new_cwd_buffer[PATH_MAX];
    if (getcwd(new_cwd_buffer, sizeof(new_cwd_buffer)) != NULL) {
        *env = update_env(*env, "PWD", new_cwd_buffer);
    } else {
        // Error al obtener el nuevo CWD, PWD podría no actualizarse o
        // podría necesitar una lógica más compleja para rutas relativas.
        // Bash a veces actualiza PWD con la ruta que se le pasó a cd si getcwd falla.
        // Por simplicidad aquí, solo reportamos el error de getcwd.
        cd_error_jocuni("error retrieving current directory after chdir", "getcwd", strerror(errno), EXIT_FAILURE);
        // No retornamos false necesariamente, ya que chdir tuvo éxito.
        // PWD podría quedar desactualizado o con el valor antiguo.
        // Para un comportamiento más robusto, se podría intentar actualizar PWD con 'path' si es absoluto,
        // o resolverlo si es relativo.
    }

    set_exit_status(EXIT_SUCCESS);
    return true;
}

// Adaptación de cd_to_home de flperez
static int cd_to_home_jocuni(t_env **env) {
    const char *path_home = getenv("HOME");

    if (!path_home || *path_home == '\0') {
        cd_error_jocuni(NULL, NULL, "HOME not set", EXIT_FAILURE);
        return EXIT_FAILURE;
    }
    
    const char *p = path_home;
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p == '\0') {
        cd_error_jocuni(NULL, NULL, "HOME not set (only spaces)", EXIT_FAILURE);
        return EXIT_FAILURE;
    }

    return perform_directory_change_jocuni(env, path_home) ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Adaptación de cd_to_oldpwd de flperez
static int cd_to_oldpwd_jocuni(t_env **env) {
    char *path_oldpwd_val = NULL;
    t_env *oldpwd_node = variable_exists_op4(*env, "OLDPWD"); // de jocuni

    if (oldpwd_node && oldpwd_node->val) {
        path_oldpwd_val = oldpwd_node->val;
    }

    if (!path_oldpwd_val || *path_oldpwd_val == '\0') {
        cd_error_jocuni(NULL, NULL, "OLDPWD not set", EXIT_FAILURE);
        return EXIT_FAILURE;
    }

    ft_putendl_fd(path_oldpwd_val, STDOUT_FILENO); // Imprime la nueva ruta
    return perform_directory_change_jocuni(env, path_oldpwd_val) ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Adaptación de cd_handle_path_arg de flperez
static int cd_handle_path_arg_jocuni(const char *path_arg, t_env **env) {
    // El caso 'cd .' es manejado aquí. perform_directory_change_jocuni
    // se encargará de llamar a getcwd para obtener la ruta canónica.
    if (!path_arg || *path_arg == '\0') { // Manejar `cd ""` o argumento vacío explícito
        cd_error_jocuni(NULL, path_arg, "No such file or directory", EXIT_FAILURE); // Bash-like
        return EXIT_FAILURE;
    }
    return perform_directory_change_jocuni(env, path_arg) ? EXIT_SUCCESS : EXIT_FAILURE;
}

// --- Función principal builtin_cd ---
// La firma en minishell_jocuni es: int builtin_cd(t_cmd *cmd, t_env **env)
int builtin_cd(t_cmd *cmd, t_env **env) { // Nombre del parámetro cambiado a **env
    char **args = cmd->commands; // En jocuni, los argumentos están en cmd->commands

    // args[0] es "cd". args[1] es el primer argumento real.
    // size_arr2d es de jocuni.

    if (size_arr2d(args) > 2) {
        cd_error_jocuni(NULL, NULL, "too many arguments", EXIT_FAILURE);
        return EXIT_FAILURE; // Retorna el código de estado de error
    }

    // Caso: `cd` (sin argumentos) o `cd --`
    // El caso `cd ""` es manejado por cd_handle_path_arg_jocuni si llega allí.
    if (!args[1] || (args[1] && ft_strcmp(args[1], "--") == 0)) {
        return cd_to_home_jocuni(env);
    }
    
    // Caso: `cd ~` (si ~ no fue expandido por el parser previamente)
    // o `cd ~/algun/path` (si ~ no fue expandido)
    // Esta implementación asume que `~` como argumento único va a HOME.
    // La expansión de `~` en medio de una ruta (e.g., `~/docs`)
    // debería ser manejada por una fase de expansión de tildes antes del builtin.
    // Si `~` llega como `args[1]`, lo tratamos como ir a HOME.
    if (args[1] && ft_strcmp(args[1], "~") == 0) {
        return cd_to_home_jocuni(env);
    }

    // Caso: `cd -`
    if (ft_strcmp(args[1], "-") == 0) {
        return cd_to_oldpwd_jocuni(env);
    }

    // Caso: `cd <path_arg>` (incluye `cd .`, `cd ..`, `cd ""`, etc.)
    return cd_handle_path_arg_jocuni(args[1], env);
}

/*Change to the user's home directory
int	go_home(void)
{
	char	*home_dir;

	home_dir = getenv("HOME");
	if (home_dir == NULL)
	{
		perror("getenv() error");
		return (1);
	}
	if (chdir(home_dir) != 0)
	{
		perror("chdir() error");
		return (1);
	}
	set_exit_status(0);
	return (0);
}

Get information about a file
int	get_info_file(t_cmd *cmd, struct stat *info_f)
{
	if (stat(cmd->commands[1], info_f) == -1)
	{
		ft_msgs(4, cmd);
		return (1);
	}
	if (!S_ISDIR(info_f->st_mode) || \
		!(info_f->st_mode & S_IRUSR) || !(info_f->st_mode & S_IXUSR))
	{
		ft_msgs(7, cmd);
		return (1);
	}
	return (0);
}

 Change to a specific directory 
int	go_path(t_cmd *cmd)
{
	struct stat	info_f;

	if (!exist_cwd())
		return (1);
	else
	{
		if (cmd->commands[1] == NULL)
			return (1);
		get_info_file(cmd, &info_f);
		if (chdir(cmd->commands[1]) == -1)
		{
			return (1);
		}
	}
	return (0);
}

Change to a specific directory
static int	change_directory(t_cmd *cmd, char **current_wd)
{
	*current_wd = getcwd(NULL, 0);
	if (*current_wd == NULL)
	{
		perror("getcwd() error");
		return (1);
	}
	if (go_path(cmd) != 0)
	{
		free(*current_wd);
		*current_wd = NULL;
		return (1);
	}
	return (0);
}

Change to a directory accordingly the parameter if applicable.
int	builtin_cd(t_cmd *cmd, t_env **env)
{
	char	*current_wd;

	current_wd = NULL;
	if ((size_arr2d(cmd->commands)) > 2)
	{
		ft_msgs(9, cmd);
		return (1);
	}
	if (handle_no_argument(cmd) == 1)
		return (1);
	if (handle_tilde(cmd) == 1)
		return (1);
	if (handle_dash(cmd) == 1)
		return (1);
	if (handle_dot(cmd) == 1)
		return (0);
	if (handle_invalid_path(cmd) == 1)
		return (1);
	if (change_directory(cmd, &current_wd) != 0)
		return (1);
	update_environment(*env, current_wd);
	return (free_current_wd(current_wd));
}
*/