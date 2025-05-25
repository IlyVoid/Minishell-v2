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
#include <termios.h>

/**
 * Set up terminal attributes for shell use
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	setup_terminal(t_shell *shell)
{
	struct termios	term;

	// Save original terminal attributes
	if (tcgetattr(STDIN_FILENO, &shell->orig_termios) == -1)
	{
		print_error("tcgetattr", NULL, "failed to get terminal attributes");
		return (ERROR);
	}
	
	shell->term_saved = 1;
	
	// Make a copy of the original attributes
	term = shell->orig_termios;
	
	// Modify terminal settings:
	// - Disable echo to avoid echoing control characters
	// - Disable canonical mode to allow control character processing
	term.c_lflag &= ~(ECHOCTL);
	
	// Apply the modified settings
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
	{
		print_error("tcsetattr", NULL, "failed to set terminal attributes");
		return (ERROR);
	}
	
	return (SUCCESS);
}

/**
 * Restore original terminal attributes
 * @param shell Shell structure
 */
void	restore_terminal(t_shell *shell)
{
	// Only restore if we successfully saved the original attributes
	if (shell->term_saved)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, &shell->orig_termios);
		shell->term_saved = 0;
	}
}

/**
 * Safely add input to command history
 * @param input Input string to add to history
 * @return SUCCESS or ERROR
 */
int	handle_history(char *input)
{
	// Skip empty inputs or NULL
	if (!input || !*input)
		return (SUCCESS);
	
	// Check if the input contains only whitespace
	int	i;
	int	is_whitespace_only;

	i = 0;
	is_whitespace_only = 1;
	while (input[i])
	{
		if (input[i] != ' ' && input[i] != '\t' && input[i] != '\n')
		{
			is_whitespace_only = 0;
			break;
		}
		i++;
	}
	
	// Only add non-whitespace inputs to history
	if (!is_whitespace_only)
	{
		add_history(input);
		return (SUCCESS);
	}
	
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
void	set_signal_mode(t_shell *shell, int mode)
{
	shell->signal_state = mode;
	
	if (mode == 0)
		setup_signals();
	else if (mode == 1)
		setup_exec_signals();
	else if (mode == 2)
		setup_heredoc_signals();
}

/**
 * Start a heredoc operation and update shell state
 * @param shell Shell structure
 * @param file Heredoc temporary file path
 */
void	start_heredoc(t_shell *shell, char *file)
{
	// Mark heredoc as active and store file path
	shell->heredoc_active = 1;
	shell->heredoc_file = ft_strdup(file);
	
	// Set signal mode to heredoc
	set_signal_mode(shell, 2);
}

/**
 * End a heredoc operation and update shell state
 * @param shell Shell structure
 */
void	end_heredoc(t_shell *shell)
{
	// Mark heredoc as inactive and clean up
	shell->heredoc_active = 0;
	if (shell->heredoc_file)
	{
		free(shell->heredoc_file);
		shell->heredoc_file = NULL;
	}
	
	// Return to interactive mode
	set_signal_mode(shell, 0);
}

