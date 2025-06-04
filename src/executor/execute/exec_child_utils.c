/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 16:49:38 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/11 16:49:41 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * Maneja casos donde el nombre del comando es una cadena vacía, ya sea
 * literalmente desde el parser o después de la expansión.
 * Esta función siempre termina el proceso hijo con el código de salida 127.
*/
void	child_handle_empty_or_not_found(t_cmd_exe *cmd, char **argv)
{
	(void)cmd;
	msg_error_cmd(argv[0], NULL, "command not found", 127);
	free_str_tab(argv);
	exit(127);
}

/*
 * Valida un path (existencia, si es directorio, permisos de ejecución).
 * Muestra mensajes de error apropiados.
 * Retorna 0 si el path es válido y ejecutable, o un código de error (126 o 127).
*/
int	get_path_execution_errors(const char *exec_path, \
									const char *cmd_name_for_err)
{
	struct stat	path_stat;

	if (stat(exec_path, &path_stat) != 0)
	{
		msg_error_cmd((char *)cmd_name_for_err, NULL, strerror(errno), 127);
		return (127);
	}
	if (S_ISDIR(path_stat.st_mode))
	{
		msg_error_cmd((char *)exec_path, NULL, "Is a directory", 126);
		return (126);
	}
	if (access(exec_path, X_OK) != 0)
	{
		msg_error_cmd((char *)exec_path, NULL, strerror(errno), 126);
		return (126);
	}
	return (0);
}
