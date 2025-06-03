/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_exit.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 18:13:15 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Check if a string is a valid numeric value
 * @param str String to check
 * @return 1 if numeric, 0 otherwise
 */
int	is_numeric(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
		
	// Check string length first to avoid overflow
	if (ft_strlen(str) > 10)  // Long int max is 10 digits
		return (0);
		
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
		
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	
	return (1);
}

/**
 * Built-in exit command - exits the shell with optional status
 * @param cmd Command structure
 * @param shell Shell structure
 * @return Exit code
 */
int	builtin_exit(t_command *cmd, t_shell *shell)
{
	int	exit_code;
	long long value;

	// Basic error checking
	if (!cmd || !shell)
		return (ERROR);
		
	// Write "exit" message
	if (write(STDOUT_FILENO, "exit\n", 5) == -1)
	{
		// Even if write fails, continue with exit
		print_error("exit", NULL, "write error");
	}
	
	// No argument case
	if (!cmd->args[1])
		exit_code = shell->exit_status;
	// Non-numeric argument case
	else if (!is_numeric(cmd->args[1]))
	{
		print_error("exit", cmd->args[1], "numeric argument required");
		exit_code = 255;
	}
	// Too many arguments case
	else if (cmd->args[2])
	{
		print_error("exit", NULL, "too many arguments");
		return (ERROR);  // Don't exit in this case
	}
	// Numeric argument case
	else
	{
		// Use atoi but check for overflow
		value = ft_atoi(cmd->args[1]);
		
		// Use modulo 256 to get valid exit code
		exit_code = (int)(value % 256);
		
		// Handle negative values
		if (exit_code < 0)
			exit_code += 256;
	}
	
	// Set shell to not running
	shell->running = 0;
	shell->exit_status = exit_code;
	return (exit_code);
}

