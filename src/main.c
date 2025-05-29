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
/*
#include "minishell.h"

int	g_get_signal = 0;

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
		set_signals(PARENT);
		minishell(envlist);
	}
	return (0);
}
*/
#include "minishell.h"         // Primero, para definir t_cmd, t_env, etc.
#include "minishell_executor.h" // Luego, para usar t_cmd en sus prototipos y definir t_cmd_exe, etc.

// Variable global de tu proyecto
int g_get_signal = 0;

int main(int ac, char *av[], char *envp[])
{
    t_env   *envlist_bonus = NULL; // Tu lista de entorno de bonus
    // Variables para -c mode
    char    **arg_input_lines;
    int     i;

    // Nueva estructura para el ejecutor de flperez
    t_data_env_exe data_flperez;

    // Configuración inicial de señales para el modo interactivo
    signals_interactive(); // <--- CAMBIO: Llamada inicial a signals_interactive()

    env_init_list(envp, &envlist_bonus); // Inicializa tu t_env de bonus

    if (ac == 3 && ft_strcmp(av[1], "-c") == 0 && av[2] && *av[2])
    {
        // set_signals(PARENT); // <--- ELIMINADO: Ya no se usa el antiguo sistema

        arg_input_lines = ft_split(av[2], ';');
        if (!arg_input_lines)
        {
            write(2, "minishell: Error: ft_split failed\n", 34);
            cleaner_envlist(&envlist_bonus);
            exit(EXIT_FAILURE);
        }

        i = 0;
        while (arg_input_lines[i])
        {
            char *current_command_line = arg_input_lines[i];
            t_tok *token_list_bonus = NULL;
            t_cmd *cmd_list_bonus = NULL;

            tokenizer(&token_list_bonus, current_command_line);

            if (token_list_bonus == NULL) {
                i++;
                continue;
            }
            
            if (parser(&cmd_list_bonus, token_list_bonus) == 1)
            {
                if (cmd_list_bonus)
                    cmd_free(&cmd_list_bonus);
                i++;
                continue;
            }
            tok_free(&token_list_bonus);

            if (cmd_list_bonus == NULL) {
                i++;
                continue;
            }

            // --- Lógica de expansión y heredoc (REVISAR según tus necesidades) ---
            // Si tu parte "bonus" maneja la expansión antes de la traducción:
            // should_expand(cmd_list_bonus, envlist_bonus); 
            
            // Si tu parte "bonus" prepara los heredocs (delimitador, comillas):
            // heredoc(cmd_list_bonus); 
            
            // --- Preparación para el NUEVO executor de flperez ---
            ft_memset(&data_flperez, 0, sizeof(t_data_env_exe));

            data_flperez.cmds_head = convert_bonus_cmd_list_to_flperez_cmd_list(cmd_list_bonus);
            
            if (data_flperez.cmds_head == NULL && cmd_list_bonus != NULL)
            {
                write(2, "minishell: Error during command translation\n", 44);
                cmd_free(&cmd_list_bonus);
                i++;
                continue;
            }

            data_flperez.env = convert_bonus_envlist_to_flperez_envp(envlist_bonus);
            if (!data_flperez.env && envlist_bonus)
            {
                write(2, "minishell: Error converting environment\n", 40);
                free_flperez_cmd_list(data_flperez.cmds_head);
                cmd_free(&cmd_list_bonus);
                i++;
                continue;
            }

            data_flperez.last_exit_status = g_get_signal;

            if (data_flperez.cmds_head)
            {
                // execute_pipeline internamente llamará a signals_noninteractive()
                // y luego a signals_interactive() al finalizar.
                execute_pipeline(data_flperez.cmds_head, &data_flperez);
                g_get_signal = data_flperez.last_exit_status;
            }
            
            free_flperez_cmd_list(data_flperez.cmds_head);
            free_arr2d(data_flperez.env);
            cmd_free(&cmd_list_bonus);

            i++;
        }
        free_arr2d(arg_input_lines);
        
        int final_exit_status = g_get_signal;
        cleaner_envlist(&envlist_bonus);
        exit(final_exit_status);
    }
    else if (ac != 1)
    {
        ft_msgs(10, NULL);
        cleaner_envlist(&envlist_bonus);
        exit(EXIT_FAILURE);
    }
    else // Modo Interactivo
    {
        while (1)
        {
            // Configurar señales para el modo interactivo ANTES de readline
            signals_interactive(); // <--- CAMBIO: Llamada antes de readline()
            
            char *line = readline("minishell_bonus> ");
            if (!line) 
            {
                write(1, "exit\n", 5);
                break; 
            }
            if (*line)
            {
                add_history(line);
                t_tok *token_list_bonus = NULL;
                t_cmd *cmd_list_bonus = NULL;

                tokenizer(&token_list_bonus, line);
                if (token_list_bonus != NULL)
                {
                    if (parser(&cmd_list_bonus, token_list_bonus) == 0)
                    {
                        // --- Lógica de expansión y heredoc (REVISAR) ---
                        // Si tu parte "bonus" maneja la expansión antes de la traducción:
                        // should_expand(cmd_list_bonus, envlist_bonus);
            
                        // Si tu parte "bonus" prepara los heredocs (delimitador, comillas):
                        // heredoc(cmd_list_bonus);

                        ft_memset(&data_flperez, 0, sizeof(t_data_env_exe));
                        data_flperez.cmds_head = convert_bonus_cmd_list_to_flperez_cmd_list(cmd_list_bonus);
                        data_flperez.env = convert_bonus_envlist_to_flperez_envp(envlist_bonus);
                        data_flperez.last_exit_status = g_get_signal;

                        if (data_flperez.cmds_head)
                        {
                             // execute_pipeline internamente llamará a signals_noninteractive()
                             // y luego a signals_interactive() al finalizar.
                            execute_pipeline(data_flperez.cmds_head, &data_flperez);
                            g_get_signal = data_flperez.last_exit_status;
                        }
                        
                        free_flperez_cmd_list(data_flperez.cmds_head);
                        free_arr2d(data_flperez.env);
                    }
                    if (cmd_list_bonus)
                         cmd_free(&cmd_list_bonus);
                }
                if (token_list_bonus)
                    tok_free(&token_list_bonus);
            }
            free(line);
        }
        cleaner_envlist(&envlist_bonus);
    }
    return (g_get_signal);
}