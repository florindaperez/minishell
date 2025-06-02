/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 09:41:30 by flperez-          #+#    #+#             */
/*   Updated: 2025/03/15 14:07:52 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"
#include "minishell_executor.h"

/*
* static int cd_to_home(t_data_env_exe *data)
* -------------------------------------------
* Cambia el directorio actual al valor de la variable de entorno HOME.
* Si HOME no está definida, está vacía o solo contiene espacios,
* muestra un error y establece el estado de salida a EXIT_FAILURE.
* Utiliza perform_directory_change para realizar el cambio y actualizar
* las variables de entorno PWD y OLDPWD.
*
* data: Puntero a la estructura que contiene el entorno y el estado de salida.
*
* Retorna: EXIT_SUCCESS si el cambio es exitoso, EXIT_FAILURE en caso contrario.
* El estado de salida global (data->last_exit_status) es actualizado por
* esta función o por perform_directory_change.
*/
static int	cd_to_home(t_data_env_exe *data)
{
	char	*path_home;

	path_home = get_env_var_val(data->env_for_execve, "HOME");
	if (!path_home || *path_home == '\0' || ft_isspace(*path_home))
	{
		msg_error_cmd("cd", NULL, "HOME not set", EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	if (!perform_directory_change(data, path_home))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

/*
* static int cd_to_oldpwd(t_data_env_exe *data)
* ---------------------------------------------
* Cambia el directorio actual al valor de la variable de entorno OLDPWD.
* Si OLDPWD no está definida, muestra un error y establece el estado de salida
* a EXIT_FAILURE. Si tiene éxito, imprime la ruta de OLDPWD a la salida
* estándar antes de cambiar. Utiliza perform_directory_change para el cambio.
*
* data: Puntero a la estructura que contiene el entorno y el estado de salida.
*
* Retorna: EXIT_SUCCESS si el cambio es exitoso, EXIT_FAILURE en caso contrario.
* El estado de salida global (data->last_exit_status) es actualizado.
*/
static int	cd_to_oldpwd(t_data_env_exe *data)
{
	char	*path_oldpwd;

	path_oldpwd = get_env_var_val(data->env_for_execve, "OLDPWD");
	if (!path_oldpwd)
	{
		msg_error_cmd("cd", NULL, "OLDPWD not set", EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	ft_putendl_fd(path_oldpwd, STDOUT_FILENO);
	if (!perform_directory_change(data, path_oldpwd))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

/*
* static int cd_handle_path_arg(char *path_arg, t_data_env_exe *data)
* --------------------------------------------------------------------
* Maneja el cambio de directorio cuando se proporciona un argumento de ruta.
* Si el argumento es ".", simplemente establece el estado a EXIT_SUCCESS
* (ya que no hay cambio real de directorio necesario, pero PWD podría
* necesitar actualizarse si fue modificado externamente, lo cual
* perform_directory_change con "." podría manejar si esa lógica existe).
* Para otras rutas, utiliza perform_directory_change.
*
* path_arg: La ruta a la que se intentará cambiar.
* data: Puntero a la estructura que contiene el entorno y el estado de salida.
*
* Retorna: EXIT_SUCCESS si el cambio es exitoso o la ruta es ".",
* EXIT_FAILURE en caso contrario. El estado de salida global es actualizado.
*/
static int	cd_handle_path_arg(char *path_arg, t_data_env_exe *data)
{
	if (!perform_directory_change(data, path_arg))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

/*
* int builtin_cd(char **args, t_data_env_exe *data)
* -------------------------------------------------
* Implementa el comando `cd` (cambiar directorio) para minishell.
*
* Comportamiento:
* - Sin argumentos, o con "~", o con "--": Cambia al directorio HOME.
* Si HOME no está definido, se produce un error.
* - Con "-": Cambia al directorio almacenado en OLDPWD. Si OLDPWD
* no está definido, se produce un error. La nueva ruta se imprime.
* - Con una ruta específica: Intenta cambiar al directorio especificado.
* La ruta "." actualiza PWD/OLDPWD si es necesario.
* - Con demasiados argumentos: Muestra un error.
*
* La función actualiza las variables de entorno PWD y OLDPWD a través de
* la función auxiliar 'perform_directory_change' y establece el estado de
* salida global 'data->last_exit_status'.
*
* args: Array de cadenas que representan los argumentos del comando.
* args[0] es "cd", args[1] es el primer argumento, etc.
* data: Puntero a la estructura t_data_env_exe que contiene las variables
* de entorno y el último estado de salida.
*
* Retorna: EXIT_SUCCESS (0) si el comando se ejecuta correctamente, o
* EXIT_FAILURE (1) si ocurre algún error. El estado de salida
* también se almacena en data->last_exit_status.
*/
int	builtin_cd(char **args, t_data_env_exe *data)
{
	if (!args[1] || ft_isspace(args[1][0]) \
		|| args[1][0] == '\0' || ft_strncmp(args[1], "--", 3) == 0)
	{
		return (cd_to_home(data));
	}
	if (args[2])
	{
		msg_error_cmd("cd", NULL, "too many arguments", EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	if (ft_strcmp(args[1], "-") == 0)
	{
		return (cd_to_oldpwd(data));
	}
	return (cd_handle_path_arg(args[1], data));
}
