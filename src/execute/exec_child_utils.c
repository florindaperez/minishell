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

/*
 * Maneja casos de comando vacío literal ("" o '') o expansión a cadena vacía.
 * Esta función siempre termina el proceso hijo con el código de salida 
 * apropiado.
 */
void	child_handle_empty_or_not_found(t_cmd *cmd, char **argv)
{
	if (cmd->was_literal_empty_command)
	{
		msg_error_cmd("", NULL, "command not found", 127);
		free_str_tab(argv);
		exit(127);
	}
	else if (argv[0] && argv[0][0] == '\0')
	{
		free_str_tab(argv);
		exit(EXIT_SUCCESS);
	}
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
