/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocuni-p <jocuni-p@student.42barcelona.com +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 17:55:12 by castorga          #+#    #+#             */
/*   Updated: 2024/06/07 16:59:38 by jocuni-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_executor.h"

/*
 * g_get_signal: Variable global. Almacena el estado de salida del último
 * comando ejecutado o el código de una señal. Es esencial para el
 * funcionamiento de $? y la gestión de señales en todo el shell.
 * Su uso global está justificado por la necesidad de acceso y modificación
 * desde diversas partes del programa, incluyendo manejadores de señales
 * y el bucle principal de ejecución de comandos.
 */
int	g_get_signal = 0;

/*
 * Prototipo para la función que procesa una única interacción del usuario.
 * Esta función se encuentra definida en  minishell.c y es el núcleo 
 * del bucle interactivo.
 */
void	minishell(t_env **envlist_head_ptr);

/*
 * main: Punto de entrada del programa minishell.
 * Configura el entorno inicial y entra en un bucle para procesar
 * los comandos introducidos por el usuario en modo interactivo.
 * Argumentos:
 * ac: Contador de argumentos (debe ser 1 para modo interactivo).
 * av: Vector de argumentos (no se utiliza en modo interactivo).
 * envp: Puntero a las variables de entorno del sistema.
 * Retorna:
 * 0 si la salida es controlada (aunque típicamente el programa
 * termina mediante una llamada a exit() desde otra parte).
 * EXIT_FAILURE si los argumentos iniciales son incorrectos.
 */
int	main(int ac, char *av[], char *envp[])
{
	t_env	*envlist_head;

	envlist_head = NULL;
	if (ac != 1)
	{
		write(STDERR_FILENO, "Uso: ./minishell\n", 17);
		exit(EXIT_FAILURE);
	}
	(void)av;
	env_init_list(envp, &envlist_head);
	while (1)
	{
		signals_interactive();
		minishell(&envlist_head);
	}
	return (0);
}

/*
int	g_get_signal = 0;
//set_signals(PARENT) => signals_interactive();

int	main(int ac, char *av[], char *envp[])
{
	t_env	*envlist;

	(void)av;
	envlist = NULL;
	if (ac != 1)
	{
		ft_msgs(10, NULL);
		exit(EXIT_FAILURE);
	}
	env_init_list(envp, &envlist);
	while (1)
	{
		signals_interactive();
		minishell(envlist);
	}
	return (0);
}
*/
