/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_path_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 00:12:14 by flperez-          #+#    #+#             */
/*   Updated: 2025/06/02 00:12:24 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * ft_strjoin_path
 * Une dos componentes de una ruta con un único separador '/'. Se asegura de
 * no duplicar el separador si el primer componente ya termina con uno.
 *
 * s1: El primer componente de la ruta (ej: /usr/bin).
 * s2: El segundo componente de la ruta (ej: ls).
 *
 * Retorna: Una nueva cadena de caracteres que representa la ruta completa
 * (ej: /usr/bin/ls), o NULL si falla la asignación de memoria.
 */
char	*ft_strjoin_path(char const *s1, char const *s2)
{
	char	*result;
	size_t	len1;
	size_t	len2;
	bool	s1_ends_with_slash;

	if (!s1 || !s2)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	s1_ends_with_slash = (len1 > 0 && s1[len1 - 1] == '/');
	result = (char *)malloc(sizeof(char) * (len1 + len2 + \
				(1 - s1_ends_with_slash) + 1));
	if (!result)
		return (NULL);
	ft_memcpy(result, s1, len1);
	if (!s1_ends_with_slash)
	{
		result[len1] = '/';
		len1++;
	}
	ft_memcpy(result + len1, s2, len2);
	result[len1 + len2] = '\0';
	return (result);
}

/*
 * validate_basic_cmd_input
 * Realiza validaciones iniciales sobre el nombre de un comando antes de
 * intentar buscar su ruta. Comprueba casos nulos, vacíos o especiales como '.'.
 *
 * cmd:          El nodo del comando que se va a validar.
 * cmd_name_out: Puntero a un (char *) que se establecerá para apuntar al
 * nombre del comando si la validación es exitosa.
 *
 * Retorna: 'true' si el comando pasa las validaciones básicas, 'false' si se
 * encuentra un error.
 */
bool	validate_basic_cmd_input(t_cmd_exe *cmd, char **cmd_name_out)
{
	if (!cmd || !cmd->args || !cmd->args[0] || !cmd->args[0]->value)
	{
		msg_error_cmd("", NULL, "No such file or directory", 127);
		return (false);
	}
	*cmd_name_out = cmd->args[0]->value;
	if ((*cmd_name_out)[0] == '\0')
	{
		msg_error_cmd(*cmd_name_out, NULL, "No such file or directory", 127);
		return (false);
	}
	if (ft_strcmp(*cmd_name_out, ".") == 0)
	{
		msg_error_cmd(".", NULL, "filename argument required", 2);
		ft_putendl_fd("minishell: .: usage: . filename [arguments]", 2);
		return (false);
	}
	return (true);
}

/*
 * handle_path_search_error
 * Centraliza el manejo de errores que pueden ocurrir durante la búsqueda de la
 * ruta de un comando. Imprime un mensaje de error apropiado según el estado
 * de error proporcionado.
 *
 * status:   El código de estado que indica el tipo de error de la búsqueda.
 * cmd_name: El nombre del comando que causó el error, para incluirlo en el
 * mensaje.
 * data:     La estructura de datos del ejecutor, usada para verificar la
 * existencia de la variable PATH.
 */
void	handle_path_search_error(t_path_status status, \
									const char *cmd_name, \
									t_data_env_exe *data)
{
	char	*path_val_for_msg;

	if (status == PATH_ERR_IS_DIRECTORY)
		msg_error_cmd((char *)cmd_name, NULL, "Is a directory", 126);
	else if (status == PATH_ERR_NO_PERMISSION || \
			status == PATH_ERR_ACCESS_FAILED)
		msg_error_cmd((char *)cmd_name, NULL, "Permission denied", 126);
	else if (status == PATH_ERR_MALLOC)
		msg_error_cmd((char *)cmd_name, "memory error", strerror(errno), \
			EXIT_FAILURE);
	else if (status == PATH_ERR_STAT_FAILED)
		msg_error_cmd((char *)cmd_name, "stat error", strerror(errno), \
			EXIT_FAILURE);
	else if (status == PATH_NOT_IN_ENV)
	{
		path_val_for_msg = get_env_var_val(data->env_for_execve, "PATH");
		if (!path_val_for_msg || path_val_for_msg[0] == '\0')
			msg_error_cmd((char *)cmd_name, NULL, \
				"No such file or directory", 127);
		else
			msg_error_cmd((char *)cmd_name, NULL, "command not found", 127);
	}
	else
		msg_error_cmd((char *)cmd_name, NULL, "No such file or directory", 127);
}
