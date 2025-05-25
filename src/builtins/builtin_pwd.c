/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 12:48:16 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/23 12:48:27 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"


 
// La función update_env se asume que está definida en 
// src/builtins/utils_export/var_exists.c y declarada en minishell.h
// Su prototipo es: t_env *update_env(t_env *env, char *key, char *val);

/*
* builtin_pwd:
* Imprime el directorio de trabajo actual.
* Actualiza la variable de entorno PWD.
*/
int builtin_pwd(t_env *envlist)
{
    char    *current_wd;

    current_wd = getcwd(NULL, 0); 

    if (current_wd == NULL)
    {
        ft_putstr_fd("minishell: pwd: ", STDERR_FILENO);
        perror(NULL); 
        set_exit_status(EXIT_FAILURE); 
        return (1); 
    }

    ft_putstr_fd(current_wd, STDOUT_FILENO);
    ft_putchar_fd('\n', STDOUT_FILENO);

    update_env(envlist, "PWD", current_wd);

    free(current_wd); 
    set_exit_status(0);
    return (0);
}

/*
* update_pwd:
* Actualiza la variable de entorno PWD con el directorio de trabajo actual.
* Esta función es llamada por otras partes del shell, como 'cd'.
*/
char	*update_pwd(t_env *envlist)
{
	char	*current_wd;

	current_wd = getcwd(NULL, 0);
	if (!current_wd)
	{
		return (NULL); 
	}

	update_env(envlist, "PWD", current_wd);
	
	free(current_wd);

	return (NULL); 
}

/*
* exist_cwd:
* Comprueba si el directorio de trabajo actual es accesible usando getcwd.
* Devuelve:
* 1 si getcwd tiene éxito (el directorio actual existe y es accesible).
* 0 si getcwd falla.
*/
int	exist_cwd(void)
{
	char	*curr_wd;

	curr_wd = getcwd(NULL, 0);
	if (curr_wd == NULL)
		return (0); // Falla al obtener el directorio actual
	free(curr_wd); // Liberar la memoria si getcwd tuvo éxito
	return (1); // El directorio actual es accesible
}


// La función exist_cwd de jocuni no es estrictamente necesaria si manejamos
// el NULL de getcwd directamente en las funciones que lo usan.

// La función exist_cwd de jocuni no es estrictamente necesaria si manejamos
// el NULL de getcwd directamente.
// La función update_pwd de jocuni es esencialmente lo que hace parte de este
// builtin_pwd adaptado (obtener cwd y llamar a update_env).
// Mantendremos la función update_env de jocuni tal como está en su archivo original
// (la que se encuentra en builtin_pwd.c de jocuni).

/*Update env after moving to new location with cd
t_env	*update_env(t_env *env, char *key, char *val)
{
	t_env	*current;
	t_env	*new_env;

	current = env;
	while (current != NULL)
	{
		if (ft_strcmp(key, current->key) == 0)
		{
			free(current->val);
			current->val = ft_strdup(val);
			return (env);
		}
		current = current->next;
	}
	new_env = lstnew(key, val);
	lstadd_back(&env, new_env);
	return (env);
}

checks if the current working directory exists
int	exist_cwd(void)
{
	char	*curr_wd;

	curr_wd = getcwd(NULL, 0);
	if (curr_wd == NULL)
		return (0);
	free(curr_wd);
	return (1);
}

Function that returns the current path(pwd). 
Use the getcwd function to do this.
int	builtin_pwd(t_env *env)
{
	char	*current_wd;

	if (!exist_cwd())
	{
		current_wd = getenv("OLDPWD");
		printf("%s\n", current_wd);
		chdir(current_wd);
		return (1);
	}
	else
	{
		current_wd = getcwd(NULL, 0);
		*env = *update_env(env, "PWD", current_wd);
		printf("%s\n", current_wd);
		free(current_wd);
	}
	set_exit_status(0);
	return (0);
}

updates the PWD environment variable
char	*update_pwd(t_env *env)
{
	char	*current_wd;

	current_wd = getcwd(NULL, 0);
	if (!current_wd)
	{
		perror("getcwd");
		free(current_wd);
	}
	env = update_env(env, "PWD", current_wd);
	free(current_wd);
	return (current_wd);
}
*/
