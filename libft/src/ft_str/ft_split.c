/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <asans-go@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 18:23:47 by asans-go          #+#    #+#             */
/*   Updated: 2024/03/27 19:37:49 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/libft.h"

static int	count_words(const char *s, char c)
{
	int	words;
	int	flag;

	words = 0;
	flag = 0;
	while (*s)
	{
		if (*s != c && flag == 0)
		{
			flag = 1;
			words++;
		}
		else if (*s == c)
			flag = 0;
		s++;
	}
	return (words);
}

static char	*word_dup(const char *s, int start, int end)
{
	char	*word;
	int		i;

	i = 0;
	word = malloc((end - start + 1) * sizeof(char));
	if (!word)
	{
		return (NULL);
	}
	while (start < end)
	{
		word[i++] = s[start++];
	}
	word[i] = '\0';
	return (word);
}

static char	**free_array(char **arr, size_t k)
{
	while (k-- > 0)
	{
		free(arr[k]);
		arr[k] = NULL;
	}
	free(arr);
	arr = NULL;
	return (NULL);
}

static char	**result_array(size_t size, char **res_arr, char const *s, char c)
{
	size_t	i;
	size_t	start;
	size_t	end;

	i = 0;
	start = 0;
	end = 0;
	while (i < size)
	{
		while (s[start] == c)
			start++;
		end = start;
		while (s[end] && s[end] != c)
			end++;
		res_arr[i] = word_dup(s, start, end);
		if (!res_arr[i])
			return (free_array(res_arr, i));
		start = end;
		i++;
	}
	res_arr[i] = NULL;
	return (res_arr);
}

char	**ft_split(char const *s, char c)
{
	char		**result;
	size_t		size;

	size = count_words(s, c);
	if (!s)
		return (NULL);
	result = (char **)malloc(sizeof(char *) * (size + 1));
	if (!result)
		return (NULL);
	result = result_array(size, result, s, c);
	if (!result)
		return (NULL);
	return (result);
}
