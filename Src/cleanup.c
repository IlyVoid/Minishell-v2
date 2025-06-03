/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:29:58 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:29:58 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Clean up command resources (tokens and commands)
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	cleanup_command_resources(t_shell *shell)
{
	if (!shell)
		return (ERROR);
		
	// Free tokens and commands regardless of errors
	if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
	
	if (shell->commands)
	{
		free_commands(shell->commands);
		shell->commands = NULL;
	}
	
	return (SUCCESS);
}

/**
 * Clean up terminal state and history
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	cleanup_terminal_state(t_shell *shell)
{
	int	status;

	if (!shell)
		return (ERROR);
		
	status = SUCCESS;
	
	// Clear readline history
	clear_history();
	
	// Restore terminal settings
	if (restore_terminal(shell) != SUCCESS)
	{
		ft_putstr_fd("minishell: Warning: Failed to restore terminal settings\n", 
			STDERR_FILENO);
		status = ERROR;
		// Continue with cleanup despite the error
	}
	
	// Reset signal handlers
	reset_signals();
	
	return (status);
}

/**
 * Clean up active process resources
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	cleanup_active_process(t_shell *shell)
{
	int	status;

	if (!shell)
		return (ERROR);
		
	status = SUCCESS;
	
	// Clean up any active heredoc
	if (shell->heredoc_active)
	{
		// Try to clean up the heredoc file
		cleanup_interrupted_heredoc(shell);
		
		// Check if cleanup failed
		if (shell->heredoc_active)
		{
			ft_putstr_fd("minishell: Warning: Failed to clean up heredoc\n", 
				STDERR_FILENO);
			status = ERROR;
			// Force reset to avoid infinite loop
			shell->heredoc_active = 0;
		}
		
		// Free the filename
		if (shell->heredoc_file)
		{
			free(shell->heredoc_file);
			shell->heredoc_file = NULL;
		}
	}
	
	// Reset signal handlers to interactive mode
	set_signal_mode(shell, 0);
	
	return (status);
}

/**
 * Clean up all shell resources
 * @param shell Shell structure to clean up
 * @return SUCCESS or ERROR (though typically not checked as this is called during shutdown)
 */
int	cleanup_shell(t_shell *shell)
{
	int	status;

	if (!shell)
		return (ERROR);
		
	status = SUCCESS;
	
	// Clean up commands and tokens
	if (cleanup_command_resources(shell) != SUCCESS)
		status = ERROR;
	
	// Free environment list
	if (shell->env_list)
	{
		free_env(shell->env_list);
		shell->env_list = NULL;
	}
	
	// Clean up active processes (e.g., heredoc)
	if (cleanup_active_process(shell) != SUCCESS)
		status = ERROR;
	
	// Clean up terminal state
	if (cleanup_terminal_state(shell) != SUCCESS)
		status = ERROR;
	
	// Finally, free the shell structure itself
	free(shell);
	
	return (status);
}

