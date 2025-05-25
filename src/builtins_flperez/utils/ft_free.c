/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 14:50:05 by flperez-          #+#    #+#             */
/*   Updated: 2025/03/05 14:57:24 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

/*
 * free_ptr: Libera un puntero si no es NULL.
 * Función básica de utilidad para evitar liberar NULL, aunque free(NULL) es
 * seguro por el estándar C.
 * Asegura que la función empieza en la columna 1 y usa tabs para indentar.
*/
void	free_ptr(void *ptr)
{
	if (ptr != NULL)
	{
		free(ptr);
	}
}

/*
 * free_str_tab: Libera un array de strings terminado en NULL (char **).
 * Recorre el array liberando cada string y finalmente el propio array.
*/
void	free_str_tab(char **tab)
{
	int	i;

	if (!tab)
		return ;
	i = 0;
	while (tab[i])
	{
		free_ptr(tab[i]);
		tab[i] = NULL;
		i++;
	}
	free(tab);
}

/*
 * free_data_env: Libera la memoria de la estructura t_data_env.
 * Libera los campos internos que son punteros (env, pwd, old_pwd, cmd).
*/
void	free_data_env(t_data_env *data)
{
	if (!data)
	{
		return ;
	}
	if (data->env)
	{
		free_str_tab(data->env);
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
}

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
