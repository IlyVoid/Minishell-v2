/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_basic.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 17:58:00 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Checks if a string is the -n flag for echo
 * @param str String to check
 * @return 1 if it's a -n flag, 0 otherwise
 */
static int	is_n_flag(char *str)
{
	int	i;

	if (!str || str[0] != '-' || str[1] != 'n')
		return (0);
		
	i = 2;
	while (str[i])
	{
		if (str[i] != 'n')
			return (0);
		i++;
	}
	
	return (1);
}

/**
 * Built-in echo command - prints arguments with optional newline suppression
 * @param cmd Command structure
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	builtin_echo(t_command *cmd, t_shell *shell)
{
	int	i;
	int	n_flag;
	size_t total_len;

	(void)shell;
	if (!cmd || !cmd->args)
		return (ERROR);
		
	n_flag = 0;
	i = 1;
	total_len = 0;
	
	// Process all -n flags
	while (cmd->args[i] && is_n_flag(cmd->args[i]))
	{
		n_flag = 1;
		i++;
	}
	
	// Calculate total output length for validation
	int arg_start = i;
	while (cmd->args[i])
	{
		total_len += ft_strlen(cmd->args[i]);
		if (cmd->args[i + 1])
			total_len += 1; // Space
		i++;
	}
	
	// Check for reasonable output length
	if (total_len > 10000)
	{
		print_error("echo", NULL, "output too long");
		return (ERROR);
	}
	
	// Print arguments with error checking
	i = arg_start;
	while (cmd->args[i])
	{
		if (write(STDOUT_FILENO, cmd->args[i], ft_strlen(cmd->args[i])) == -1)
		{
			print_error("echo", NULL, "write error");
			return (ERROR);
		}
		
		if (cmd->args[i + 1])
		{
			if (write(STDOUT_FILENO, " ", 1) == -1)
			{
				print_error("echo", NULL, "write error");
				return (ERROR);
			}
		}
		i++;
	}
	
	// Print newline if -n flag not present
	if (!n_flag)
	{
		if (write(STDOUT_FILENO, "\n", 1) == -1)
		{
			print_error("echo", NULL, "write error");
			return (ERROR);
		}
	}
	
	return (SUCCESS);
}

/**
 * Built-in pwd command - prints current working directory
 * @param cmd Command structure
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	builtin_pwd(t_command *cmd, t_shell *shell)
{
	char	current_dir[4096];

	(void)cmd;
	(void)shell;
	
	// Get current working directory with specific error handling
	if (getcwd(current_dir, sizeof(current_dir)) == NULL)
	{
		if (errno == ERANGE)
			print_error("pwd", NULL, "path too long");
		else if (errno == EACCES)
			print_error("pwd", NULL, "permission denied");
		else
			print_error("pwd", NULL, strerror(errno));
			
		return (ERROR);
	}
	
	// Print with error checking
	size_t dir_len = ft_strlen(current_dir);
	if (write(STDOUT_FILENO, current_dir, dir_len) == -1)
	{
		print_error("pwd", NULL, "write error");
		return (ERROR);
	}
	
	if (write(STDOUT_FILENO, "\n", 1) == -1)
	{
		print_error("pwd", NULL, "write error");
		return (ERROR);
	}
	
	return (SUCCESS);
}

