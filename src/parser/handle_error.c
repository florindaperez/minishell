/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 10:57:51 by jocuni-p          #+#    #+#             */
/*   Updated: 2024/06/07 17:02:06 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* Imprime un mensaje de error de sintaxis formateado a STDERR_FILENO
* y establece el estado de salida global a 2.
*
* syntax_error_message: La cadena específica del error de sintaxis,
* generalmente una macro como PRINT_SYNTAX_ERR_1.
*/
void	handle_error(char *syntax_error_message)
{
	write(STDERR_FILENO, "minishell: ", 11);
	if (syntax_error_message)
	{
		write(STDERR_FILENO, syntax_error_message, \
			ft_strlen(syntax_error_message));
	}
	set_exit_status(2);
}

/*------FREES t_tok list AND PRINTS AN ERROR MESSAGE-----
* daba error se liberaba 2 veces tok 1. La liberacion la tiene que hacer quien 
 le llama
void	handle_error(char *str, t_tok **tok)
{
	write(2, str, ft_strlen(str));
	set_exit_status(2);
	if (tok != NULL)
		tok_free(tok);
}
*/