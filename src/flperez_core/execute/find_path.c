/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 02:40:33 by flperez-          #+#    #+#             */
/*   Updated: 2025/04/25 02:41:10 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
# include "minishell_executor.h"

/*
 * Une dos componentes de ruta con un '/'.
 * Maneja el caso donde s1 ya termina con '/'.
 * Devuelve una nueva cadena alocada, o NULL si falla malloc.
 */
static char	*ft_strjoin_path(char const *s1, char const *s2)
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
	if (s1_ends_with_slash)
		len1--;
	result = (char *)malloc(sizeof(char) * (len1 + 1 + len2 + 1));
	if (!result)
		return (NULL);
	ft_memcpy(result, s1, len1);
	result[len1] = '/';
	ft_memcpy(result + len1 + 1, s2, len2);
	result[len1 + 1 + len2] = '\0';
	return (result);
}

/*
 * Verifica y asigna una ruta explícita (que contiene '/').
 * cmd => El comando cuya ruta se va a establecer.
 * cmd_name => El nombre del comando a verificar.
 * return => 1 si la ruta explícita es válida y se asigna.
 * 0 si no es una ruta explícita (no contiene '/').
 * -1 si es una ruta explícita pero hay un error (no existe o fallo de strdup).
 */
static int	handle_explicit_path(t_cmd_exe *cmd, char *cmd_name)
{
	if (ft_strchr(cmd_name, '/') == NULL)
		return (0);
	if (access(cmd_name, F_OK) == 0)
	{
		cmd->path = ft_strdup(cmd_name);
		if (!cmd->path)
		{
			msg_error_cmd(cmd_name, "strdup failed", strerror(errno), 1);
			return (-1);
		}
		return (1);
	}
	return (-1);
}

/*
 * Itera sobre los directorios en 'paths' para encontrar 'cmd_name'.
 * return => La ruta completa alocada si se encuentra,
 * NULL si hay un error de alocación,
 * una cadena vacía literal "" si no se encuentra en el PATH.
 */
static char	*search_in_paths_array(char **paths, char *cmd_name)
{
	int		i;
	char	*temp_path;

	i = 0;
	while (paths[i])
	{
		if (paths[i][0] == '\0' || (paths[i][0] == '.' && paths[i][1] == '\0'))
			temp_path = ft_strjoin_path(".", cmd_name);
		else
			temp_path = ft_strjoin_path(paths[i], cmd_name);
		if (!temp_path)
		{
			msg_error_cmd(cmd_name, "ft_strjoin_path failed", \
				strerror(errno), 1);
			return (NULL);
		}
		if (access(temp_path, F_OK) == 0)
			return (temp_path);
		free_ptr(temp_path);
		i++;
	}
	return ("");
}

/*
 * Intenta encontrar el comando buscando en la variable de entorno PATH.
 * cmd => El comando cuya ruta se va a establecer.
 * cmd_name => El nombre del comando.
 * data => Datos del entorno que contienen la variable PATH.
 * return => true si se encuentra y se asigna, false en caso contrario o error.
 */
static bool	try_search_in_env_path(t_cmd_exe *cmd, char *cmd_name, t_data_env_exe *data)
{
	char	*path_env_value;
	char	**paths_array;
	char	*found_path_str;

	path_env_value = get_env_var_val(data->env, "PATH");
	if (!path_env_value || *path_env_value == '\0')
		return (false);
	paths_array = ft_split(path_env_value, ':');
	if (!paths_array)
	{
		msg_error_cmd(cmd_name, "ft_split for PATH failed", \
			strerror(errno), 1);
		return (false);
	}
	found_path_str = search_in_paths_array(paths_array, cmd_name);
	str_free_and_null(paths_array);
	if (!found_path_str)
		return (false);
	if (found_path_str[0] == '\0')
		return (false);
	cmd->path = found_path_str;
	return (true);
}

/*
 * Busca la ruta ejecutable de un comando.
 * Primero verifica si es una ruta explícita, luego busca en PATH.
 * Almacena la ruta completa encontrada en cmd->path.
 *
 * cmd => Puntero al comando.
 * data => Puntero a los datos del entorno.
 * return => true si la ruta es válida y se encuentra, false si no.
 */
bool	find_command_path(t_cmd_exe *cmd, t_data_env_exe *data)
{
	char	*cmd_name;
	int		explicit_status;

	if (!cmd || !cmd->args || !cmd->args[0] || !cmd->args[0]->value
		|| cmd->args[0]->value[0] == '\0')
		return (false);
	cmd_name = cmd->args[0]->value;
	free_ptr(cmd->path);
	cmd->path = NULL;
	explicit_status = handle_explicit_path(cmd, cmd_name);
	if (explicit_status == 1)
		return (true);
	if (explicit_status == -1)
		return (false);
	return (try_search_in_env_path(cmd, cmd_name, data));
}
