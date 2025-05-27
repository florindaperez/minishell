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

/*
 * Redirige al builtin correspondiente según el comando recibido.
 * Retorna 1 si se identificó y se intentó ejecutar un builtin.
 * Retorna 0 si el comando no es un builtin reconocido.
 * Cada función builtin específica es responsable de establecer el estado
 * de salida ($?) usando set_exit_status().
 */
int	builtins(t_cmd *cmd, t_env **env)
{
	if (!cmd || !cmd->commands || !cmd->commands[0])
		return (0);
	if (ft_strcmp(cmd->commands[0], "exit") == 0)
		builtin_exit(cmd, env);
	else if (ft_strcmp(cmd->commands[0], "pwd") == 0)
		builtin_pwd(*env);
	else if (ft_strcmp(cmd->commands[0], "cd") == 0)
		builtin_cd(cmd, env);
	else if (ft_strcmp(cmd->commands[0], "env") == 0)
		builtin_env(cmd, *env);
	else if (ft_strcmp(cmd->commands[0], "echo") == 0)
		builtin_echo(cmd, *env);
	else if (ft_strcmp(cmd->commands[0], "export") == 0)
		builtin_export(cmd, env);
	else if (ft_strcmp(cmd->commands[0], "unset") == 0)
		builtin_unset(cmd, env);
	else if (ft_strcmp(cmd->commands[0], "help") == 0)
		help_mini();
	else
		return (0);
	return (1);
}

/*
 * Comprueba si el comando dado es un builtin.
 * Retorna 1 si es un builtin, 0 en caso contrario.
 */
int	is_builtins(t_cmd *cmd)
{
	if (!cmd || !cmd->commands || !cmd->commands[0])
		return (0);
	if (ft_strcmp(cmd->commands[0], "exit") == 0
		|| ft_strcmp(cmd->commands[0], "pwd") == 0
		|| ft_strcmp(cmd->commands[0], "cd") == 0
		|| ft_strcmp(cmd->commands[0], "env") == 0
		|| ft_strcmp(cmd->commands[0], "echo") == 0
		|| ft_strcmp(cmd->commands[0], "export") == 0
		|| ft_strcmp(cmd->commands[0], "unset") == 0
		|| ft_strcmp(cmd->commands[0], "help") == 0)
		return (1);
	return (0);
}
