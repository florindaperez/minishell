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
#include "minishell.h" // Ensure this includes necessary headers for ft_split, ft_strcmp,
                       // and all your processing functions (tokenizer, parser, executor, etc.)

// Global variable from your project
int	g_get_signal = 0;

int	main(int ac, char *av[], char *envp[])
{
	t_env	*envlist = NULL;
	// Variables for -c mode
	char	**arg_input;
	int		i;
	// Variables for command processing (similar to those in your main_process or minishell functions)
	t_tok	*tok;
	t_cmd	*cmd;
	t_exe	exe; 
	// int		size_pipe; // exe.num_cmds will store this after init_exe

	env_init_list(envp, &envlist); // Initialize envlist once at the start

	if (ac == 3 && ft_strcmp(av[1], "-c") == 0 && av[2] && *av[2])
	{
		// Mode: Execute commands from argv[2]
		set_signals(PARENT); // Set parent signals for the -c execution mode

		arg_input = ft_split(av[2], ';');
		if (!arg_input)
		{
			// Handle ft_split allocation failure
			write(2, "minishell: Error: ft_split failed in -c mode\n", 45);
			cleaner_envlist(&envlist); // Clean up environment list
			exit(EXIT_FAILURE);
		}

		i = 0;
		while (arg_input[i])
		{
			char *current_command_line = arg_input[i];
			tok = NULL; // Initialize for each command
			cmd = NULL; // Initialize for each command

			// Skip empty or whitespace-only command segments
			// Your tokenizer already skips leading whitespace.
			// If a command segment is empty after splitting (e.g., "cmd1;;cmd2"),
			// tokenizer might result in tok being NULL.
			
			tokenizer(&tok, current_command_line);

			if (tok == NULL) { // If tokenizer produced no tokens (e.g., empty command string)
				i++;
				continue;
			}
			
			// Attempt to parse the tokens
			if (parser(&cmd, tok) == 1)
			{
				// parser calls handle_error which sets g_get_signal and frees tok.
				// cmd might be partially formed; cmd_free should handle it.
				if (cmd)
					cmd_free(&cmd);
				i++;
				continue; // Move to the next command in the sequence
			}
			// If parser succeeded (returned 0), tok is not yet freed by parser.
			tok_free(&tok);

			// If parsing resulted in no command structures (e.g. valid but empty input like only spaces after split)
			// This check might be redundant if parser itself guarantees a valid cmd or handles it by error.
			// Assuming parser creates a valid cmd structure if it returns 0, even for "empty" commands
			// that pass syntax checks. If cmd is NULL here, it implies an issue not caught above.
			if (cmd == NULL) {
				i++;
				continue;
			}

			should_expand(cmd, envlist);
			
			init_exe(&exe, cmd); // Initialize t_exe structure (allocates exe.pid)
			heredoc(cmd);        // Handle heredocs if any

			// size_pipe = cmd_size(cmd); // exe.num_cmds is set within init_exe
			pre_executor(&envlist, cmd, &exe, exe.num_cmds); // Execute the command(s)
			                                                 // pre_executor is responsible for freeing exe.pid

			cmd_free(&cmd);      // Free command structure

			i++;
		}
		free_arr2d(arg_input); // Free the array of command strings produced by ft_split
		
		int final_exit_status = g_get_signal; // Get the exit status of the last command
		cleaner_envlist(&envlist); // Clean up environment list
		exit(final_exit_status);   // Exit minishell with the determined status
	}
	else if (ac != 1) // Incorrect number of arguments for interactive mode
	{
		ft_msgs(10, NULL); // "Run minishell without arguments!"
		cleaner_envlist(&envlist);
		exit(EXIT_FAILURE);
	}
	else // (ac == 1) - Interactive mode
	{
		while (1)
		{
			set_signals(PARENT);
			minishell(envlist); // Call your existing main interactive loop
		}
		// The following lines are likely unreachable if minishell() is an infinite loop
		// but are good practice if the loop could terminate.
		// cleaner_envlist(&envlist); 
	}

	return (0); // Should generally be unreachable
}