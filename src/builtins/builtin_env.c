/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 12:50:19 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/23 12:51:19 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* builtin_env: (Anteriormente builtin_env_)
* Imprime las variables de entorno de la lista enlazada t_env.
* Si se proporcionan argumentos, muestra un error similar al de bash.
*/
int builtin_env(t_cmd *cmd, t_env *envlist)
{
    if (cmd->commands[1] != NULL)
    {
        ft_putstr_fd("minishell: env: ‘", STDERR_FILENO);
        ft_putstr_fd(cmd->commands[1], STDERR_FILENO);
        ft_putstr_fd("’: No such file or directory\n", STDERR_FILENO);
        set_exit_status(127);
        return (127);
    }

    t_env *current = envlist;
    while (current != NULL)
    {
        if (current->key) 
        {
            ft_putstr_fd(current->key, STDOUT_FILENO);
            ft_putchar_fd('=', STDOUT_FILENO);
            if (current->val)
            {
                ft_putstr_fd(current->val, STDOUT_FILENO);
            }
            ft_putchar_fd('\n', STDOUT_FILENO);
        }
        current = current->next;
    }

    set_exit_status(0);
    return (0);
}

/*
* no_path_env:
* Verifica si la variable de entorno PATH está definida en envlist.
* Es llamada por pre_executor.
*
* Parámetros:
* cmd: Estructura del comando actual (para ft_msgs).
* exe: Estructura de ejecución (no utilizada aquí).
* envlist: Puntero a la cabeza de la lista de entorno.
*
* Retorna:
* 0 si PATH existe.
* 1 si PATH no existe (e imprime un mensaje de error).
*/
int no_path_env(t_cmd *cmd, t_exe exe, t_env *envlist)
{
    (void)exe; // Marcar exe como no utilizado

    // variable_exists_op3 busca si la clave "PATH" existe en la lista envlist.
    // Asumimos que variable_exists_op3 está correctamente definida en 
    // src/builtins/utils_export/var_exists.c y declarada en minishell.h
    if (!variable_exists_op3(envlist, "PATH"))
    {
        // ft_msgs(4, cmd) en jocuni original imprime "No such file or directory".
        // Aunque "PATH not set" sería más específico, mantenemos la consistencia.
        ft_msgs(4, cmd); 
        return (1); // Indica que PATH no está seteado
    }
    return (0); // PATH está seteado
}


/*check if the PATH variable exists
int	no_path_env(t_cmd *cmd, t_exe exe, t_env *env)
{
	int		flag;

	(void)exe;
	flag = 0;
	while (env)
	{
		if (variable_exists_op3(env, "PATH"))
			flag = 1;
		env = env->next;
	}
	if (flag == 0)
	{
		ft_msgs(4, cmd);
		return (1);
	}
	return (0);
}

void	print_env(t_env *env)
{
	while (env)
	{
		if (!(ft_strchr(env->val, 32)) || (ft_strcmp(env->key, "PATH") == 0))
			printf("%s=%s\n", env->key, env->val);
		env = env->next;
	}
}

builtin that handle the environment
int	builtin_env(t_cmd *cmd, t_exe exe, t_env *env)
{
	if (size_arr2d(cmd->commands) > 1)
	{
		if ((ft_strcmp(cmd->commands[1], "test") == 0) || \
		(ft_strcmp(cmd->commands[1], "true") == 0) || \
		(ft_strcmp(cmd->commands[1], "false") == 0))
			return (1);
		else if (ft_strcmp(cmd->commands[1], "echo") == 0)
		{
			printf("\n");
			return (0);
		}
		else
			ft_msgs(4, cmd);
		return (1);
	}
	else
	{
		if (no_path_env(cmd, exe, env))
			return (1);
		print_env(env);
	}
	set_exit_status(0);
	return (0);
}
*/
