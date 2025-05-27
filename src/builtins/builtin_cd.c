/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 11:06:56 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/26 11:07:06 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
*	execute_directory_change:
*	Función central que intenta cambiar el directorio actual al 'target_path'.
*	Antes de cambiar, captura el directorio de trabajo actual para guardarlo
*	como OLDPWD. Después del cambio, actualiza PWD con el nuevo directorio.
*	Maneja errores si chdir falla o si getcwd falla después del cambio.
*	Devuelve EXIT_SUCCESS o EXIT_FAILURE.
*/
static int	execute_directory_change(const char *target_path, \
										t_env **env_list)
{
	char	old_pwd_buffer[MINISHELL_PATH_BUF_SIZE];
	char	new_pwd_buffer[MINISHELL_PATH_BUF_SIZE];
	bool	old_pwd_captured;

	if (!target_path)
		return (EXIT_FAILURE);
	old_pwd_captured = util_cd_capture_old_work_dir(old_pwd_buffer, \
			sizeof(old_pwd_buffer), *env_list);
	if (chdir(target_path) != 0)
	{
		msg_error_cmd("cd", (char *)target_path, strerror(errno), EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	if (old_pwd_captured)
		util_cd_set_env_val(env_list, "OLDPWD", old_pwd_buffer);
	if (getcwd(new_pwd_buffer, sizeof(new_pwd_buffer)) != NULL)
		util_cd_set_env_val(env_list, "PWD", new_pwd_buffer);
	else
	{
		msg_error_cmd("cd", "getcwd after chdir", strerror(errno), 1);
		set_exit_status(EXIT_FAILURE);
	}
	set_exit_status(EXIT_SUCCESS);
	return (EXIT_SUCCESS);
}

/*
*	handle_cd_to_home:
*	Maneja los casos del comando 'cd' que resultan en un cambio al directorio
*	HOME del usuario. Esto incluye 'cd' sin argumentos, 'cd ~', o 'cd --'.
*	Obtiene la ruta HOME de las variables de entorno. Si HOME no está definida
*	o está vacía (o solo espacios), reporta un error.
*	Luego, llama a execute_directory_change con la ruta HOME.
*/
static int	handle_cd_to_home(t_env **env_list)
{
	char		*home_path;
	const char	*p_validator;

	home_path = util_cd_get_env_val(*env_list, "HOME");
	if (!home_path || *home_path == '\0')
	{
		msg_error_cmd("cd", NULL, "HOME not set", EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	p_validator = home_path;
	while (*p_validator && isspace((unsigned char)*p_validator))
		p_validator++;
	if (*p_validator == '\0')
	{
		msg_error_cmd("cd", NULL, "HOME value is empty or only spaces", \
			EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	return (execute_directory_change(home_path, env_list));
}

/*
*	handle_cd_to_oldpwd:
*	Maneja el caso del comando 'cd -', que cambia al directorio OLDPWD
*	(directorio de trabajo anterior).
*	Obtiene la ruta OLDPWD de las variables de entorno. Si OLDPWD no está
*	definida o está vacía, reporta un error.
*	Imprime la ruta OLDPWD en la salida estándar antes de cambiar.
*	Luego, llama a execute_directory_change con la ruta OLDPWD.
*/
static int	handle_cd_to_oldpwd(t_env **env_list)
{
	char	*old_pwd_path;

	old_pwd_path = util_cd_get_env_val(*env_list, "OLDPWD");
	if (!old_pwd_path || *old_pwd_path == '\0')
	{
		msg_error_cmd("cd", NULL, "OLDPWD not set", EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	ft_putendl_fd(old_pwd_path, STDOUT_FILENO);
	return (execute_directory_change(old_pwd_path, env_list));
}

/*
*	handle_cd_to_specific_path:
*	Maneja el caso del comando 'cd <path>', donde <path> es una ruta
*	específica proporcionada por el usuario.
*	Verifica si el path es nulo o una cadena vacía, en cuyo caso reporta
*	un error "No such file or directory".
*	De lo contrario, llama a execute_directory_change con la ruta dada.
*/
static int	handle_cd_to_specific_path(const char *path, t_env **env_list)
{
	if (!path || *path == '\0')
	{
		msg_error_cmd("cd", (char *)path, "No such file or directory", \
			EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	return (execute_directory_change(path, env_list));
}

/*
*	builtin_cd:
*	Función principal para el comando incorporado 'cd'.
*	Determina el comportamiento de 'cd' basado en el número y tipo de
*	argumentos proporcionados (ej. sin argumentos, '~', '--', '-', o una ruta).
*	Delega la acción a la función manejadora correspondiente.
*	Maneja el error de "too many arguments".
*/
int	builtin_cd(t_cmd *cmd, t_env **env_list)
{
	char	**args;
	int		arg_count;
	int		i;

	args = cmd->commands;
	arg_count = 0;
	i = 0;
	while (args[i])
	{
		arg_count++;
		i++;
	}
	if (arg_count > 2)
	{
		msg_error_cmd("cd", NULL, "too many arguments", EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	if (arg_count == 1 || (args[1] && ft_strcmp(args[1], "~") == 0) \
		|| (args[1] && ft_strcmp(args[1], "--") == 0))
		return (handle_cd_to_home(env_list));
	else if (args[1] && ft_strcmp(args[1], "-") == 0)
		return (handle_cd_to_oldpwd(env_list));
	else
		return (handle_cd_to_specific_path(args[1], env_list));
}
