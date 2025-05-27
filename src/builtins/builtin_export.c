/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 16:11:06 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/23 16:11:18 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
*	just_export:
*	Imprime las variables de entorno cuando 'export' se ejecuta sin argumentos.
*	Formato: 'declare -x KEY="VALUE"' o 'declare -x KEY'.
*/
static int	just_export(t_env *env)
{
	t_env	*current;

	current = env;
	while (current != NULL)
	{
		if (current->key)
		{
			if (current->val)
				printf("declare -x %s=\"%s\"\n", current->key, current->val);
			else
				printf("declare -x %s\n", current->key);
		}
		current = current->next;
	}
	return (0);
}

/*
*	is_valid_env_key:
*	Verifica si 'key' es un identificador válido para una variable de entorno.
*	Debe empezar con letra o '_', seguido de alfanuméricos o '_'.
*	No debe contener '='.
*/
static bool	is_valid_env_key(const char *key)
{
	int	i;

	if (!key || !(*key) || ft_strchr(key, '=') != NULL)
		return (false);
	if (!(ft_isalpha(key[0]) || key[0] == '_'))
		return (false);
	i = 1;
	while (key[i])
	{
		if (!(ft_isalnum(key[i]) || key[i] == '_'))
			return (false);
		i++;
	}
	return (true);
}

/*
*	extract_key_value_from_arg:
*	Extrae clave (key_out) y valor (value_out) del argumento 'arg'.
*	Formatos: "KEY=value" o "KEY" (valor="").
*	Retorna true si éxito, false si falla malloc.
*/
static bool	extract_key_value_from_arg(const char *arg,
										char **key_out, char **value_out)
{
	char	*eq_pos;

	*key_out = NULL;
	*value_out = NULL;
	eq_pos = ft_strchr(arg, '=');
	if (eq_pos)
		*key_out = ft_substr(arg, 0, eq_pos - arg);
	else
		*key_out = ft_strdup(arg);
	if (!(*key_out))
		return (false);
	if (eq_pos)
		*value_out = ft_strdup(eq_pos + 1);
	else
		*value_out = ft_strdup("");
	if (!(*value_out))
	{
		free(*key_out);
		*key_out = NULL;
		return (false);
	}
	return (true);
}

/*
*	process_export_argument:
*	Procesa un argumento de 'export'. Extrae key/value, valida la clave,
*	y actualiza/añade la variable al entorno usando update_env.
*	Usa msg_error_cmd para errores. Retorna EXIT_SUCCESS o EXIT_FAILURE.
*/
static int	process_export_argument(const char *arg, t_env **env_ptr)
{
	char	*key;
	char	*value;

	key = NULL;
	value = NULL;
	if (!extract_key_value_from_arg(arg, &key, &value))
	{
		return (msg_error_cmd("export", NULL, \
			"memory allocation error\n", EXIT_FAILURE));
	}
	if (!is_valid_env_key(key))
	{
		msg_error_cmd("export", (char *)arg, \
			"not a valid identifier\n", EXIT_FAILURE);
		free(key);
		free(value);
		return (EXIT_FAILURE);
	}
	*env_ptr = update_env(*env_ptr, key, value);
	free(key);
	free(value);
	return (EXIT_SUCCESS);
}

/*
*	builtin_export:
*	Comando 'export'. Sin args, muestra variables (declare -x).
*	Con args, los añade/modifica en el entorno. Establece g_get_signal.
*/
int	builtin_export(t_cmd *cmd, t_env **env_ptr)
{
	int	i;
	int	arg_status;
	int	overall_status;

	overall_status = EXIT_SUCCESS;
	i = 1;
	if (cmd->commands[i] == NULL)
	{
		just_export(*env_ptr);
		set_exit_status(0);
		return (0);
	}
	while (cmd->commands[i] != NULL)
	{
		arg_status = process_export_argument(cmd->commands[i], env_ptr);
		if (arg_status == EXIT_FAILURE)
			overall_status = EXIT_FAILURE;
		i++;
	}
	set_exit_status(overall_status);
	return (overall_status);
}
