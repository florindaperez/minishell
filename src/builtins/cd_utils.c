/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 17:34:13 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/26 17:34:29 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h" 

/*
 * Recupera el valor de una variable de entorno.
 * Devuelve el valor (char *) o NULL si no se encuentra.
 * Asume que variable_exists_op4 ahora toma const char *key.
 */
char
	*util_cd_get_env_val(t_env *env_list, const char *var_name)
{
	t_env	*node;

	node = variable_exists_op4(env_list, var_name);
	if (node && node->val)
		return (node->val);
	return (NULL);
}

/*
 * Establece o actualiza el valor de una variable de entorno.
 * Duplica var_name y value para pasarlos a update_env,
 * ya que update_env espera char* y podría gestionar esta memoria.
 */
void
	util_cd_set_env_val(t_env **env_list, const char *var_name,
	const char *value)
{
	char	*key_copy;
	char	*value_copy;

	key_copy = ft_strdup(var_name);
	if (!key_copy)
	{
		msg_error_cmd("cd", "strdup for key", "Memory allocation failed",
			EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return ;
	}
	value_copy = ft_strdup(value);
	if (!value_copy)
	{
		msg_error_cmd("cd", "strdup for value", "Memory allocation failed",
			EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		free(key_copy);
		return ;
	}
	*env_list = update_env(*env_list, key_copy, value_copy);
	free(key_copy);
	free(value_copy);
}

/*
 * Captura el directorio de trabajo actual para usarlo como OLDPWD.
 * Primero intenta obtenerlo de la variable de entorno PWD,
 * luego usa getcwd() como fallback.
 * Devuelve true si se capturó una ruta, false en caso contrario.
 */
bool
	util_cd_capture_old_work_dir(char *buffer, size_t buf_size,
	t_env *env_list)
{
	char	*pwd_val;

	pwd_val = util_cd_get_env_val(env_list, "PWD");
	if (pwd_val && *pwd_val)
	{
		ft_strlcpy(buffer, pwd_val, buf_size);
		return (true);
	}
	else if (getcwd(buffer, buf_size) != NULL)
	{
		return (true);
	}
	return (false);
}
