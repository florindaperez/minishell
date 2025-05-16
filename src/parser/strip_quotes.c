/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strip_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 11:50:38 by flperez-          #+#    #+#             */
/*   Updated: 2025/04/29 11:50:41 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_parser.h" 

/*
* Procesa el contenido dentro de un par de comillas específicas.
*
* Avanza los índices 'i' (para raw_content) y 'j' (para result)
* mientras copia caracteres desde raw_content a result, hasta que
* se encuentra la comilla de cierre 'quote_char' o se alcanza el final
* de la cadena. Omite las comillas de apertura y cierre.
*/
static void	process_quoted_content(struct s_strip_q_data *data, size_t *i,
									size_t *j, char quote_char)
{
	(*i)++;
	while (*i < data->len && data->raw[*i] != quote_char)
	{
		data->res[*j] = data->raw[*i];
		(*j)++;
		(*i)++;
	}
	if (*i < data->len && data->raw[*i] == quote_char)
		(*i)++;
}

/*
*  Bucle principal para procesar todo el contenido de la cadena.
*
* Itera sobre la cadena original, manejando caracteres normales y
* delegando el contenido entrecomillado a process_quoted_content.
* Inicializa y usa los índices i y j para la iteración.
* Termina la cadena resultante con '\0'.
*/
static void	process_all_content_loop(struct s_strip_q_data *data)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (i < data->len)
	{
		if (data->raw[i] == '\'')
			process_quoted_content(data, &i, &j, '\'');
		else if (data->raw[i] == '"')
			process_quoted_content(data, &i, &j, '"');
		else
		{
			data->res[j] = data->raw[i];
			j++;
			i++;
		}
	}
	data->res[j] = '\0';
}

/*
* Elimina las comillas estructurales (' o ") de una cadena.
*
* Esta función toma una cadena que puede contener comillas simples o dobles
* que delimitan partes de la cadena. Elimina estas comillas delimitadoras
* y devuelve una nueva cadena con el contenido "desnudo".
* Por ejemplo, "'hola'" se convierte en "hola", y "\"mundo\"" en "mundo".
* Caracteres fuera de comillas se copian tal cual.
* Asume que las comillas en la entrada están balanceadas.
*/
char	*strip_quotes_from_arg(const char *raw_token_content)
{
	struct s_strip_q_data	strip_data;

	if (!raw_token_content)
		return (NULL);
	strip_data.len = ft_strlen(raw_token_content);
	strip_data.raw = raw_token_content;
	strip_data.res = (char *)malloc(sizeof(char) * (strip_data.len + 1));
	if (!strip_data.res)
	{
		msg_error_cmd("strip_quotes", "malloc failed", strerror(errno), 1);
		return (NULL);
	}
	process_all_content_loop(&strip_data);
	return (strip_data.res);
}
