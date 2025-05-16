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

/*
 * redir_utils.c:
 * Este archivo contiene funciones de utilidad para la preparación y
 * configuración de las redirecciones de E/S en minishell.
 * Incluye la creación de rutas de directorios necesarias, la determinación
 * de nombres de comandos para mensajes de error y la copia de seguridad
 * de los descriptores de fichero estándar.
 */

/*
 * Función auxiliar estática para intentar crear un directorio si no existe.
 * Es llamada por redir_create_path_if_needed.
 *
 * path_to_create: La ruta del directorio que se intentará crear.
 * err_buf:        Buffer para formatear mensajes de error específicos de mkdir.
 *
 * Retorna: 0 en caso de éxito o si el directorio ya existía.
 * -1 en caso de error al llamar a mkdir (y errno no sea EEXIST).
 * Establece g_exit_status en 1 en caso de error.
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
			g_exit_status = 1;
			ret = -1;
		}
	}
	return (ret);
}

/*
 * Extrae el componente de directorio de un filepath y lo asigna.
 * Maneja errores de ft_strndup internamente.
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
		g_exit_status = 1;
		return (NULL);
	}
	return (dir_path_to_create);
}

/*
 * Asegura que la ruta del directorio para un 'filepath' exista.
 * Si la ruta del directorio no existe, intenta crearla.
 *
 * filepath: La ruta completa al archivo para el cual asegurar el directorio.
 *
 * Retorna: 0 en caso de éxito (directorio existe o fue creado).
 * -1 en caso de error (ej. ft_strndup falló, o mkdir falló).
 * Establece g_exit_status en 1 en caso de error.
 */
int	redir_create_path_if_needed(const char *filepath)
{
	char	*dir_path_to_create;
	int		ret;
	char	perror_msg_buffer[1024];

	if (!filepath)
		return (-1);
	dir_path_to_create = extract_dir_path_component(filepath);
	if (g_exit_status == 1 && !dir_path_to_create)
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
 * Determina el nombre del comando para usar en mensajes de error.
 * Utiliza el primer argumento del comando.
 *
 * cmd: Puntero a la estructura del comando (t_cmd).
 *
 * Retorna: El nombre del comando si está disponible.
 * NULL si no hay argumentos o el primer argumento es inválido.
 */
char	*redir_determine_cmd_name(t_cmd *cmd)
{
	if (cmd && cmd->args && cmd->args[0]
		&& cmd->args[0]->value && cmd->args[0]->value[0] != '\0')
		return (cmd->args[0]->value);
	return (NULL);
}

/*
 * Realiza una copia de seguridad de los descriptores de fichero estándar
 * (STDIN_FILENO, STDOUT_FILENO) si hay alguna redirección de E/S
 * especificada en la estructura io.
 * Los descriptores originales se guardan en io->stdin_backup y 
 * io->stdout_backup.
 *
 * io: Puntero a la estructura t_cmd_io que contiene la información de E/S.
 *
 * Retorna: true si la copia de seguridad fue exitosa o no fue necesaria.
 * false si falló la llamada a dup(2).
 * Establece g_exit_status en 1 en caso de fallo.
 */
bool	redir_backup_fds(t_cmd_io *io)
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
			g_exit_status = 1;
			return (false);
		}
	}
	return (true);
}
