/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:53:02 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 11:53:02 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Inc/minishell.h"

/**
 * Recover from terminal attribute errors
 * @param shell Shell structure
 * @return SUCCESS if recovery was successful, ERROR otherwise
 */
int	recover_terminal_error(t_shell *shell)
{
	// Reset terminal saved state
	shell->term_saved = 0;
	
	// Log the error
	ft_putstr_fd("minishell: Warning: Failed to configure terminal attributes\n", STDERR_FILENO);
	ft_putstr_fd("minishell: Some features may not work correctly\n", STDERR_FILENO);
	
	// Try to disable ECHOCTL using alternative methods if available
	system("stty -echoctl 2>/dev/null");
	
	// Set signal handlers anyway
	set_signal_mode(shell, 0);
	
	return (SUCCESS);
}

/**
 * Handle prompt creation errors
 * @param shell Shell structure
 * @return A basic fallback prompt or NULL on severe error
 */
char	*handle_prompt_error(t_shell *shell)
{
	char	*fallback;
	
	// Log the error
	ft_putstr_fd("minishell: Warning: Failed to create custom prompt\n", STDERR_FILENO);
	
	// Try to allocate a simple fallback prompt
	fallback = ft_strdup("minishell$ ");
	if (!fallback)
	{
		// If even this fails, use a static string
		ft_putstr_fd("minishell: Error: Critical memory allocation failure\n", STDERR_FILENO);
		return (NULL);
	}
	
	return (fallback);
}

/**
 * Handle cleanup after command execution interruption
 * @param shell Shell structure
 */
void	handle_interrupted_execution(t_shell *shell)
{
	if (!shell)
		return;

	// Log the interruption
	ft_putstr_fd("minishell: Command execution interrupted\n", STDERR_FILENO);
	
	// Check shell state consistency
	if (shell->signal_state != 0 && shell->signal_state != 1 && shell->signal_state != 2)
	{
		ft_putstr_fd("minishell: Warning: Invalid signal state detected\n", STDERR_FILENO);
		shell->signal_state = 0; // Reset to interactive mode
	}
	
	// First, clean up any active heredoc (highest priority)
	if (shell->heredoc_active)
	{
		ft_putstr_fd("minishell: Cleaning up interrupted heredoc\n", STDERR_FILENO);
		cleanup_interrupted_heredoc(shell);
	}
	
	// Reset terminal if needed (second priority)
	if (shell->term_saved)
	{
		ft_putstr_fd("minishell: Restoring terminal attributes\n", STDERR_FILENO);
		if (tcsetattr(STDIN_FILENO, TCSANOW, &shell->orig_termios) == -1)
			ft_putstr_fd("minishell: Warning: Failed to restore terminal attributes\n", STDERR_FILENO);
	}
	
	// Clean up commands and tokens (third priority)
	if (shell->commands)
	{
		free_commands(shell->commands);
		shell->commands = NULL;
	}
	
	if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
	
	// Reset signal handlers to interactive mode
	set_signal_mode(shell, 0);
	
	// Set exit status for interrupt
	shell->exit_status = 130;  // 128 + SIGINT
}

/**
 * Verify shell state consistency
 * @param shell Shell structure to verify
 * @return SUCCESS if state is consistent, ERROR otherwise
 */
int	verify_shell_state(t_shell *shell)
{
	if (!shell)
		return (ERROR);
	
	// Verify signal state
	if (shell->signal_state < 0 || shell->signal_state > 2)
	{
		ft_putstr_fd("minishell: Warning: Invalid signal state detected\n", STDERR_FILENO);
		shell->signal_state = 0; // Reset to interactive mode
	}
	
	// Verify heredoc state
	if (shell->heredoc_active && !shell->heredoc_file)
	{
		ft_putstr_fd("minishell: Warning: Inconsistent heredoc state detected\n", STDERR_FILENO);
		shell->heredoc_active = 0;
	}
	
	// Verify environment list
	if (!shell->env_list)
	{
		ft_putstr_fd("minishell: Warning: Environment list is NULL\n", STDERR_FILENO);
		return (ERROR);
	}
	
	return (SUCCESS);
}

/**
 * Initialize the shell structure with default values
 * @param envp Environment variables array
 * @return Initialized shell structure
 */
t_shell	*init_shell(char **envp)
{
	t_shell	*shell;
	int		env_init_status;

	shell = (t_shell *)malloc(sizeof(t_shell));
	if (!shell)
	{
		perror("minishell: malloc");
		return (NULL);
	}
	
	// Initialize with zeros first to ensure clean state
	ft_memset(shell, 0, sizeof(t_shell));
	
	// Set initial values
	shell->env_list = init_env(envp);
	if (!shell->env_list)
	{
		ft_putstr_fd("minishell: Failed to initialize environment variables\n", STDERR_FILENO);
		free(shell);
		return (NULL);
	}
	
	shell->exit_status = 0;
	shell->running = 1;
	
	// Set up terminal attributes
	if (setup_terminal(shell) != SUCCESS)
	{
		// Try to recover from terminal setup error
		if (recover_terminal_error(shell) != SUCCESS)
		{
			ft_putstr_fd("minishell: Failed to set up terminal\n", STDERR_FILENO);
			free_env(shell->env_list);
			free(shell);
			return (NULL);
		}
	}
	
	// Verify shell state consistency
	if (verify_shell_state(shell) != SUCCESS)
	{
		ft_putstr_fd("minishell: Shell state verification failed\n", STDERR_FILENO);
		cleanup_shell(shell);
		return (NULL);
	}
	
	return (shell);
}

/**
 * Clean up resources and free memory before exiting
 * @param shell Shell structure to clean up
 */
void	cleanup_shell(t_shell *shell)
{
	if (!shell)
		return;
		
	// Clean up any command structures
	if (shell->tokens)
		free_tokens(shell->tokens);
	if (shell->commands)
		free_commands(shell->commands);
	
	// Clean up environment
	if (shell->env_list)
		free_env(shell->env_list);
	
	// Clean up any active heredoc
	if (shell->heredoc_active)
	{
		cleanup_interrupted_heredoc(shell);
	}
	
	// Clear readline history
	clear_history();
	
	// Restore terminal settings
	restore_terminal(shell);
	
	// Reset signals to default
	reset_signals();
	
	// Free the shell structure itself
	free(shell);
}

/**
 * Check if a string contains only whitespace
 * @param str String to check
 * @return 1 if the string contains only whitespace, 0 otherwise
 */
static int	is_whitespace_only(char *str)
{
	int	i;

	if (!str)
		return (1);
	
	i = 0;
	while (str[i])
	{
		if (!is_whitespace(str[i]))
			return (0);
		i++;
	}
	return (1);
}

/**
 * Process a single command line input
 * @param input Command line input string
 * @param shell Shell structure
 * @return Success or error code
 */
int	process_input(char *input, t_shell *shell)
{
	if (!input || is_whitespace_only(input))
		return (SUCCESS);
	
	// Add command to history
	handle_history(input);
	
	// Tokenize input
	shell->tokens = tokenize_input(input);
	if (!shell->tokens)
		return (ERROR);
	
	// Expand variables in tokens
	if (expand_variables(shell->tokens, shell->env_list, shell->exit_status) != SUCCESS)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return (ERROR);
	}
	
	// Parse tokens into commands
	shell->commands = parse_tokens(shell->tokens, shell);
	if (!shell->commands)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return (ERROR);
	}
	
	// Set signal state for command execution
	set_signal_mode(shell, 1);
	
	// Execute commands
	shell->exit_status = execute_commands(shell->commands, shell);
	
	// Reset signal state to interactive mode
	set_signal_mode(shell, 0);
	
	// Cleanup
	free_tokens(shell->tokens);
	free_commands(shell->commands);
	shell->tokens = NULL;
	shell->commands = NULL;
	
	return (SUCCESS);
}

/**
 * Create a custom prompt with username, directory, and exit status
 * @param shell Shell structure
 * @return Dynamically allocated prompt string, or NULL on error
 */
char	*create_prompt(t_shell *shell)
{
	char	cwd[4096];
	char	*username;
	char	*prompt;
	char	*dir_name;
	char	*status_indicator;
	size_t	prompt_size;

	// Get username from environment
	username = get_env_value(shell->env_list, "USER");
	if (!username)
		username = "user";
	
	// Get current working directory
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		// If getcwd fails, use a default directory
		ft_strlcpy(cwd, "(unknown)", sizeof(cwd));
	}
	
	// Get just the directory name, not the full path
	dir_name = strrchr(cwd, '/');
	if (dir_name)
		dir_name++;
	else
		dir_name = cwd;
	
	// Choose status indicator based on last command result
	if (shell->exit_status == 0)
		status_indicator = "\033[32m✓\033[0m"; // Green checkmark
	else
		status_indicator = "\033[31m✗\033[0m"; // Red X
	
	// Calculate needed buffer size with safety margin
	prompt_size = ft_strlen(username) + ft_strlen(dir_name) + 
		ft_strlen(status_indicator) + 40; // 40 bytes for colors and formatting
	
	// Create the complete prompt: username@dir [status] $ 
	prompt = ft_malloc(prompt_size);
	if (!prompt)
		return (NULL);
	
	// Use snprintf for safer string formatting
	if (snprintf(prompt, prompt_size, "\033[36m%s\033[0m:\033[34m%s\033[0m %s $ ", 
		username, dir_name, status_indicator) < 0)
	{
		// Handle snprintf error
		free(prompt);
		return (NULL);
	}
	
	return (prompt);
}

/**
 * Display shell prompt and read input
 * @param shell Shell structure
 * @return Command line input string
 */
char	*get_shell_input(t_shell *shell)
{
	char	*prompt;
	char	*input;

	// Create a custom prompt
	prompt = create_prompt(shell);
	if (!prompt)
	{
		// Handle prompt creation error
		prompt = handle_prompt_error(shell);
		if (!prompt)
			prompt = "minishell$ "; // Static fallback if all else fails
	}
	
	// Set signal state to interactive mode
	set_signal_mode(shell, 0);
	
	// Read input with readline
	input = readline(prompt);
	
	// Free the dynamically allocated prompt if not using static fallback
	if (prompt && prompt != "minishell$ ")
		free(prompt);
	
	// Handle EOF (Ctrl+D) or readline errors
	if (!input)
	{
		ft_putendl_fd("exit", STDOUT_FILENO);
		shell->running = 0;
		return (NULL);
	}
	
	return (input);
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
		// Check for and clean up any interrupted heredoc
		if (shell->heredoc_active)
			cleanup_interrupted_heredoc(shell);
		
		// Get input from user (signals are set up inside)
		input = get_shell_input(shell);
		if (!input)
			continue;
		
	// Process the input and handle errors
	status = process_input(input, shell);
	if (status != SUCCESS)
	{
		// Handle interrupted execution or errors
		handle_interrupted_execution(shell);
	}
		
		// Free the input string
		free(input);
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
	
	// Initialize the shell with proper error handling
	shell = init_shell(envp);
	if (!shell)
	{
		ft_putstr_fd("minishell: Failed to initialize shell\n", STDERR_FILENO);
		return (ERROR);
	}
	
	// Set up initial signal state
	set_signal_mode(shell, 0);
	
	// Verify shell state before entering the main loop
	if (verify_shell_state(shell) != SUCCESS)
	{
		ft_putstr_fd("minishell: Shell verification failed before main loop\n", STDERR_FILENO);
		cleanup_shell(shell);
		return (ERROR);
	}
	
	// Run the shell loop
	shell_loop(shell);
	
	// Save the exit status before cleanup
	exit_status = shell->exit_status;
	
	// Clean up and exit
	cleanup_shell(shell);
	return (exit_status);
}

