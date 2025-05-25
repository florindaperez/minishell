/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_exists.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: castorga <castorga@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 15:25:50 by castorga          #+#    #+#             */
/*   Updated: 2024/05/27 15:25:54 by castorga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"


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
 			return (env); // Devuelve la cabeza original
 		}
 		current = current->next;
 	}
/* Si no se encuentra, crea y añade al final*/
 	new_env = lstnew(key, val); // lstnew y lstadd_back deben manejar el caso de env == NULL
 	lstadd_back(&env, new_env); // Pasa la dirección de env para que pueda ser modificado si era NULL
 	return (env); // Devuelve la cabeza (posiblemente nueva si env era NULL)
 }
// Nota: La llamada lstadd_back(&env, new_node) sugiere que 'env' en update_env
// debería ser t_env ** si se espera que la cabeza de la lista cambie.
// Sin embargo, la firma de builtin_pwd es t_env *, lo que implica que update_env
// no debería cambiar la cabeza de la lista de forma que el puntero original
// de builtin_pwd quede invalidado, o que el valor de retorno de update_env
// no se está utilizando para reasignar 'env' en builtin_pwd.
// Para simplificar, asumiremos que update_env modifica la lista in-place
// o que su valor de retorno no es crucial para reasignar en este contexto.
// La implementación actual de update_env en el archivo de jocuni es la correcta
// para manejar la lista t_env.


/*checks if the variable oldpwd already exists in the env.*/
int	var_exists_oldpwd(t_env *env, char *variable)
{
	char	**var_ent;
	int		flag;

	flag = 0;
	var_ent = ft_split(variable, '=');
	while (env != NULL)
	{
		if (ft_strcmp(var_ent[0], env->key) == 0)
			flag = 1;
		env = env->next;
	}
	free_arr2d(var_ent);
	return (flag);
}

/*checks if the variable already exists in the env. op3*/
int	variable_exists_op3(t_env *env, char *variable)
{
	char	**var_ent;
	int		flag;

	flag = 0;
	var_ent = ft_split(variable, '=');
	while (env != NULL)
	{
		if (ft_strcmp(var_ent[0], env->key) == 0)
		{
			flag = 1;
			break ;
		}
		env = env->next;
	}
	free_arr2d(var_ent);
	return (flag);
}

/*checks if the variable already exists in the env. op4*/
t_env	*variable_exists_op4(t_env *env, char *key)
{
	while (env != NULL)
	{
		if (ft_strcmp(key, env->key) == 0)
			return (env);
		env = env->next;
	}
	return (NULL);
}

/*checks if the variable already exists in the env. and updates it,
 if applicable(case +=)*/
int	variable_exists_op2(t_env *env, char *variable)
{
	char	**var_ent1;
	char	**var_ent2;
	int		flag;
	char	*new_val;

	var_ent1 = ft_split(variable, '+');
	var_ent2 = ft_split(var_ent1[1], '=');
	flag = 0;
	while (env != NULL)
	{
		if (ft_strcmp(var_ent1[0], env->key) == 0)
		{
			new_val = ft_strjoin(env->val, var_ent2[0]);
			free(env->val);
			env->val = new_val;
			flag = 1;
			break ;
		}
		env = env->next;
	}
	free_arr2d(var_ent1);
	free_arr2d(var_ent2);
	return (flag);
}

/*checks if the variable already exists in the env. and updates it,
 if applicable(case =)*/
int	variable_exists(t_env **env, char **variable)
{
	char	**var_ent;
	int		flag;
	t_env	*current;

	var_ent = ft_split(*variable, '=');
	flag = 0;
	current = *env;
	if (var_ent == NULL || var_ent[0] == NULL)
	{
		if (var_ent)
			free_arr2d(var_ent);
		return (0);
	}
	while (current != NULL)
	{
		if (ft_strcmp(var_ent[0], current->key) == 0)
		{
			current->val = ft_strdup(variable[1]);
			flag = 1;
			break ;
		}
		current = current->next;
	}
	free_arr2d(var_ent);
	return (flag);
}
