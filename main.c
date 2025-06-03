/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:53:02 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:40:51 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Inc/minishell.h"

/**
 * Main shell loop
 * @param shell Shell structure
 */
/**
 * Check if input is only whitespace
 * @param str The string to check
 * @return 1 if only whitespace, 0 otherwise
 */
static int	is_only_whitespace(const char *str)
{
	int	i;

	if (!str)
		return (1);
	i = 0;
	while (str[i])
	{
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
			return (0);
		i++;
	}
	return (1);
}

/**
 * Main shell loop
 * @param shell Shell structure
 */
void	shell_loop(t_shell *shell)
{
	char	*input;
	int		status;

	while (shell->running)
	{
		/* Check for signals that might have been received */
		if (g_received_signal)
		{
			shell->exit_status = 128 + g_received_signal;
			g_received_signal = 0;
		}

		/* Handle interrupted heredoc */
		if (shell->heredoc_active)
		{
			cleanup_interrupted_heredoc(shell);
			if (shell->heredoc_active)
			{
				ft_putstr_fd("minishell: Failed to cleanup heredoc\n", STDERR_FILENO);
				shell->heredoc_active = 0; /* Force reset to avoid infinite loop */
			}
		}
		
		input = get_shell_input(shell);
		if (!input)
			continue;
		
		/* Skip empty or whitespace-only commands */
		if (is_only_whitespace(input))
		{
			free(input);
			continue;
		}
			
		status = process_input(input, shell);
		if (status == SYNTAX_ERROR)
			shell->exit_status = 2; /* Standard syntax error exit code */
		else if (status != SUCCESS)
			handle_interrupted_execution(shell);
			
		free(input);
		input = NULL; /* Prevent use-after-free */
	}
}

/**
 * Main function
 * @param argc Argument count
 * @param argv Argument vector
 * @param envp Environment variables
 * @return Exit status
 */

int	main(int argc, char **argv, char **envp)
{
	t_shell	*shell;
	int		exit_status;

	(void)argc;
	(void)argv;
	shell = init_shell(envp);
	if (!shell)
	{
		ft_putstr_fd("minishell: Failed to initialize shell\n", STDERR_FILENO);
		return (ERROR);
	}
	
	// Setup terminal with error checking
	if (setup_terminal(shell) != SUCCESS && recover_terminal_error(shell) != SUCCESS)
	{
		ft_putstr_fd("minishell: Fatal terminal setup error\n", STDERR_FILENO);
		cleanup_shell(shell);
		return (ERROR);
	}
	
	// Setup signal handlers
	set_signal_mode(shell, 0);
	
	// Verify shell state
	if (verify_shell_state(shell) != SUCCESS)
	{
		ft_putstr_fd("minishell: Shell state verification failed\n", STDERR_FILENO);
		cleanup_shell(shell);
		return (ERROR);
	}
	
	// Main shell loop
	shell_loop(shell);
	
	// Final cleanup of any remaining heredoc files
	if (shell->heredoc_active)
	{
		cleanup_interrupted_heredoc(shell);
	}
	
	// Cleanup all commands to ensure no heredoc files remain
	if (shell->commands)
	{
		cleanup_all_heredocs(shell->commands);
	}
	
	// Cleanup and return
	exit_status = shell->exit_status;
	cleanup_shell(shell);
	return (exit_status);
}

