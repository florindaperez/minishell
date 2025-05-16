/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 18:31:13 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/11 18:31:16 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* Función auxiliar estática para extraer y anadir el valor de una variable de 
* entorno.
* Es llamada por exp_utils_dollar_expansion_in_dq.
*/
static bool	extract_and_append_env_var(t_buffer_state *b_s,
										const char *r_t_c,
										size_t *i,
										t_data_env *d_env)
{
	char	var_name_buffer[MAX_TOKEN_LENGTH];
	size_t	var_name_idx;
	char	*var_value;

	var_name_idx = 0;
	while (r_t_c[*i] && (ft_isalnum(r_t_c[*i]) || r_t_c[*i] == '_'))
	{
		if (var_name_idx < MAX_TOKEN_LENGTH - 1)
			var_name_buffer[var_name_idx++] = r_t_c[*i];
		else
			break ;
		(*i)++;
	}
	var_name_buffer[var_name_idx] = '\0';
	var_value = get_env_var_val(d_env->env, var_name_buffer);
	if (var_value)
	{
		if (!append_string_to_buffer(b_s, var_value))
			return (false);
	}
	return (true);
}

/*
* Maneja la expansión de variables ($?, $$, $VAR) específicamente dentro
* de una cadena entrecomillada doble (cuando original_quote_status es NO_QUOTE
* y se encuentra un '"').
* Es una función auxiliar para exp_utils_handle_dq_no_quote.
*/
static bool	exp_utils_dollar_expansion_in_dq(t_buffer_state *b_s,
											const char *r_t_c,
											size_t *i,
											t_data_env *d_env)
{
	char	str_val_special[12];

	(*i)++;
	if (r_t_c[*i] == '?')
	{
		snprintf(str_val_special, sizeof(str_val_special), "%d", g_exit_status);
		(*i)++;
		return (append_string_to_buffer(b_s, str_val_special));
	}
	else if (r_t_c[*i] == '$')
	{
		(*i)++;
		return (append_char_to_buffer(b_s, '$'));
	}
	else if (ft_isalpha(r_t_c[*i]) || r_t_c[*i] == '_')
	{
		return (extract_and_append_env_var(b_s, r_t_c, i, d_env));
	}
	return (append_char_to_buffer(b_s, '$'));
}

/* 
* Procesa un segmento entre comillas simples encontrado en modo NO_QUOTE 
*/
bool	exp_utils_handle_sq_no_quote(t_buffer_state *b_s,
										const char *raw_token_content,
										size_t *i)
{
	(*i)++;
	while (raw_token_content[*i] != '\0' && raw_token_content[*i] != '\'')
	{
		if (!append_char_to_buffer(b_s, raw_token_content[*i]))
			return (false);
		(*i)++;
	}
	if (raw_token_content[*i] == '\'')
		(*i)++;
	return (true);
}

/* 
* Procesa un segmento entre comillas dobles encontrado en modo NO_QUOTE 
*/
bool	exp_utils_handle_dq_no_quote(t_buffer_state *b_s,
				const char *raw_token_content, size_t *i, t_data_env *d_env)
{
	(*i)++;
	while (raw_token_content[*i] != '\0' && raw_token_content[*i] != '"')
	{
		if (raw_token_content[*i] == '$')
		{
			if (!exp_utils_dollar_expansion_in_dq(b_s, raw_token_content,
					i, d_env))
				return (false);
		}
		else
		{
			if (!append_char_to_buffer(b_s, raw_token_content[*i]))
				return (false);
			(*i)++;
		}
	}
	if (raw_token_content[*i] == '"')
		(*i)++;
	return (true);
}
