/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 12:49:17 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/23 12:49:24 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"

/*according to the command received, redirects to its corresponding building.*/
int builtins(t_cmd *cmd, t_exe exe, t_env **env) // env es t_env **
{
    (void)exe;
	// Primero, una comprobación básica para evitar dereferenciar cmd->commands[0] si cmd o cmd->commands es NULL
    if (!cmd || !cmd->commands || !cmd->commands[0])
        return (0); // No hay comando para ejecutar o está mal formado

    if (ft_strcmp(cmd->commands[0], "exit") == 0)
    {
        builtin_exit(cmd); // Asumiendo que la firma de builtin_exit es compatible o ya fue adaptada
        exit (g_get_signal); // Usar la variable global de jocuni para el estado de salida
    }
    else if ((ft_strcmp(cmd->commands[0], "pwd") == 0) || \
        (ft_strcmp(cmd->commands[0], "PWD") == 0))
        builtin_pwd(*env); // builtin_pwd de jocuni espera t_env *
    else if (ft_strcmp(cmd->commands[0], "cd") == 0)
        builtin_cd(cmd, env); // builtin_cd de jocuni espera t_cmd* y t_env**
    else if ((ft_strcmp(cmd->commands[0], "env") == 0) || \
    (ft_strcmp(cmd->commands[0], "ENV") == 0))
        builtin_env(cmd, *env); // builtin_env de jocuni espera t_cmd*, t_exe, t_env*
    else if ((ft_strcmp(cmd->commands[0], "echo") == 0) || \
    (ft_strcmp(cmd->commands[0], "ECHO") == 0))
        builtin_echo(cmd, *env); // Adaptado al mio
    else if (ft_strcmp(cmd->commands[0], "export") == 0)
        builtin_export(cmd, env); // builtin_export de jocuni espera t_cmd* y t_env**
    else if (ft_strcmp(cmd->commands[0], "unset") == 0)
        builtin_unset(cmd, env); // builtin_unset de jocuni espera t_cmd* y t_env**
    else if (ft_strcmp(cmd->commands[0], "help") == 0)
        help_mini(); // Asumiendo que help_mini no necesita el entorno
    else
        return (0); // No es un builtin reconocido
    return (1); // Es un builtin y fue (intentado) ejecutar
}

/*check if is builtin(1) or not(0)*/
int is_builtins(t_cmd *cmd)
{
    // Primero, una comprobación básica
    if (!cmd || !cmd->commands || !cmd->commands[0])
        return (0);

    if (ft_strcmp(cmd->commands[0], "exit") == 0)
        return (1);
    else if ((ft_strcmp(cmd->commands[0], "pwd") == 0) || \
        (ft_strcmp(cmd->commands[0], "PWD") == 0))
        return (1);
    else if (ft_strcmp(cmd->commands[0], "cd") == 0)
        return (1);
    else if ((ft_strcmp(cmd->commands[0], "env") == 0) || \
    (ft_strcmp(cmd->commands[0], "ENV") == 0))
        return (1);
    else if ((ft_strcmp(cmd->commands[0], "echo") == 0) || \
    (ft_strcmp(cmd->commands[0], "ECHO") == 0))
        return (1);
    else if (ft_strcmp(cmd->commands[0], "export") == 0)
        return (1);
    else if (ft_strcmp(cmd->commands[0], "unset") == 0)
        return (1);
    else if (ft_strcmp(cmd->commands[0], "help") == 0)
        return (1);
    return (0);
}


/*according to the command received, redirects to its corresponding building.
int	builtins(t_cmd *cmd, t_exe exe, t_env **env)
{
	if (ft_strcmp(*cmd->commands, "exit") == 0)
	{
		builtin_exit(cmd);
		exit (0);
	}
	else if ((ft_strcmp(*cmd->commands, "pwd") == 0) || \
		(ft_strcmp(*cmd->commands, "PWD") == 0))
		builtin_pwd(*env);
	else if (ft_strcmp(*cmd->commands, "cd") == 0)
		builtin_cd(cmd, env);
	else if ((ft_strcmp(*cmd->commands, "env") == 0) || \
	(ft_strcmp(*cmd->commands, "ENV") == 0))
		builtin_env(cmd, exe, *env);
	else if ((ft_strcmp(*cmd->commands, "echo") == 0) || \
	(ft_strcmp(*cmd->commands, "ECHO") == 0))
		builtin_echo(cmd);
	else if (ft_strcmp(*cmd->commands, "export") == 0)
		builtin_export(cmd, env);
	else if (ft_strcmp(*cmd->commands, "unset") == 0)
		builtin_unset(cmd, env);
	else if (ft_strcmp(*cmd->commands, "help") == 0)
		help_mini();
	else
		return (0);
	return (1);
}

check if is builtin(1) or not(0)
int	is_builtins(t_cmd *cmd)
{
	if (ft_strcmp(*cmd->commands, "exit") == 0)
		return (1);
	else if ((ft_strcmp(*cmd->commands, "pwd") == 0) || \
		(ft_strcmp(*cmd->commands, "PWD") == 0))
		return (1);
	else if (ft_strcmp(*cmd->commands, "cd") == 0)
		return (1);
	else if ((ft_strcmp(*cmd->commands, "env") == 0) || \
	(ft_strcmp(*cmd->commands, "ENV") == 0))
		return (1);
	else if ((ft_strcmp(*cmd->commands, "echo") == 0) || \
	(ft_strcmp(*cmd->commands, "ECHO") == 0))
		return (1);
	else if (ft_strcmp(*cmd->commands, "export") == 0)
		return (1);
	else if (ft_strcmp(*cmd->commands, "unset") == 0)
		return (1);
	else if (ft_strcmp(*cmd->commands, "help") == 0)
		return (1);
	return (0);
}
*/
