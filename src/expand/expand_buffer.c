/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_buffer.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 17:54:13 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/11 17:54:16 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	resize_buffer(t_buffer_state *b_state, size_t required_add_len)
{
	char	*new_buffer;
	size_t	new_size;
	size_t	min_total_size;

	min_total_size = b_state->buf_idx + required_add_len + 1;
	if (min_total_size <= b_state->buf_size)
		return (true);
	if (b_state->buf_size == 0)
		new_size = 128;
	else
		new_size = b_state->buf_size;
	while (new_size < min_total_size)
		new_size *= 2;
	new_buffer = (char *)malloc(new_size);
	if (!new_buffer)
	{
		msg_error_cmd("buffer_utils", "malloc (resize)", strerror(errno), 1);
		return (false);
	}
	if (b_state->buffer)
		ft_memcpy(new_buffer, b_state->buffer, b_state->buf_idx);
	free_ptr(b_state->buffer);
	b_state->buffer = new_buffer;
	b_state->buf_size = new_size;
	return (true);
}

bool	init_buffer_state(t_buffer_state *b_state)
{
	b_state->buffer = (char *)malloc(128);
	if (!b_state->buffer)
	{
		msg_error_cmd("buffer_utils", "malloc (init)", strerror(errno), 1);
		return (false);
	}
	b_state->buffer[0] = '\0';
	b_state->buf_size = 128;
	b_state->buf_idx = 0;
	return (true);
}

bool	append_char_to_buffer(t_buffer_state *b_state, char c)
{
	if (!resize_buffer(b_state, 1))
		return (false);
	(b_state->buffer)[b_state->buf_idx] = c;
	b_state->buf_idx++;
	(b_state->buffer)[b_state->buf_idx] = '\0';
	return (true);
}

bool	append_string_to_buffer(t_buffer_state *b_state, const char *str)
{
	size_t	str_len;

	if (!str)
		return (true);
	str_len = ft_strlen(str);
	if (str_len == 0)
		return (true);
	if (!resize_buffer(b_state, str_len))
		return (false);
	ft_memcpy((b_state->buffer) + b_state->buf_idx, str, str_len);
	b_state->buf_idx += str_len;
	(b_state->buffer)[b_state->buf_idx] = '\0';
	return (true);
}
