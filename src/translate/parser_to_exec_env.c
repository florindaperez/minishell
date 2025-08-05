/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_to_exec_env.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:01:54 by flperez-          #+#    #+#             */
/*   Updated: 2025/06/02 20:02:10 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * Crea una cadena con el formato "KEY=VALUE" a partir de un nodo de variable
 * de entorno del formato del parser (t_env).
 *
 * @param env_node Puntero al nodo t_env que contiene la clave y el valor.
 * @return Una nueva cadena (char *) alocada con el formato "KEY=VALUE",
 * o NULL si falla la asignación o la entrada es inválida.
 */
static	char	*assemble_env_string(t_env *env_node)
{
	char	*key_equals_sign;
	char	*env_str;

	if (!env_node || !env_node->key || !env_node->val)
		return (NULL);
	key_equals_sign = ft_strjoin(env_node->key, "=");
	if (!key_equals_sign)
		return (NULL);
	env_str = ft_strjoin(key_equals_sign, env_node->val);
	free(key_equals_sign);
	if (!env_str)
		return (NULL);
	return (env_str);
}

/*
 * Cuenta el número de variables de entorno en la lista que tienen un valor.
 *
 * @param env_list Puntero a la cabeza de la lista t_env.
 * @return El número de variables de entorno con valor.
 */
static size_t	count_env_vars_with_value(struct s_env *env_list)
{
	size_t	count;
	t_env	*current_env_node;

	count = 0;
	current_env_node = env_list;
	while (current_env_node)
	{
		if (current_env_node->val)
			count++;
		current_env_node = current_env_node->next;
	}
	return (count);
}

/*
 * Rellena el array 'envp_array' con cadenas de variables de entorno "KEY=VALUE".
 * Itera sobre 'env_list' hasta 'count' elementos válidos.
 * En caso de fallo de asignación para una cadena, libera 'envp_array'
 * y retorna 'false'. Si todo tiene éxito, el array es terminado en NULL
 * y retorna 'true'.
 *
 * @param envp_array Array de strings a rellenar.
 * @param env_list   Lista enlazada de variables de entorno (formato t_env).
 * @param count      Número de variables de entorno (con valor) a procesar.
 * @return           'true' si tiene éxito, 'false' si falla la asignación.
 */
static bool	fill_envp_array(char **envp_array, t_env *env_list,
	size_t count)
{
	size_t	i;
	t_env	*current_node;

	i = 0;
	current_node = env_list;
	while (i < count && current_node)
	{
		if (current_node->key && current_node->val)
		{
			envp_array[i] = assemble_env_string(current_node);
			if (!envp_array[i])
			{
				free_arr2d(envp_array);
				return (false);
			}
			i++;
		}
		current_node = current_node->next;
	}
	envp_array[i] = NULL;
	return (true);
}

/*
 * Convierte la lista enlazada de variables de entorno del formato del parser
 * (t_env) a un array de strings (char **) terminado en NULL, adecuado para
 * ser pasado como argumento `envp` a `execve`.
 *
 * @param env_list Puntero a la lista t_env.
 * @return Un array de strings (char **) con las variables de entorno,
 * o NULL si falla la asignación o la lista está vacía.
 */
char	**convert_env_list_to_exec_envp(struct s_env *env_list)
{
	size_t	count;
	char	**envp_array;

	count = count_env_vars_with_value(env_list);
	envp_array = (char **)p_malloc(sizeof(char *) * (count + 1));
	if (!envp_array)
		return (NULL);
	if (!fill_envp_array(envp_array, env_list, count))
		return (NULL);
	return (envp_array);
}
