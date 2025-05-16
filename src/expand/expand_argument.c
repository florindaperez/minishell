/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_argument.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 17:14:05 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/06 17:14:50 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* Expande una variable de entorno (ej: $VARNAME) encontrada después de un '$'.
* Avanza el índice 'i' más allá del nombre de la variable.
*/
static bool	expand_env_var_from_dollar(t_buffer_state *b_s, const char *r_t_c,
									size_t *i, t_data_env *d_env)
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
* Decide cómo manejar un '$' encontrado.
* Incluye la lógica para $?, $$ y llama a expand_env_var_from_dollar para 
* $VARNAME.
*/
static bool	handle_current_dollar(t_buffer_state *b_s, const char *r_t_c,
									size_t *i, t_data_env *d_env)
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
		return (expand_env_var_from_dollar(b_s, r_t_c, i, d_env));
	}
	return (append_char_to_buffer(b_s, '$'));
}

/*
* Bucle principal para procesar el contenido del token.
*/
static bool	process_token_loop(const char *r_t_c, t_quote_type o_q_s,
								t_data_env *d_env, t_buffer_state *b_s)
{
	size_t	i;

	i = 0;
	while (r_t_c[i] != '\0')
	{
		if (r_t_c[i] == '$')
		{
			if (!handle_current_dollar(b_s, r_t_c, &i, d_env))
				return (false);
		}
		else if (r_t_c[i] == '\'' && o_q_s == NO_QUOTE)
		{
			if (!exp_utils_handle_sq_no_quote(b_s, r_t_c, &i))
				return (false);
		}
		else if (r_t_c[i] == '"' && o_q_s == NO_QUOTE)
		{
			if (!exp_utils_handle_dq_no_quote(b_s, r_t_c, &i, d_env))
				return (false);
		}
		else if (!append_char_to_buffer(b_s, r_t_c[i++]))
			return (false);
	}
	return (true);
}

/* 
* Función principal de expansión de argumentos 
*/
char	*expand_argument(const char *raw_token_content,
						t_quote_type original_quote_status,
						t_data_env *data_env)
{
	t_buffer_state	b_state;

	b_state.buffer = NULL;
	b_state.buf_idx = 0;
	b_state.buf_size = 0;
	if (!raw_token_content)
		return (ft_strdup(""));
	if (original_quote_status == SINGLE_QUOTE)
		return (ft_strdup(raw_token_content));
	if (!init_buffer_state(&b_state))
		return (NULL);
	if (!process_token_loop(raw_token_content, original_quote_status,
			data_env, &b_state))
	{
		free_ptr(b_state.buffer);
		return (NULL);
	}
	return (b_state.buffer);
}
