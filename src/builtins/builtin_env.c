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
 * Maneja los argumentos pasados al comando 'env'.
 * Si se proporcionan argumentos, muestra un error y establece el estado de
 * salida.
 * Devuelve 0 si no hay argumentos (y env debe continuar).
 * Devuelve 127 si hubo argumentos (y env debe terminar con ese estado).
 */
static int
	handle_env_args(char **args)
{
	if (args[1] != NULL)
	{
		ft_putstr_fd("minishell: env: ‘", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putstr_fd("’: No such file or directory\n", STDERR_FILENO);
		set_exit_status(127);
		return (127);
	}
	return (0);
}

/*
 * Imprime las variables de entorno de la lista enlazada t_env.
 * Si se proporcionan argumentos, muestra un error similar al de bash.
 */
int
	builtin_env(t_cmd *cmd, t_env *env)
{
	t_env	*current;
	int		arg_status;

	arg_status = handle_env_args(cmd->commands);
	if (arg_status != 0)
	{
		return (arg_status);
	}
	current = env;
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
 * Verifica si la variable de entorno PATH está definida en env.
 * Es llamada por pre_executor.	FUNCION ELIMINADA YA NO ES LLAMADA 
 *
 * Parámetros:
 * cmd: Estructura del comando actual (para ft_msgs).
 * exe: Estructura de ejecución (no utilizada aquí).
 * env: Puntero a la cabeza de la lista de entorno.
 *
 * Retorna:
 * 0 si PATH existe.
 * 1 si PATH no existe (e imprime un mensaje de error).
 *
int	no_path_env(t_cmd *cmd, t_exe exe, t_env *env)
{
	(void) exe;
	if (!variable_exists_op3(env, "PATH"))
	{
		ft_msgs(4, cmd);
		return (1);
	}
	return (0);
}
*/