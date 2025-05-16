/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asans-go <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:30:17 by asans-go          #+#    #+#             */
/*   Updated: 2025/03/17 18:33:20 by asans-go         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Definición de la variable global para el estado de salida.
 * Esta es la ÚNICA definición; en minishell.h es 'extern int g_exit_status;'.
 */
int	g_exit_status = 0;

/*
 * Procesa una única línea de comando (lexing, parsing, ejecución).
 * Libera tokens y comandos si se generan.
 */
void	process_command_line(char *line, t_data_env *data)
{
	t_token	*tokens_from_lexer;
	t_cmd	*cmd_list;

	tokens_from_lexer = lexer(line);
	if (tokens_from_lexer)
	{
		cmd_list = parse_tokens(tokens_from_lexer, data);
		if (cmd_list)
		{
			execute_pipeline(cmd_list, data);
			free_commands(cmd_list);
		}
		free_tokens(tokens_from_lexer);
	}
}

/*
 * Ejecuta minishell en modo no interactivo (con la opción -c).
 */
static int	run_non_interactive(char **argv, t_data_env *data)
{
	char	*line;

	signals_noninteractive();
	line = argv[2];
	if (!line)
	{
		ft_putendl_fd("minishell: -c: option requires an argument", \
			STDERR_FILENO);
		g_exit_status = 2;
		return (g_exit_status);
	}
	process_command_line(line, data);
	return (g_exit_status);
}

/*
 * Bucle principal para el modo interactivo de minishell.
 */
static int	run_interactive_loop(t_data_env *data)
{
	char	*line;

	while (1)
	{
		signals_interactive();
		line = readline("minishell> ");
		if (!line)
		{
			ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		if (line && *line)
		{
			add_history(line);
			process_command_line(line, data);
		}
		free_ptr(line);
		line = NULL;
	}
	return (g_exit_status);
}

/*
 * Función principal de minishell.
 * Maneja argumentos, inicializa datos y llama al modo apropiado.
 */
int	main(int argc, char **argv, char **envp)
{
	t_data_env	data;

	if (!start_data(&data, envp))
	{
		ft_putendl_fd("Error fatal: Fallo en inicialización de datos.", \
			STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	if (argc >= 3 && ft_strcmp(argv[1], "-c") == 0)
	{
		g_exit_status = run_non_interactive(argv, &data);
	}
	else if (argc == 1)
	{
		g_exit_status = run_interactive_loop(&data);
	}
	else
	{
		ft_putendl_fd("minishell: Uso: ./minishell [-c \"comando\"]", \
			STDERR_FILENO);
		g_exit_status = 2;
	}
	free_data_env(&data);
	return (g_exit_status);
}
