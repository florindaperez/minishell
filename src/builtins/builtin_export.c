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

// Funciones de utilidad de minishell_jocuni que se usarán:
// t_env *update_env(t_env *envlist, char *key, char *val); 
//   (de src/builtins/utils_export/var_exists.c, actualiza/añade a la lista t_env)
// void ft_msgs(int error_code, t_cmd *cmd);
// void set_exit_status(int status);
// ft_strchr, ft_substr, ft_strdup, ft_strcmp, ft_isalpha, ft_isalnum, free

/*
* just_export: (Basada en just_export de jocuni)
* Imprime las variables de entorno en formato 'declare -x KEY="VALUE"'.
* Es llamada cuando 'export' se ejecuta sin argumentos.
*/
static int just_export(t_env *envlist)
{
    t_env *current = envlist;
    while (current != NULL)
    {
        // Asegurarse de que key y val no sean NULL para printf
        // Si val es NULL (p.ej. export VAR sin asignación previa), printf lo tratará como "(null)"
        // o podría ser un comportamiento indefinido. Es mejor asegurar que val sea al menos "".
        // La función update_env (de jocuni) debería asegurar que val sea "" si el valor es NULL.
        if (current->key)
        {
            if (current->val)
            {
                printf("declare -x %s=\"%s\"\n", current->key, current->val);
            }
            else // Si val es NULL (variable exportada sin valor asignado explícitamente)
            {
                printf("declare -x %s\n", current->key); // O "" según el comportamiento deseado
            }
        }
        current = current->next;
    }
    return (0);
}

/*
* is_valid_env_key_char:
* Verifica si un carácter es válido para una clave de variable de entorno.
*/
static bool is_valid_env_key_char(char c, bool is_first_char)
{
    if (is_first_char)
    {
        return (ft_isalpha(c) || c == '_');
    }
    return (ft_isalnum(c) || c == '_');
}

/*
* is_valid_env_key: (Basada en is_valid_env_var_key de flperez)
* Verifica si una cadena es una clave válida para variable de entorno.
* (Ej: "VAR_NAME", "_VAR", "V1"). No debe contener '='.
*/
static bool is_valid_env_key(const char *key)
{
    if (!key || !(*key) || ft_strchr(key, '=') != NULL) // La clave no debe tener '='
        return (false);

    if (!is_valid_env_key_char(key[0], true))
        return (false);

    for (int i = 1; key[i]; i++)
    {
        if (!is_valid_env_key_char(key[i], false))
            return (false);
    }
    return (true);
}

/*
* process_export_argument:
* Procesa un único argumento para el comando export (ej: "VAR=value" o "VAR").
* Valida el argumento y lo añade/actualiza en la lista de entorno.
*/
static int process_export_argument(const char *arg, t_cmd *cmd_struct, t_env **env_ptr)
{
    (void)cmd_struct;
	char *key = NULL;
    char *value_str = NULL; // Valor a almacenar (puede ser "" si no se especifica)
    char *equal_sign_pos;

    equal_sign_pos = ft_strchr(arg, '=');

    if (equal_sign_pos != NULL) // Formato "KEY=value" o "KEY="
    {
        key = ft_substr(arg, 0, equal_sign_pos - arg);
        if (!key) 
        {
            perror("minishell: export (ft_substr)"); // O usar ft_msgs si hay código
            return (EXIT_FAILURE);
        }
        value_str = ft_strdup(equal_sign_pos + 1); // ft_strdup maneja bien si equal_sign_pos+1 es '\0'
        if (!value_str)
        {
            perror("minishell: export (ft_strdup)");
            free(key);
            return (EXIT_FAILURE);
        }
    }
    else // Formato "KEY"
    {
        key = ft_strdup(arg);
        if (!key)
        {
            perror("minishell: export (ft_strdup)");
            return (EXIT_FAILURE);
        }
        // Para 'export VAR', la variable se exporta sin valor o con valor vacío.
        // La función update_env de jocuni (en var_exists.c) hace strdup(val).
        // Si pasamos NULL, ft_strdup(NULL) crashea.
        // Pasamos "" para que se almacene como una cadena vacía.
        value_str = ft_strdup("");
        if (!value_str)
        {
             perror("minishell: export (ft_strdup for empty value)");
             free(key);
             return(EXIT_FAILURE);
        }
    }

    if (!is_valid_env_key(key))
    {
        // Guardar el argumento original para el mensaje de error
        // Necesitamos pasar la estructura cmd completa a ft_msgs, y el argumento problemático.
        // ft_msgs(5, cmd_struct) imprime "minishell: export: `ARG`: not a valid identifier"
        // Necesitamos asegurar que ft_msgs pueda acceder al 'ARG' problemático.
        // Temporalmente, podemos usar un mensaje directo si ft_msgs no es flexible.
        ft_putstr_fd("minishell: export: `", STDERR_FILENO);
        ft_putstr_fd(arg, STDERR_FILENO); // Imprime el argumento original
        ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
        
        free(key);
        free(value_str);
        set_exit_status(1); // Bash devuelve 1 para identificador no válido
        return (EXIT_FAILURE); 
    }

    // Usar la función update_env de jocuni que toma t_env* y devuelve t_env*
    // Esta función ya está en src/builtins/utils_export/var_exists.c
    // y maneja la actualización o adición a la lista.
    *env_ptr = update_env(*env_ptr, key, value_str);
    // No hay un código de error específico de update_env que podamos verificar aquí fácilmente,
    // asumimos que maneja errores de malloc internamente (ej. retornando el envlist original).

    free(key);
    free(value_str);
    return (EXIT_SUCCESS);
}

/*
* builtin_export: (Renombrar a builtin_export en el archivo final)
* Agrega o modifica variables en el entorno, o muestra todas las variables exportables.
* Adaptado de la lógica de flperez para usar las estructuras de jocuni.
*/
int builtin_export(t_cmd *cmd, t_env **env_ptr) // Firma de jocuni
{
    int i;
    int overall_ret_status;

    overall_ret_status = EXIT_SUCCESS; // Asumir éxito hasta que un argumento falle
    i = 1; // cmd->commands[0] es "export"

    // Si no hay argumentos (solo "export")
    if (cmd->commands[i] == NULL)
    {
        just_export(*env_ptr); // Llama a la función de jocuni para imprimir
        set_exit_status(0);
        return (0);
    }

    // Procesar cada argumento
    while (cmd->commands[i] != NULL)
    {
        if (process_export_argument(cmd->commands[i], cmd, env_ptr) == EXIT_FAILURE)
        {
            overall_ret_status = EXIT_FAILURE; // Si algún argumento falla, el estado general es de fallo
            // Bash continúa procesando los siguientes argumentos incluso si uno es inválido.
        }
        i++;
    }

    if (overall_ret_status == EXIT_FAILURE)
    {
        set_exit_status(1); // Código de error general para export si algún argumento fue inválido
        return (1);
    }
    
    set_exit_status(0);
    return (0);
}

/*print the export output(without argument)
static int just_export(t_env *env)
{
	while (env != NULL)
	{
		printf("declare -x %s=\"%s\"\n", env->key, env->val);
		env = env->next;
	}
	return (0);
}

check if a variable exists. If not, create it
int create_variable(char *cmd, t_env **env)
{
	char **tokens;
	t_env *new_var;

	tokens = ft_split(cmd, '=');
	if (tokens && tokens[0] && tokens[1])
	{
		if (!variable_exists(env, tokens))
		{
			new_var = lstnew(tokens[0], tokens[1]);
			lstadd_back(env, new_var);
		}
		free_arr2d(tokens);
	}
	else if (tokens && tokens[0])
	{
		if (!variable_exists_op3(*env, tokens[0]))
		{
			new_var = lstnew(tokens[0], " ");
			lstadd_back(env, new_var);
		}
		free_arr2d(tokens);
	}
	return (0);
}

check if a variable exists. If not, create it. If yes, concatenates it
int overwrite_variable(t_env *env, char *cmd)
{
	char **tokens;
	char **tokens2;
	t_env *new_var;

	tokens = ft_split(cmd, '+');
	if (tokens && tokens[0])
	{
		tokens2 = ft_split(tokens[1], '=');
		if (tokens2 && tokens2[0])
		{
			if (!variable_exists_op2(env, cmd))
			{
				new_var = lstnew(tokens[0], tokens2[0]);
				lstadd_back(&env, new_var);
			}
			free_arr2d(tokens2);
		}
		free_arr2d(tokens);
	}
	return (0);
}

Function that adds a new environment variable if applicable.
int builtin_export_core(t_cmd *cmd, t_env **env)
{
	int i;
	int chk_exp;

	i = 1;
	while (cmd->commands[i] != NULL)
	{
		if (check_syntax(cmd->commands[i]))
		{
			ft_msgs(5, cmd);
			return (1);
		}
		chk_exp = check_export(cmd->commands[i]);
		if ((chk_exp == 1) || (chk_exp == 3))
			create_variable(cmd->commands[i], env);
		else if (chk_exp == 2)
			overwrite_variable(*env, cmd->commands[i]);
		else
			return (1);
		i++;
	}
	return (0);
}

builtin export
int builtin_export(t_cmd *cmd, t_env **env)
{
	if (size_arr2d(cmd->commands) == 1)
		just_export(*env);
	else
		builtin_export_core(cmd, env);
	set_exit_status(0);
	return (0);
}
*/
