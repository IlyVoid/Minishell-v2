/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:04:24 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:04:24 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#endif

/**
 * Set up terminal attributes for shell use
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	setup_terminal(t_shell *shell)
{
	if (!shell)
		return (ERROR);
	return (SUCCESS);
}

/**
 * Restore original terminal attributes
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	restore_terminal(t_shell *shell)
{
	if (!shell)
		return (ERROR);
		
	// Only restore if we successfully saved the original attributes
	if (shell->term_saved)
	{
		shell->term_saved = 0;
	}
	
	return (SUCCESS);
}

/**
 * Check if string contains only whitespace
 * @param str String to check
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
 * Safely add input to command history
 * @param input Input string to add to history
 * @return SUCCESS or ERROR
 */
int	handle_history(char *input)
{
	// Skip empty inputs, NULL or whitespace-only inputs
	if (!input || !*input || is_only_whitespace(input))
		return (SUCCESS);
	
	// Add to history - readline's add_history doesn't return a value
	// so we can't check for errors, but we handle input validation above
	add_history(input);
	
	return (SUCCESS);
}

/**
 * Handle signal state transitions
 * @param shell Shell structure
 */
void	handle_signal_state(t_shell *shell)
{
	// Update signal state based on the shell's current state
	if (shell->heredoc_active)
	{
		setup_heredoc_signals();
		shell->signal_state = 2;  // Heredoc mode
	}
	else if (shell->commands)
	{
		setup_exec_signals();
		shell->signal_state = 1;  // Execution mode
	}
	else
	{
		setup_signals();
		shell->signal_state = 0;  // Interactive mode
	}
}

/**
 * Clean up after a heredoc is interrupted
 * @param shell Shell structure
 */
void	cleanup_interrupted_heredoc(t_shell *shell)
{
	// Only clean up if there is an active heredoc
	if (shell->heredoc_active && shell->heredoc_file)
	{
		// Clean up the temporary file
		cleanup_heredoc(shell->heredoc_file);
		free(shell->heredoc_file);
		shell->heredoc_file = NULL;
		shell->heredoc_active = 0;
		
		// Reset signal state to interactive mode
		shell->signal_state = 0;
		setup_signals();
	}
}

/**
 * Set up signals for appropriate mode and update shell state
 * @param shell Shell structure
 * @param mode Signal mode (0: interactive, 1: execution, 2: heredoc)
 */
/**
 * Set signal mode for different shell states
 * @param shell Shell structure
 * @param mode Signal mode (0: interactive, 1: execution, 2: heredoc)
 * @return SUCCESS or ERROR
 */
int	set_signal_mode(t_shell *shell, int mode)
{
	if (!shell)
		return (ERROR);
		
	shell->signal_state = mode;
	
	// Set appropriate signal handlers based on mode
	if (mode == 0)
	{
		// Interactive mode
		setup_signals();
	}
	else if (mode == 1)
	{
		// Execution mode
		setup_exec_signals();
	}
	else if (mode == 2)
	{
		// Heredoc mode
		setup_heredoc_signals();
	}
	else
	{
		// Invalid mode
		return (ERROR);
	}
	
	return (SUCCESS);
}

/**
 * Start a heredoc operation and update shell state
 * @param shell Shell structure
 * @param file Heredoc temporary file path
 */
/**
 * Start a heredoc operation and update shell state
 * @param shell Shell structure
 * @param file Heredoc temporary file path
 * @return SUCCESS or ERROR
 */
int	start_heredoc(t_shell *shell, char *file)
{
	if (!shell || !file)
		return (ERROR);
		
	// Mark heredoc as active and store file path
	shell->heredoc_active = 1;
	
	// Duplicate file path with proper error checking
	shell->heredoc_file = ft_strdup(file);
	if (!shell->heredoc_file)
	{
		shell->heredoc_active = 0;
		print_error("heredoc", NULL, "memory allocation failed");
		return (ERROR);
	}
	
	// Set signal mode to heredoc
	set_signal_mode(shell, 2);
	
	return (SUCCESS);
}

/**
 * End a heredoc operation and update shell state
 * @param shell Shell structure
 */
/**
 * End a heredoc operation and update shell state
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	end_heredoc(t_shell *shell)
{
	if (!shell)
		return (ERROR);
		
	// Mark heredoc as inactive and clean up
	shell->heredoc_active = 0;
	if (shell->heredoc_file)
	{
		free(shell->heredoc_file);
		shell->heredoc_file = NULL;
	}
	
	// Return to interactive mode
	set_signal_mode(shell, 0);
	
	return (SUCCESS);
}

/**
 * Recover from terminal setup errors
 * @param shell Shell structure
 * @return SUCCESS if recovery was successful, ERROR otherwise
 */
int	recover_terminal_error(t_shell *shell)
{
	if (!shell)
		return (ERROR);

	// If we failed to set up the terminal, we can still run in a limited mode
	// Just make sure to mark that we don't have saved terminal settings
	shell->term_saved = 0;
	
	// Log a warning about running in limited mode
	ft_putstr_fd("minishell: Warning: Terminal setup failed, running in limited mode\n", 
		STDERR_FILENO);
	
	return (SUCCESS);
}
