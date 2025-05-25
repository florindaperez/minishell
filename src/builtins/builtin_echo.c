/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flperez- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 12:47:21 by flperez-          #+#    #+#             */
/*   Updated: 2025/05/23 12:47:48 by flperez-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
* Comprueba si el argumento es una opción -n válida (ej: "-n", "-nnn").
*/
static bool value_n_flag(char *arg)
{
    int     i;
    bool    n_flag;

    n_flag = false;
    i = 0;
    if (!arg || arg[i] != '-')
        return (n_flag);
    i++;
    while (arg[i] && arg[i] == 'n')
        i++;
    if (arg[i] == '\0')
        n_flag = true;
    return (n_flag);
}

/*
* Imprime los argumentos de echo, manejando el flag -n.
*/
static void print_args_echo(char **args, bool n_flag, int start_index)
{
    if (!args[start_index])
    {
        if (!n_flag)
            ft_putchar_fd('\n', STDOUT_FILENO);
        return;
    }

    while (args[start_index])
    {
        ft_putstr_fd(args[start_index], STDOUT_FILENO);
        if (args[start_index + 1])
            ft_putchar_fd(' ', STDOUT_FILENO);
        start_index++;
    }

    if (!n_flag)
        ft_putchar_fd('\n', STDOUT_FILENO);
}

/*
* Maneja el caso especial 'echo ~', imprimiendo el directorio HOME.
* Devuelve true si el caso fue manejado (y echo debe terminar), false en caso contrario.
*/
static bool handle_echo_tilde(char **args, t_env *envlist)
{
    char *home_path;

    // Verifica si el primer argumento después de 'echo' es "~"
    // y si no hay más argumentos después de "~" (echo ~ solo, no echo ~ algo_mas)
    if (args[1] && ft_strcmp(args[1], "~") == 0 && args[2] == NULL)
    {
        // Intenta obtener HOME de la lista de entorno de jocuni
        home_path = get_env_val("HOME", envlist); //
        if (home_path)
        {
            ft_putstr_fd(home_path, STDOUT_FILENO);
            ft_putchar_fd('\n', STDOUT_FILENO);
            // No necesitamos liberar home_path aquí si get_env_val devuelve
            // un puntero al valor existente en la lista o una copia que
            // no debemos liberar nosotros (depende de la implementación de get_env_val).
            // La versión de jocuni de get_env_val hace un ft_strdup, así que SÍ hay que liberarlo.
             free(home_path); // Importante si get_env_val hace strdup
        }
        else
        {
            // Si HOME no está seteado, 'echo ~' usualmente imprime '~' literalmente
            // o una línea vacía seguida de newline, dependiendo del shell.
            // Para replicar el `getenv("HOME")` del jocuni original que podría devolver NULL:
            // si getenv devuelve NULL, printf("%s\n", NULL) es comportamiento indefinido.
            // Un comportamiento más seguro si HOME no existe es imprimir '~' o nada.
            // Aquí optaremos por imprimir '~' literalmente si HOME no se encuentra,
            // lo que significa que este 'if' no se cumpliría y caería en la lógica general.
            // O, si queremos que imprima una línea vacía:
            ft_putchar_fd('\n', STDOUT_FILENO);
        }
        return (true); // Indica que 'echo ~' fue manejado
    }
    return (false); // '~' no fue el único argumento o no fue el primer argumento.
}

/*
* Réplica del comando "echo" para la estructura de minishell_jocuni.
*/
int builtin_echo(t_cmd *cmd, t_env *envlist) // Añadido t_env *envlist
{
    int     i;
    bool    n_flag;
    char    **args;

    args = cmd->commands;
    n_flag = false;
    i = 1;

    // Manejar 'echo' sin argumentos
    if (args[1] == NULL)
    {
        ft_putchar_fd('\n', STDOUT_FILENO);
        set_exit_status(0);
        return (0);
    }

    // Manejar 'echo ~' (si es el único argumento después de echo)
    // Pasamos envlist para poder obtener la variable HOME
    if (handle_echo_tilde(args, envlist))
    {
        set_exit_status(0);
        return (0); // Termina aquí si 'echo ~' fue manejado
    }

    // Procesar opciones -n
    while (args[i] && value_n_flag(args[i]))
    {
        n_flag = true;
        i++;
    }

    print_args_echo(args, n_flag, i);

    set_exit_status(0);
    return (0);
}

/* echo ~ option 
static int	echo_options(t_cmd *cmd, int i, int *print_newline)
{
	if (ft_strcmp(cmd->commands[1], "~") == 0)
	{
		printf("%s\n", getenv("HOME"));
		*print_newline = 0;
		return (0);
	}
	while (cmd->commands[i])
	{
		printf("%s", cmd->commands[i]);
		if (cmd->commands[i + 1] && cmd->commands[i][0] != '\0')
			printf(" ");
		i++;
	}
	return (0);
}

builtin echo 
int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	print_newline;

	i = 1;
	print_newline = 1;
	if (size_arr2d(cmd->commands) == 1)
	{
		printf("\n");
		set_exit_status(0);
		return (0);
	}
	if (ft_strcmp(cmd->commands[1], "-n") == 0)
	{
		print_newline = 0;
		i++;
	}
	echo_options(cmd, i, &print_newline);
	if (print_newline)
		printf("\n");
	set_exit_status(0);
	return (0);
}
*/
