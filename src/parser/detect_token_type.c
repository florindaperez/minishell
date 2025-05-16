/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   detect_token_type.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 19:36:48 by asans-go          #+#    #+#             */
/*   Updated: 2025/03/17 19:36:52 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h" 
#include "minishell_parser.h"

/**
 * @brief Detecta el tipo de token al inicio de la cadena 'str',
 *        priorizando operadores de dos caracteres.
 *
 * @param str La subcadena de la línea de entrada a analizar.
 * @return El t_token_type correspondiente. Devuelve TOKEN_WORD si no es
 *         un operador reconocido (esto incluye el inicio de palabras,
 *         números, variables y comillas ' o "). Devuelve TOKEN_EOF si
 *         str es NULL o vacío.
 */
t_token_type	detect_token_type(const char *str)
{
	if (str == NULL || *str == '\0')
	{
		return (TOKEN_EOF);
	}
	if (ft_strncmp(str, ">>", 2) == 0)
	{
		return (TOKEN_APPEND);
	}
	if (ft_strncmp(str, "<<", 2) == 0)
	{
		return (TOKEN_HEREDOC);
	}
	if (ft_strncmp(str, ">", 1) == 0)
	{
		return (TOKEN_REDIRECT_OUT);
	}
	if (ft_strncmp(str, "<", 1) == 0)
	{
		return (TOKEN_REDIRECT_IN);
	}
	if (ft_strncmp(str, "|", 1) == 0)
	{
		return (TOKEN_PIPE);
	}
	return (TOKEN_WORD);
}
