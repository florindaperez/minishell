/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message_error.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 15:28:49 by flperez-          #+#    #+#             */
/*   Updated: 2025/03/05 15:29:06 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * merge_msg: Concatenamos las dos cadenas de "str" y "add"
 * y devolvemos la nueva cadena resultante.
 *
*/

static char	*merge_msg(char *str, char *add)
{
	char	*tmp;

	if (!add)
		return (str);
	if (!str)
		return (ft_strdup(add));
	tmp = str;
	str = ft_strjoin(tmp, add);
	free_ptr(tmp);
	return (str);
}

/*
 * msg_export_unset: "export" & "unset" w con variables de entorno. Se deben 
 * seguir ciertas normas: no pueden comenzar por numero o contener caracteres
 * especiales (@, !, -, etc.). Da un mensaje de error especifico que hemos de 
 * replicar: EJEMPLO ==> export 1var=value obtenemos
 * "bash: export: `1var=value': not a valid identifier"
 * EJEMPLO ==> unset ?
 * "bash: unset: `?': not a valid identifier"
 * 
 */

static bool	msg_export_unset(char *arg_cmd)
{
	if (!arg_cmd)
		return (false);
	if (ft_strncmp(arg_cmd, "export", 7) == 0
		|| ft_strncmp(arg_cmd, "unset", 6) == 0)
		return (true);
	return (false);
}

/*
 * msg_error_cmd : Mostramos el mensaje de error. Simula mensajes de minishell:
 * Si el comando es  "export" o "unset", se añaden comillas invertidas alrededor
 * "descrip", imprime el mensaje STDERR_FILENO, actualiza g_get_signal y
 * devolvemos "nb_err" ,codigo del error.
 *
*/

int	msg_error_cmd(char *arg_cmd, char *descrip, char *err_msg, int nb_err)
{
	char	*msg;
	bool	descrip_export_unset;

	descrip_export_unset = msg_export_unset(arg_cmd);
	msg = ft_strdup("minishell: ");
	if (arg_cmd != NULL)
	{
		msg = merge_msg(msg, arg_cmd);
		msg = merge_msg(msg, ": ");
	}
	if (descrip != NULL)
	{
		if (descrip_export_unset)
			msg = merge_msg(msg, "`");
		msg = merge_msg(msg, descrip);
		if (msg_export_unset(arg_cmd))
			msg = merge_msg(msg, "'");
		msg = merge_msg(msg, ": ");
	}
	msg = merge_msg(msg, err_msg);
	ft_putendl_fd(msg, STDERR_FILENO);
	free_ptr(msg);
	set_exit_status(nb_err);
	return (nb_err);
}
