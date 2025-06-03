/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 17:49:56 by castorga          #+#    #+#             */
/*   Updated: 2024/06/07 17:26:28 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

/*Frees and sets to NULL a char *str passed as argument by reference*/
void	str_free_and_null(char **str)
{
	if (*str != NULL)
		free(*str);
	*str = NULL;
}

/*
 * free_ptr: Libera un puntero si no es NULL.
 * Función básica de utilidad para evitar liberar NULL, aunque free(NULL) es
 * seguro por el estándar C.
*/
void	free_ptr(void *ptr)
{
	if (ptr != NULL)
	{
		free(ptr);
	}
}
