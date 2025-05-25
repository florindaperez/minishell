/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 17:17:00 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/09 17:17:03 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h" 
/*
* Avanza sobre espacios iniciales y procesa un signo opcional (+ o -).
* Actualiza el índice 'i' y el multiplicador de signo 'neg'.
*/
static void	process_sign_and_spaces(const char *str, int *i, int *neg)
{
	while (str[*i] && ft_isspace(str[*i]))
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
* Convierte la secuencia de dígitos en 'str' a un unsigned long long.
* Detecta desbordamientos con respecto a LLONG_MAX/LLONG_MIN.
* Actualiza 'error' a true en caso de desbordamiento o si no hay dígitos.
*/
static unsigned long long	convert_digits(const char *str, int *i,
											int neg, bool *error)
{
	unsigned long long	num;
	unsigned long long	digit;

	num = 0;
	if (!ft_isdigit(str[*i]))
	{
		*error = true;
		return (0);
	}
	while (str[*i] && ft_isdigit(str[*i]))
	{
		digit = str[*i] - '0';
		if (neg == 1 && num > (LLONG_MAX - digit) / 10)
			*error = true;
		else if (neg == -1 && num > ((unsigned long long)LLONG_MAX + 1ULL \
					- digit) / 10)
			*error = true;
		if (*error)
			break ;
		num = (num * 10) + digit;
		(*i)++;
	}
	return (num);
}

/*
* Verifica si hay caracteres no espaciales después de los dígitos numéricos.
* Actualiza 'error' a true si se encuentran tales caracteres.
*/
static void	check_trailing_chars(const char *str, int *i, bool *error)
{
	while (str[*i] && ft_isspace(str[*i]))
		(*i)++;
	if (str[*i] != '\0')
		*error = true;
}

/*
* Convierte una cadena a long long, detectando errores y desbordamientos.
* Similar a atoll, pero con manejo de errores más explícito.
*/
long long	ft_atoll_with_error_check(const char *str, bool *error)
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
