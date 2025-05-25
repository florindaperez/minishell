/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 16:48:15 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/23 16:48:22 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "minishell.h"

/*
* is_valid_unset_key_char:
* Verifica si un carácter es válido para una clave de variable de entorno en unset.
* Las claves no pueden comenzar con un dígito.
*/
static bool is_valid_unset_key_char(char c, bool is_first_char)
{
    if (is_first_char)
    {
        return (ft_isalpha(c) || c == '_');
    }
    return (ft_isalnum(c) || c == '_');
}

/*
* is_valid_key_for_unset:
* Verifica si una cadena es una clave válida para 'unset'.
* Una clave válida para unset consta solo de letras, números y '_',
* y no comienza con un dígito. No debe contener '=', '+', etc.
*/
static bool is_valid_key_for_unset(const char *key)
{
    if (!key || !(*key)) // Clave nula o vacía no es válida
        return (false);

    if (!is_valid_unset_key_char(key[0], true)) // Comprobar primer carácter
        return (false);

    for (int i = 1; key[i]; i++)
    {
        // Comprobar el resto de caracteres
        if (!is_valid_unset_key_char(key[i], false))
            return (false); // Carácter inválido encontrado (como '=', '+', etc.)
    }
    return (true); // Todos los caracteres son válidos para un nombre de variable
}

/*
* process_unset_argument:
* Procesa un único argumento para el comando unset.
* Valida el argumento y lo elimina del entorno si es válido.
* Retorna EXIT_SUCCESS o EXIT_FAILURE.
*/
static int process_unset_argument(const char *arg, t_cmd *cmd_struct, t_env **env_ptr)
{
    (void)cmd_struct; // Marcar como no utilizado si ft_msgs no se usa para este error específico

    // Para 'unset', el argumento debe ser un nombre de variable válido.
    // No debe contener '=', '+', etc.
    if (!is_valid_key_for_unset(arg))
    {
        // Mensaje de error específico para 'unset', similar a bash:
        // bash: unset: `ARG': not a valid identifier
        ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
        ft_putstr_fd((char *)arg, STDERR_FILENO); // Cast si ft_putstr_fd de tu libft espera char*
        ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
        // set_exit_status(1); // No se establece aquí; se hace al final del builtin si algún argumento falló.
        return (EXIT_FAILURE); // Indica que este argumento específico falló
    }

    // Si la clave es sintácticamente válida, intentar eliminarla.
    // env_delone de jocuni elimina el nodo si la clave coincide.
    // No es un error si la variable no existía.
    // env_delone espera char ** como segundo argumento, que apunta a la clave a eliminar.
    // El contenido de 'arg' es la clave.
    char *key_to_delete = (char *)arg; 
    env_delone(env_ptr, &key_to_delete, free); // free se usa para el contenido del nodo (key/val)

    return (EXIT_SUCCESS); // Éxito, incluso si la variable no existía (comportamiento estándar)
}

/*
* builtin_unset: (Renombrar a builtin_unset en el archivo final)
* Elimina variables del entorno.
* Adaptado de la lógica de flperez para las estructuras de jocuni.
*/
int builtin_unset(t_cmd *cmd, t_env **env_ptr) // Firma de jocuni
{
    int i;
    int individual_arg_status;
    int overall_shell_exit_status;

    // Si 'unset' se ejecuta sin argumentos, no hace nada y retorna éxito.
    if (cmd->commands[1] == NULL)
    {
        set_exit_status(0);
        return (0);
    }

    overall_shell_exit_status = 0; // Asumir éxito para $? por defecto
    i = 1; // cmd->commands[0] es "unset"

    // Procesar cada argumento
    while (cmd->commands[i] != NULL)
    {
        individual_arg_status = process_unset_argument(cmd->commands[i], cmd, env_ptr);
        if (individual_arg_status == EXIT_FAILURE)
        {
            overall_shell_exit_status = 1; // Si cualquier argumento es inválido, $? será 1
                                          // Bash continúa procesando otros argumentos.
        }
        i++;
    }
    
    set_exit_status(overall_shell_exit_status);
    return (overall_shell_exit_status); // El builtin retorna el mismo estado que $?
}


/* removes environment variables specified in the command, checking their 
syntax and existence, and sets a status flag based on the result.
static int	builtin_unset_core(t_cmd *cmd, t_env **env, int *flag)
{
	int	chk_exp;
	int	i;

	i = 1;
	while (cmd->commands[i] != NULL)
	{
		if (check_syntax(cmd->commands[i]))
			return (1);
		chk_exp = check_export(cmd->commands[i]);
		if (chk_exp == 1 || chk_exp == 2)
			return (0);
		else
		{
			if (!(variable_exists_op3(*env, cmd->commands[i])))
				*flag = 1;
			else
			{
				env_delone(env, &cmd->commands[i], &free);
				*flag = 2;
			}
		}
		i++;
	}
	return (0);
}

builtin unset
int	builtin_unset(t_cmd *cmd, t_env **env)
{
	int	flag;

	flag = 0;
	if (size_arr2d(cmd->commands) == 1)
	{
		set_exit_status(0);
		return (0);
	}
	else
	{
		if (builtin_unset_core(cmd, env, &flag))
		{
			ft_msgs(5, cmd);
			return (1);
		}
		builtin_unset_core(cmd, env, &flag);
	}
	set_exit_status(0);
	return (0);
}
*/
