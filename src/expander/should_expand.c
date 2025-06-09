/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   should_expand.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 16:28:34 by jocuni-p          #+#    #+#             */
/*   Updated: 2024/06/07 16:27:31 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** expand_commands
** Descripción: Itera sobre los argumentos de un comando (`cmd->commands`).
** Si un argumento contiene '$', comillas simples o dobles, lo reemplaza
** por su versión expandida obtenida de la función `expander`.
**
** Parámetros:
** - t_cmd *cmd: Puntero al comando actual cuyos argumentos se expandirán.
** - t_env *envlist: Puntero a la lista de variables de entorno para la expans.
*/
static void	expand_commands(t_cmd *cmd, t_env *envlist)
{
	size_t	i;
	char	*exp_str;

	i = 0;
	while (cmd->commands && cmd->commands[i])
	{
		if (ft_strchr(cmd->commands[i], '$') != NULL \
			|| ft_strchr(cmd->commands[i], '\'') != NULL \
			|| ft_strchr(cmd->commands[i], '"') != NULL)
		{
			exp_str = expander(cmd->commands[i], envlist);
			free(cmd->commands[i]);
			cmd->commands[i] = exp_str;
		}
		i++;
	}
}

/*
** expand_redirections
** Descripción: Itera sobre la lista de redirecciones de un comando.
** Si el nombre de un fichero de redirección (`redir_aux->fname`) contiene
** '$', comillas simples o dobles, lo reemplaza por su versión expandida.
**
** Parámetros:
** - t_cmd *cmd: Puntero al comando actual cuyas redirecciones se expandirán.
** - t_env *envlist: Puntero a la lista de variables de entorno.
*/
static void	expand_redirections(t_cmd *cmd, t_env *envlist)
{
	t_redir	*redir_aux;
	char	*exp_str;

	redir_aux = cmd->redir;
	while (redir_aux)
	{
		if (ft_strchr(redir_aux->fname, '$') != NULL \
			|| ft_strchr(redir_aux->fname, '\'') != NULL \
			|| ft_strchr(redir_aux->fname, '"') != NULL)
		{
			exp_str = expander(redir_aux->fname, envlist);
			free(redir_aux->fname);
			redir_aux->fname = exp_str;
		}
		redir_aux = redir_aux->next;
	}
}

/*
** should_expand
** Descripción: Función principal que gestiona el proceso de expansión.
** Itera a través de una lista enlazada de comandos (conectados por pipes)
** y para cada uno, invoca funciones auxiliares para expandir tanto los
** argumentos del comando como los nombres de fichero en las redirecciones.
**
** Parámetros:
** - t_cmd *cmd: Puntero al primer comando de la lista.
** - t_env *envlist: Puntero a la lista de variables de entorno.
*/
void	should_expand(t_cmd *cmd, t_env *envlist)
{
	while (cmd)
	{
		expand_commands(cmd, envlist);
		expand_redirections(cmd, envlist);
		cmd = cmd->next;
	}
}
