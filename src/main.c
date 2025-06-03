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
// #include "minishell_executor.h" // No es necesario incluirlo aquí directamente

/*
* Variable global para almacenar el último estado de salida. Se usa
* principalmente para el manejo de señales y para que el estado sea
* accesible entre diferentes partes del shell.
*/
int g_get_signal = 0; // Se define aquí, como punto de entrada del programa.

/*
* int   main(int ac, char *av[], char *envp[])
* --------------------------------------------
* Punto de entrada principal de minishell.
* Inicializa el entorno y llama al bucle interactivo.
*/
int main(int ac, char *av[], char *envp[])
{
    t_env   *envlist;

    (void)av; // av no se utiliza en el modo interactivo

    envlist = NULL;
    signals_interactive(); // Configuración inicial de las señales

    // Solo se permite el modo interactivo (sin argumentos adicionales)
    if (ac != 1)
    {
        write(STDERR_FILENO, "minishell: incorrect arguments\n", 31);
        // Asumiendo que set_exit_status actualiza g_get_signal o directamente:
        // g_get_signal = EXIT_FAILURE;
        set_exit_status(EXIT_FAILURE); // Asegúrate de que set_exit_status esté definida.
        cleaner_envlist(&envlist); // Limpia por si acaso ya se había inicializado
        return (EXIT_FAILURE);
    }

    env_init_list(envp, &envlist); // Inicializa la lista de variables de entorno

    // Llama a la función principal del bucle interactivo que está en minishell.c
    minishell(envlist);

    cleaner_envlist(&envlist); // Limpia el entorno antes de salir
    return (g_get_signal);     // Devuelve el último estado de salida
}
