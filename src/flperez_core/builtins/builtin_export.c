/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 10:03:35 by flperez-          #+#    #+#             */
/*   Updated: 2025/02/28 13:37:40 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
* static int	apply_parsed_export_arg(const char *orig_arg, ...)
* -----------------------------------------------------------------
* Valida la clave parseada. Si es válida, la añade/actualiza en el entorno
* usando add_or_update_env_list_var (de utils). Libera key y value.
* Retorna EXIT_SUCCESS o EXIT_FAILURE.
*/
static int	apply_parsed_export_arg(const char *orig_arg, char *key, \
									char *value, t_data_env_exe *data)
{
	int	status;

	status = EXIT_SUCCESS;
	if (!is_valid_env_var_key(key))
	{
		msg_error_cmd("export", (char *)orig_arg, \
						"not a valid identifier", 1);
		set_exit_status(EXIT_FAILURE);
		status = EXIT_FAILURE;
	}
	else
	{
		if (!add_or_update_env_list_var(data->shell_env_list_ptr, key, value))
		{
			msg_error_cmd("export", (char *)orig_arg, \
						"memory allocation error", EXIT_FAILURE);
			set_exit_status(EXIT_FAILURE);
			status = EXIT_FAILURE;
		}
	}
	free_ptr(key);
	free_ptr(value);
	return (status);
}

/*
* static int	process_single_export_arg(const char *arg, ...)
* --------------------------------------------------------------
* Procesa un argumento para 'export'. Llama a parse_export_argument_pair y
* luego a apply_parsed_export_arg.
* Retorna: EXIT_SUCCESS o EXIT_FAILURE.
*/
static int	process_single_export_arg(const char *arg, t_data_env_exe *data)
{
	char	*key;
	char	*value;

	if (!parse_export_argument_pair(arg, &key, &value))
	{
		msg_error_cmd("export", (char *)arg, \
			"memory allocation error parsing argument", EXIT_FAILURE);
		set_exit_status(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	return (apply_parsed_export_arg(arg, key, value, data));
}

/*
* static bool	regenerate_exec_env_after_export(t_data_env_exe *data, ...)
* -----------------------------------------------------------------------
* Si env_modified es true, libera y regenera data->env_for_execve.
* Retorna: true si éxito o no necesario, false si la regeneración falló.
*/
static bool	regenerate_exec_env_after_export(t_data_env_exe *data, \
												bool env_modified)
{
	bool	conversion_failed;
	bool	source_list_existed;

	if (env_modified)
	{
		free_arr2d(data->env_for_execve);
		data->env_for_execve = NULL;
		source_list_existed = (data->shell_env_list_ptr && \
								*(data->shell_env_list_ptr));
		if (source_list_existed)
			data->env_for_execve = \
				convert_envlist_to_envp_exe(*(data->shell_env_list_ptr));
		conversion_failed = (!data->env_for_execve);
		if (conversion_failed && source_list_existed)
		{
			msg_error_cmd("export", "(internal)", \
				"env array update failed", 1);
			return (false);
		}
	}
	return (true);
}

/*
* static void	iterate_and_process_export_args(char **args, ...)
* -----------------------------------------------------------------
* Itera sobre los argumentos de export (a partir de args[1]) y los procesa.
* Actualiza overall_status_ptr y env_modified_ptr con los resultados.
*/
static void	iterate_and_process_export_args(char **args, \
											t_data_env_exe *data, \
											int *overall_status_ptr, \
											bool *env_modified_ptr)
{
	int	i;

	i = 1;
	while (args[i])
	{
		if (process_single_export_arg(args[i], data) == EXIT_FAILURE)
			*overall_status_ptr = EXIT_FAILURE;
		else
			*env_modified_ptr = true;
		i++;
	}
}

/*
* int	builtin_export(char **args, t_data_env_exe *data)
* -------------------------------------------------------
* Implementa el comando 'export'.
*/
int	builtin_export(char **args, t_data_env_exe *data)
{
	int		overall_status;
	bool	env_modified;

	overall_status = EXIT_SUCCESS;
	env_modified = false;
	if (!args[1])
	{
		print_export_formatted_env(*(data->shell_env_list_ptr));
		set_exit_status(EXIT_SUCCESS);
		return (EXIT_SUCCESS);
	}
	iterate_and_process_export_args(args, data, &overall_status, &env_modified);
	if (!regenerate_exec_env_after_export(data, env_modified))
		overall_status = EXIT_FAILURE;
	else if (overall_status == EXIT_SUCCESS)
		set_exit_status(EXIT_SUCCESS);
	return (overall_status);
}
