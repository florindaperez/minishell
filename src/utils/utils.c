
#include "minishell.h"

/*
 * free_data_env: Libera la memoria de la estructura t_data_env.
 * Libera los campos internos que son punteros (env, pwd, old_pwd, cmd).
*
void	free_data_env(t_data_env *data)
{
	if (!data)
	{
		return ;
	}
	if (data->env)
	{
		str_free_and_null(data->env);
		data->env = NULL;
	}
	free_ptr(data->pwd);
	data->pwd = NULL;
	free_ptr(data->old_pwd);
	data->old_pwd = NULL;
	if (data->cmd)
	{
		free_commands(data->cmd);
		data->cmd = NULL;
	}
}*/

/*
 * ft_strjoin_free: Une dos strings (s1 y s2) y libera s1 después.
*/
char	*ft_strjoin_free(char *s1, char const *s2)
{
	char	*result;

	if (!s1)
		return (ft_strdup(s2));
	result = ft_strjoin(s1, s2);
	free_ptr(s1);
	return (result);
}

/*
 * Cierra de forma segura un descriptor de archivo y lo marca como -1.
 * Solo intenta cerrar si el puntero es válido y el fd no es -1.
 *
 * fd => Puntero al descriptor de archivo a cerrar.
 */
void	safe_close(int *fd)
{
	if (fd != NULL && *fd != -1)
	{
		close(*fd);
		*fd = -1;
	}
}

/*
 * Imprime un mensaje de error del sistema en stderr y sale.
 *  
 * Formato: "minishell: <context>: <error_del_sistema>\n"
 *  Utilizado para errores fatales (fork, pipe, dup2 crítico, etc.)
 *  donde la ejecución no puede continuar de forma segura.
 *
 * context => Un string descriptivo del lugar donde ocurrió el error
 *                (e.g., "pipe", "fork", "dup2 input redirection").
 * g_get_signal => El código de salida con el que terminará el programa.
 */
void	perror_exit(const char *context, int g_get_signal)
{
	char	*err_str;

	err_str = strerror(errno);
	write(STDERR_FILENO, "minishell: ", 11);
	if (context)
	{
		write(STDERR_FILENO, context, ft_strlen(context));
		write(STDERR_FILENO, ": ", 2);
	}
	write(STDERR_FILENO, err_str, ft_strlen(err_str));
	write(STDERR_FILENO, "\n", 1);
	exit(g_get_signal);
}
