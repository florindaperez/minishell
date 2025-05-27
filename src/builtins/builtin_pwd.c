/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 12:48:16 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/23 12:48:27 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
*	builtin_pwd:
*	Implementa el comando 'pwd' (print working directory).
*	Obtiene el directorio de trabajo actual usando getcwd, lo imprime en la
*	salida estándar y actualiza la variable de entorno "PWD".
*	Establece el estado de salida global.
*/
int	builtin_pwd(t_env *env)
{
	char	*current_wd;

	current_wd = getcwd(NULL, 0);
	if (current_wd == NULL)
	{
		ft_putstr_fd("minishell: pwd: ", STDERR_FILENO);
		perror(NULL);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	ft_putstr_fd(current_wd, STDOUT_FILENO);
	ft_putchar_fd('\n', STDOUT_FILENO);
	env = update_env(env, "PWD", current_wd);
	free(current_wd);
	set_exit_status(EXIT_SUCCESS);
	return (EXIT_SUCCESS);
}

/*
*	update_pwd:
*	Actualiza la variable de entorno "PWD" con el directorio de trabajo actual.
*	Esta función puede ser llamada por otras partes del shell, como por ejemplo
*	el comando 'cd', para mantener la variable "PWD" sincronizada.
*	Retorna NULL; su propósito es el efecto secundario de actualizar el entorno.
*/
char	*update_pwd(t_env *env)
{
	char	*current_wd;

	current_wd = getcwd(NULL, 0);
	if (!current_wd)
	{
		return (NULL);
	}
	env = update_env(env, "PWD", current_wd);
	free(current_wd);
	return (NULL);
}

/*
*	exist_cwd:
*	Comprueba si el directorio de trabajo actual es accesible.
*	Utiliza getcwd para intentar obtener la ruta actual.
*	Devuelve:
*	  1 si getcwd tiene éxito (el directorio actual existe y es accesible).
*	  0 si getcwd falla (por ejemplo, si el directorio ha sido eliminado).
*/
int	exist_cwd(void)
{
	char	*curr_wd;

	curr_wd = getcwd(NULL, 0);
	if (curr_wd == NULL)
		return (0);
	free(curr_wd);
	return (1);
}
