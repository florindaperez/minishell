/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 15:57:33 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/10 15:57:36 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * is_parent_builtin
 * Comprueba si un comando dado es un built-in que debe ejecutarse en el
 * proceso padre para afectar al entorno de la shell principal.
 *
 * cmd: La estructura del comando del ejecutor que se va a comprobar.
 *
 * Retorna: 'true' si el comando es 'cd', 'exit', 'unset', o 'export' con
 * argumentos. 'false' en caso contrario.
 */
bool	is_parent_builtin(t_cmd_exe *cmd)
{
	char	*cmd_name;

	if (!cmd || !cmd->is_builtin || !cmd->args || !cmd->args[0])
		return (false);
	cmd_name = cmd->args[0]->value;
	if (!cmd_name)
		return (false);
	if (ft_strcmp(cmd_name, "cd") == 0)
		return (true);
	if (ft_strcmp(cmd_name, "exit") == 0)
		return (true);
	if (ft_strcmp(cmd_name, "unset") == 0)
		return (true);
	if (ft_strcmp(cmd_name, "export") == 0 && cmd->args[1] != NULL)
		return (true);
	return (false);
}

/*
 * setup_parent_builtin_env
 * Prepara el entorno para la ejecución de un built-in del proceso padre.
 * Gestiona las redirecciones de E/S y construye el array de argumentos (argv).
 *
 * cmd:                 El comando a ejecutar.
 * argv_ptr:            Puntero a un (char **) donde se almacenará el array
 * de argumentos construido.
 * io_redirected_flag:  Puntero a un booleano que se establecerá a 'true' si se
 * realizó alguna redirección de E/S.
 *
 * Retorna: 'true' si la preparación fue exitosa, 'false' si ocurrió un error
 * durante la redirección o la construcción de los argumentos.
 */
static bool	setup_parent_builtin_env(t_cmd_exe *cmd, char ***argv_ptr,
									bool *io_redirected_flag)
{
	*io_redirected_flag = false;
	if (cmd && cmd->io
		&& (cmd->io->infile || cmd->io->outfile
			|| cmd->io->heredoc_delimiter))
	{
		*io_redirected_flag = true;
		if (!redirect_io(cmd))
		{
			restore_io(cmd->io);
			return (false);
		}
	}
	*argv_ptr = build_argv_from_args(cmd->args);
	if (!*argv_ptr)
	{
		if (*io_redirected_flag)
			restore_io(cmd->io);
		return (false);
	}
	return (true);
}

/*
 * execute_parent_builtin
 * Orquesta la ejecución de un built-in que se ejecuta en el proceso padre.
 * Maneja la configuración, la ejecución y la restauración del entorno.
 *
 * cmd:   El nodo del comando que contiene el built-in a ejecutar.
 * data:  Puntero a la estructura de datos principal del ejecutor, que
 * contiene el estado del entorno de la shell.
 *
 * Retorna: El código de salida (exit status) del built-in ejecutado.
 */
int	execute_parent_builtin(t_cmd_exe *cmd, t_data_env_exe *data)
{
	int		result;
	char	**argv_for_builtin;
	bool	io_redirected;

	result = EXIT_SUCCESS;
	argv_for_builtin = NULL;
	if (!setup_parent_builtin_env(cmd, &argv_for_builtin, &io_redirected))
	{
		if (argv_for_builtin)
			free_str_tab(argv_for_builtin);
		return (g_get_signal);
	}
	if (!argv_for_builtin[0] && !cmd->redir_error)
		result = EXIT_SUCCESS;
	else if (!argv_for_builtin[0] && cmd->redir_error)
		result = g_get_signal;
	else if (argv_for_builtin[0])
		result = execute_builtin(cmd, data, argv_for_builtin);
	free_str_tab(argv_for_builtin);
	if (io_redirected)
	{
		if (!restore_io(cmd->io))
			perror("minishell: parent_builtin: restore_io failed");
	}
	return (result);
}

/*
 * execute_builtin
 * Actúa como un despachador (dispatcher) para todos los built-ins. Basado en
 * el nombre del comando, llama a la función de implementación específica.
 *
 * cmd_node: El nodo completo del comando (usado para contexto si es necesario).
 * data:     La estructura de datos del ejecutor, pasada a los built-ins que
 * modifican o leen el entorno.
 * args:     El array de argumentos (argv) para el built-in.
 *
 * Retorna: El código de salida devuelto por la función del built-in que fue
 * ejecutada, o 127 si el comando no es un built-in reconocido.
 */
int	execute_builtin(t_cmd_exe *cmd_node, t_data_env_exe *data, char **args)
{
	if (!args || !args[0])
		return (1);
	if (ft_strcmp(args[0], "echo") == 0)
		return (builtin_echo(args));
	if (ft_strcmp(args[0], "cd") == 0)
		return (builtin_cd(args, data));
	if (ft_strcmp(args[0], "pwd") == 0)
		return (builtin_pwd(data));
	if (ft_strcmp(args[0], "export") == 0)
		return (builtin_export(args, data));
	if (ft_strcmp(args[0], "unset") == 0)
		return (builtin_unset(args, data));
	if (ft_strcmp(args[0], "env") == 0)
		return (builtin_env(data, args));
	if (ft_strcmp(args[0], "exit") == 0)
		return (builtin_exit(args, data));
	(void)cmd_node;
	return (127);
}
