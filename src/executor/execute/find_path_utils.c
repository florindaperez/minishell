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
* Une dos componentes de ruta con un '/'.
* Maneja el caso donde s1 ya termina con '/'.
* Devuelve una nueva cadena alocada, o NULL si falla malloc.
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
* Realiza validaciones básicas sobre el nombre del comando.
* Establece cmd_name_out si la validación es exitosa.
* Imprime mensajes de error y devuelve false si falla.
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
* Contiene la lógica principal de búsqueda de ruta (explícita, PATH, caso "..").
* Esta función llamaría a las funciones estáticas de find_path.c o
* necesitaría que esa lógica se pase aquí. Para este ejemplo,
* la función find_command_path en find_path.c se encargará de orquestar
* las llamadas a sus propias funciones estáticas basado en esta lógica.
* Esta función (search_and_set_cmd_path) es conceptual para mostrar la división.
* En la práctica, su lógica se integrará en find_command_path de forma más 
* directa si las auxiliares estáticas de find_path.c se mantienen allí.
* Para simplificar y mantener find_path.c con sus 5 funciones, la siguiente
* `handle_path_search_error` es la función clave que se externaliza.
*/

/*
* Maneja los diferentes estados de error de la búsqueda de path,
* emitiendo el mensaje apropiado.
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
