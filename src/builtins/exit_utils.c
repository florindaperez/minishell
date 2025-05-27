/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 10:22:09 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/27 10:22:18 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
*	process_sign_and_spaces:
*	Avanza el puntero de índice 'i' sobre cualquier espacio en blanco inicial
*	en la cadena 'str'. Luego, detecta y procesa un signo opcional ('+' o '-').
*	Si se encuentra un '-', actualiza la variable 'neg' a -1.
*	Modifica 'i' y 'neg' por referencia.
*/
static void	process_sign_and_spaces(const char *str, int *i, int *neg)
{
	if (!str)
		return ;
	while (str[*i] && isspace((unsigned char)str[*i]))
		(*i)++;
	if (str[*i] == '+')
		(*i)++;
	else if (str[*i] == '-')
	{
		*neg = -1;
		(*i)++;
	}
}

/*
*	check_overflow:
*	Comprueba si añadir el 'digit' actual al 'num' acumulado causaría un
*	desbordamiento (overflow) para un tipo long long, considerando el signo 
*	'neg'.
*	Devuelve true si hay riesgo de desbordamiento, false en caso contrario.
*/
static bool	check_overflow(unsigned long long num, unsigned long long digit,
							int neg)
{
	if (neg == 1 && num > (LLONG_MAX - digit) / 10)
		return (true);
	else if (neg == -1 && num > ((unsigned long long)LLONG_MAX + 1ULL - digit)
		/ 10)
		return (true);
	return (false);
}

/*
*	convert_digits:
*	Convierte una secuencia de dígitos en la cadena 'str', comenzando desde el
*	índice 'i', a un valor unsigned long long. Se detiene al encontrar un
*	no dígito o el final de la cadena.
*	Utiliza 'check_overflow' para detectar desbordamientos durante la conversión.
*	Si ocurre un error (no hay dígitos válidos o hay desbordamiento),
*	establece '*error' a true. Modifica 'i' y 'error' por referencia.
*	Devuelve el número convertido o 0 si hay error.
*/
static unsigned long long	convert_digits(const char *str, int *i, int neg,
											bool *error)
{
	unsigned long long	num;
	unsigned long long	digit;

	num = 0;
	if (!str || !str[*i] || !ft_isdigit(str[*i]))
	{
		*error = true;
		return (0);
	}
	while (str[*i] && ft_isdigit(str[*i]))
	{
		digit = str[*i] - '0';
		if (check_overflow(num, digit, neg))
		{
			*error = true;
			break ;
		}
		num = (num * 10) + digit;
		(*i)++;
	}
	return (num);
}

/*
*	check_trailing_chars:
*	Verifica si hay caracteres no espaciales residuales en la cadena 'str'
*	después de la parte numérica, comenzando desde el índice 'i'.
*	Avanza 'i' sobre cualquier espacio en blanco. Si después de los espacios
*	encuentra cualquier carácter antes del final de la cadena, establece
*	'*error' a true. Modifica 'i' y 'error' por referencia.
*/
static void	check_trailing_chars(const char *str, int *i, bool *error)
{
	if (!str)
		return ;
	while (str[*i] && isspace((unsigned char)str[*i]))
		(*i)++;
	if (str[*i] != '\0')
		*error = true;
}

/*
*	ft_atoll_for_exit:
*	Convierte una cadena 'str' a un long long, similar a atoll, pero con
*	detección de errores más robusta, específicamente para el comando 'exit'.
*	Maneja espacios iniciales, signo, conversión de dígitos, desbordamiento
*	(overflow) y caracteres residuales después del número.
*	Establece el puntero '*error' a true si ocurre cualquier error durante
*	la conversión (formato inválido, desbordamiento, etc.).
*	Devuelve el número long long convertido, o 0 si hay error.
*/
long long	ft_atoll_for_exit(const char *str, bool *error)
{
	unsigned long long	num_abs;
	int					sign;
	int					i;

	sign = 1;
	i = 0;
	*error = false;
	process_sign_and_spaces(str, &i, &sign);
	num_abs = convert_digits(str, &i, sign, error);
	if (*error)
		return (0);
	check_trailing_chars(str, &i, error);
	if (*error)
		return (0);
	if (sign == 1 && num_abs > (unsigned long long)LLONG_MAX)
		*error = true;
	else if (sign == -1 && num_abs > (unsigned long long)LLONG_MAX + 1ULL)
		*error = true;
	if (*error)
		return (0);
	return ((long long)num_abs * sign);
}
