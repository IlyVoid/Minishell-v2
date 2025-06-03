/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:29:58 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:29:58 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Check if a string contains only whitespace
 * @param str String to check
 * @return 1 if whitespace only, 0 otherwise
 */

static int	is_whitespace_only(char *str)
{
	int	i;
	size_t len;

	if (!str)
		return (1);
		
	// Check string length first for efficiency
	len = ft_strlen(str);
	if (len == 0)
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
 * Handle parser errors
 * @param shell Shell structure
 * @param error_type Type of error
 */

void	handle_parse_error(t_shell *shell, int error_type)
{
	const char *error_msg;
	
	if (!shell)
		return ;
		
	// Free tokens if they exist
	if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
	
	// Choose appropriate error message
	if (error_type == SYNTAX_ERROR)
		error_msg = "minishell: syntax error\n";
	else
		error_msg = "minishell: parse error\n";
		
	// Write error message with error checking
	if (write(STDERR_FILENO, error_msg, ft_strlen(error_msg)) == -1)
	{
		// Even if writing fails, continue with exit status
		perror("minishell: write error");
	}
	
	shell->exit_status = error_type;
}

/**
 * Parse input into tokens and commands
 * @param input Input string
 * @param shell Shell structure
 * @return SUCCESS or error code
 */
int	parse_input(char *input, t_shell *shell)
{
	// Validate shell state
	if (!shell || !shell->env_list)
	{
		if (shell)
			handle_parse_error(shell, ERROR);
		return (ERROR);
	}
	
	// Tokenize input
	shell->tokens = tokenize_input(input);
	if (!shell->tokens)
		return (ERROR);
		
	// Expand variables
	if (expand_variables(shell->tokens, shell->env_list,
			shell->exit_status) != SUCCESS)
	{
		handle_parse_error(shell, ERROR);
		return (ERROR);
	}
	
	// Parse tokens into commands
	shell->commands = parse_tokens(shell->tokens, shell);
	if (!shell->commands)
	{
		handle_parse_error(shell, SYNTAX_ERROR);
		return (SYNTAX_ERROR);
	}
	
	return (SUCCESS);
}

/**
 * Execute parsed commands
 * @param shell Shell structure
 * @return Exit status of commands
 */
/**
 * Execute parsed commands
 * @param shell Shell structure
 * @return Exit status of commands
 */
int	execute_input(t_shell *shell)
{
	int	status;
	int	signal_status;

	if (!shell)
		return (ERROR);

	// Set signals for execution mode
	signal_status = set_signal_mode(shell, 1);
	if (signal_status != SUCCESS)
	{
		// Not critical, print warning but continue
		write(STDERR_FILENO, "minishell: warning: signal setup error\n", 38);
	}
	
	// Execute commands
	status = execute_commands(shell->commands, shell);
	
	// Always restore signals to interactive mode, regardless of execution result
	if (set_signal_mode(shell, 0) != SUCCESS)
	{
		// Not critical, print warning but continue
		write(STDERR_FILENO, "minishell: warning: signal restore error\n", 40);
	}
	
	// Clean up command resources
	cleanup_command_resources(shell);
	
	return (status);
}

/**
 * Process a command line input
 * @param input Command line input
 * @param shell Shell structure
 * @return SUCCESS or error code
 */
int	process_input(char *input, t_shell *shell)
{
	int	status;

	// Check for NULL or empty input
	if (!input || is_whitespace_only(input))
		return (SUCCESS);
		
	// Check input length (match shell line limit - 10000 chars)
	if (ft_strlen(input) > 10000)
	{
		write(STDERR_FILENO, "minishell: input line too long\n", 31);
		return (ERROR);
	}
	
	// Add to history with error checking
	if (handle_history(input) != SUCCESS)
	{
		// Non-critical error, continue processing
		write(STDERR_FILENO, "minishell: history error\n", 25);
	}
	
	// Parse and execute input
	status = parse_input(input, shell);
	if (status != SUCCESS)
		return (status);
		
	shell->exit_status = execute_input(shell);
	return (SUCCESS);
}

