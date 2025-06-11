/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:16:27 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/12 10:16:32 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * attempt_mkdir_if_needed
 * Intenta crear un directorio si no existe. Es una función auxiliar para
 * 'redir_create_path_if_needed'. Ignora el error si el directorio ya
 * existe (EEXIST), pero informa de otros errores de 'mkdir'.
 *
 * path_to_create: La ruta del directorio que se intentará crear.
 * err_buf:        Buffer para formatear mensajes de error específicos.
 *
 * Retorna: 0 en éxito o si el directorio ya existía, -1 en caso de error.
 */
static int	attempt_mkdir_if_needed(const char *path_to_create,
	char *err_buf)
{
	int	ret;

	ret = 0;
	if (access(path_to_create, F_OK) == -1)
	{
		if (mkdir(path_to_create, 0755) == -1 && errno != EEXIST)
		{
			snprintf(err_buf, 1024,
				"minishell: redir_utils: mkdir (%s)", path_to_create);
			perror(err_buf);
			g_get_signal = 1;
			ret = -1;
		}
	}
	return (ret);
}

/*
 * extract_dir_path_component
 * Extrae y asigna el componente de directorio de una ruta de fichero.
 * Por ejemplo, de "/path/to/file" extrae "/path/to".
 *
 * filepath: La ruta completa del fichero.
 *
 * Retorna: Una nueva cadena con la ruta del directorio, o NULL si no hay
 * componente de directorio o si falla la asignación de memoria.
 */
static char	*extract_dir_path_component(const char *filepath)
{
	char	*dir_path_to_create;
	char	*last_slash;

	last_slash = ft_strrchr(filepath, '/');
	if (!last_slash || last_slash == filepath)
		return (NULL);
	dir_path_to_create = ft_strndup(filepath, last_slash - filepath);
	if (!dir_path_to_create)
	{
		perror("minishell: redir_utils: ft_strndup failed");
		g_get_signal = 1;
		return (NULL);
	}
	return (dir_path_to_create);
}

/*
 * redir_create_path_if_needed
 * Asegura que la ruta del directorio para un 'filepath' de redirección
 * exista. Si la ruta del directorio no existe, intenta crearla.
 *
 * filepath: La ruta completa al archivo para el cual asegurar el directorio.
 *
 * Retorna: 0 en éxito, -1 en caso de error.
 */
int	redir_create_path_if_needed(const char *filepath)
{
	char	*dir_path_to_create;
	int		ret;
	char	perror_msg_buffer[1024];

	if (!filepath)
		return (-1);
	dir_path_to_create = extract_dir_path_component(filepath);
	if (g_get_signal == 1 && !dir_path_to_create)
		return (-1);
	if (!dir_path_to_create)
		return (0);
	if (dir_path_to_create[0] == '\0'
		|| ft_strcmp(dir_path_to_create, ".") == 0)
	{
		free(dir_path_to_create);
		return (0);
	}
	ret = attempt_mkdir_if_needed(dir_path_to_create, perror_msg_buffer);
	free(dir_path_to_create);
	return (ret);
}

/*
 * redir_determine_cmd_name
 * Determina el nombre del comando a partir de la estructura t_cmd_exe para
 * usarlo en mensajes de error, haciendo el feedback al usuario más claro.
 *
 * cmd: Puntero a la estructura del comando.
 *
 * Retorna: El nombre del comando si está disponible, o NULL si no.
 */
char	*redir_determine_cmd_name(t_cmd_exe *cmd)
{
	if (cmd && cmd->args && cmd->args[0]
		&& cmd->args[0]->value && cmd->args[0]->value[0] != '\0')
		return (cmd->args[0]->value);
	return (NULL);
}

/*
 * redir_backup_fds
 * Realiza una copia de seguridad de los descriptores de fichero estándar
 * (stdin y stdout) antes de aplicar cualquier redirección. Esto es crucial
 * para poder restaurarlos después de la ejecución del comando.
 *
 * io: Puntero a la estructura de E/S que contendrá los descriptores
 * de respaldo.
 *
 * Retorna: 'true' si la copia de seguridad fue exitosa o no fue necesaria,
 * 'false' si falló la llamada a 'dup'.
 */
bool	redir_backup_fds(t_cmd_io_exe *io)
{
	if (!io)
		return (false);
	if (io->infile || io->outfile || io->heredoc_delimiter)
	{
		io->stdin_backup = dup(STDIN_FILENO);
		io->stdout_backup = dup(STDOUT_FILENO);
		if (io->stdin_backup == -1 || io->stdout_backup == -1)
		{
			perror("minishell: redir_utils: dup backup failed");
			safe_close(&io->stdin_backup);
			safe_close(&io->stdout_backup);
			g_get_signal = 1;
			return (false);
		}
	}
	return (true);
}
