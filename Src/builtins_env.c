/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 17:58:00 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/* Print all environment variables in export format */
/**
 * Print all environment variables in export format
 * @param env_list Environment variable list
 * @return SUCCESS or ERROR
 */
static int	print_exported_env(t_env *env_list)
{
	t_env	*current;

	if (!env_list)
		return (ERROR);
		
	current = env_list;
	while (current)
	{
		// Check for write errors with each operation
		if (write(STDOUT_FILENO, "declare -x ", 11) == -1 ||
			write(STDOUT_FILENO, current->key, ft_strlen(current->key)) == -1)
		{
			print_error("export", NULL, "write error");
			return (ERROR);
		}
		
		// All variables should be displayed with quotes, even if value is NULL
		// This is standard shell behavior
		if (write(STDOUT_FILENO, "=\"", 2) == -1)
		{
			print_error("export", NULL, "write error");
			return (ERROR);
		}
		
		// Write the value if it exists (could be empty string)
		if (current->value && 
			write(STDOUT_FILENO, current->value, ft_strlen(current->value)) == -1)
		{
			print_error("export", NULL, "write error");
			return (ERROR);
		}
		
		// Close the quotes and add newline
		if (write(STDOUT_FILENO, "\"\n", 2) == -1)
		{
			print_error("export", NULL, "write error");
			return (ERROR);
		}
		
		current = current->next;
	}
	return (SUCCESS);
}

/* Built-in export command - sets environment variables */
int	builtin_export(t_command *cmd, t_shell *shell)
{
	int		i;
	char	*key;
	char	*value;
	int		status;

	if (!cmd || !shell || !shell->env_list)
		return (ERROR);
		
	status = SUCCESS;
	if (!cmd->args[1])
	{
		return (print_exported_env(shell->env_list));
	}
	
	i = 1;
	while (cmd->args[i])
	{
		// Check for maximum variable length
		if (ft_strlen(cmd->args[i]) > 1024)
		{
			print_error("export", cmd->args[i], "variable name too long");
			status = ERROR;
			i++;
			continue;
		}
		
		if (parse_variable_assignment(cmd->args[i], &key, &value) == ERROR)
		{
			print_error("export", cmd->args[i], "parse error");
			status = ERROR;
			i++;
			continue;
		}
		
		if (!is_valid_variable_name(key))
		{
			print_error("export", cmd->args[i], "not a valid identifier");
			free(key);
			if (value)
				free(value);
			status = ERROR;
		}
		else
		{
			// Make sure empty values are handled consistently
			// Pass empty string instead of NULL for empty values
			char *val_to_set = value ? value : "";
			
			if (set_env_value(shell->env_list, key, val_to_set) == ERROR)
			{
				print_error("export", key, "failed to set variable");
				status = ERROR;
			}
			free(key);
			if (value)
				free(value);
		}
		i++;
	}
	return (status);
}

/* Built-in unset command - removes environment variables */
int	builtin_unset(t_command *cmd, t_shell *shell)
{
	int	i;
	int	status;

	if (!cmd || !shell || !shell->env_list)
		return (ERROR);
		
	status = SUCCESS;
	if (!cmd->args[1])
		return (SUCCESS);
		
	i = 1;
	while (cmd->args[i])
	{
		// Check for maximum variable length
		if (ft_strlen(cmd->args[i]) > 1024)
		{
			print_error("unset", cmd->args[i], "variable name too long");
			status = ERROR;
			i++;
			continue;
		}
		
		if (!is_valid_variable_name(cmd->args[i]))
		{
			print_error("unset", cmd->args[i], "not a valid identifier");
			status = ERROR;
		}
		else if (unset_env_value(&shell->env_list, cmd->args[i]) == ERROR)
		{
			// Just silently continue if variable doesn't exist
			// This matches standard shell behavior
		}
		i++;
	}
	return (status);
}

/* Built-in env command - displays environment variables */
/**
 * Built-in env command - displays environment variables
 * @param cmd Command structure
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	builtin_env(t_command *cmd, t_shell *shell)
{
	t_env	*current;

	(void)cmd;
	
	if (!shell || !shell->env_list)
	{
		print_error("env", NULL, "no environment variables");
		return (ERROR);
	}
	
	current = shell->env_list;
	while (current)
	{
		// Only display variables that have a value (including empty values)
		if (current->value != NULL)
		{
			// Check for write errors with each operation
			if (write(STDOUT_FILENO, current->key, ft_strlen(current->key)) == -1 ||
				write(STDOUT_FILENO, "=", 1) == -1 ||
				write(STDOUT_FILENO, current->value, ft_strlen(current->value)) == -1 ||
				write(STDOUT_FILENO, "\n", 1) == -1)
			{
				print_error("env", NULL, "write error");
				return (ERROR);
			}
		}
		current = current->next;
	}
	return (SUCCESS);
}

