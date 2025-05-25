/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:04:24 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:04:24 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Built-in echo command - prints arguments with optional newline suppression
 * @param cmd Command structure containing arguments
 * @param shell Shell structure
 * @return Success or error code
 */
int	builtin_echo(t_command *cmd, t_shell *shell)
{
	int	i;
	int	n_flag;

	(void)shell;
	if (!cmd || !cmd->args)
		return (ERROR);
	
	n_flag = 0;
	i = 1;
	
	// Check for -n option
	if (cmd->args[i] && ft_strcmp(cmd->args[i], "-n") == 0)
	{
		n_flag = 1;
		i++;
	}
	
	// Print arguments with spaces between them
	while (cmd->args[i])
	{
		ft_putstr_fd(cmd->args[i], STDOUT_FILENO);
		if (cmd->args[i + 1])
			ft_putstr_fd(" ", STDOUT_FILENO);
		i++;
	}
	
	// Print newline if -n flag is not set
	if (!n_flag)
		ft_putstr_fd("\n", STDOUT_FILENO);
	
	return (SUCCESS);
}

/**
 * Built-in cd command - changes current directory
 * @param cmd Command structure containing arguments
 * @param shell Shell structure
 * @return Success or error code
 */
int	builtin_cd(t_command *cmd, t_shell *shell)
{
	char	*path;
	char	*old_pwd;
	char	current_dir[4096];

	if (!cmd->args[1])
	{
		// cd with no arguments changes to HOME directory
		path = get_env_value(shell->env_list, "HOME");
		if (!path)
		{
			print_error("cd", NULL, "HOME not set");
			return (ERROR);
		}
	}
	else if (ft_strcmp(cmd->args[1], "-") == 0)
	{
		// cd - changes to OLDPWD
		path = get_env_value(shell->env_list, "OLDPWD");
		if (!path)
		{
			print_error("cd", NULL, "OLDPWD not set");
			return (ERROR);
		}
		ft_putendl_fd(path, STDOUT_FILENO);
	}
	else
		path = cmd->args[1];
	
	// Save current directory before changing
	if (getcwd(current_dir, sizeof(current_dir)) == NULL)
	{
		print_error("cd", NULL, "getcwd error");
		return (ERROR);
	}
	old_pwd = ft_strdup(current_dir);
	if (!old_pwd)
		return (ERROR);
	
	// Change directory
	if (chdir(path) != 0)
	{
		print_error("cd", path, NULL);
		free(old_pwd);
		return (ERROR);
	}
	
	// Update PWD and OLDPWD environment variables
	if (getcwd(current_dir, sizeof(current_dir)) == NULL)
	{
		print_error("cd", NULL, "getcwd error");
		free(old_pwd);
		return (ERROR);
	}
	
	set_env_value(shell->env_list, "OLDPWD", old_pwd);
	set_env_value(shell->env_list, "PWD", current_dir);
	
	free(old_pwd);
	return (SUCCESS);
}

/**
 * Built-in pwd command - prints current working directory
 * @param cmd Command structure containing arguments
 * @param shell Shell structure
 * @return Success or error code
 */
int	builtin_pwd(t_command *cmd, t_shell *shell)
{
	char	current_dir[4096];

	(void)cmd;
	(void)shell;
	
	if (getcwd(current_dir, sizeof(current_dir)) == NULL)
	{
		print_error("pwd", NULL, NULL);
		return (ERROR);
	}
	
	ft_putendl_fd(current_dir, STDOUT_FILENO);
	return (SUCCESS);
}

/**
 * Checks if a variable name is valid (starts with letter or _, contains alphanumeric or _)
 * @param var Variable name to check
 * @return 1 if valid, 0 if invalid
 */
static int	is_valid_variable_name(char *var)
{
	int	i;

	if (!var || !*var)
		return (0);
	
	if (!ft_isalpha(var[0]) && var[0] != '_')
		return (0);
	
	i = 1;
	while (var[i])
	{
		if (!ft_isalnum(var[i]) && var[i] != '_')
			return (0);
		i++;
	}
	
	return (1);
}

/**
 * Parse a variable assignment string (KEY=VALUE)
 * @param arg Argument string to parse
 * @param key Pointer to store the key
 * @param value Pointer to store the value
 * @return Success or error code
 */
static int	parse_variable_assignment(char *arg, char **key, char **value)
{
	char	*equal_sign;
	int		key_len;

	equal_sign = strchr(arg, '=');
	if (!equal_sign)
	{
		*key = ft_strdup(arg);
		if (!*key)
			return (ERROR);
		*value = NULL;
		return (SUCCESS);
	}
	
	key_len = equal_sign - arg;
	*key = ft_substr(arg, 0, key_len);
	if (!*key)
		return (ERROR);
	
	*value = ft_strdup(equal_sign + 1);
	if (!*value)
	{
		free(*key);
		return (ERROR);
	}
	
	return (SUCCESS);
}

/**
 * Built-in export command - sets environment variables
 * @param cmd Command structure containing arguments
 * @param shell Shell structure
 * @return Success or error code
 */
int	builtin_export(t_command *cmd, t_shell *shell)
{
	int		i;
	char	*key;
	char	*value;
	int		status;
	t_env	*current;

	status = SUCCESS;
	
	if (!cmd->args[1])
	{
		// If no arguments, print all environment variables in export format
		current = shell->env_list;
		while (current)
		{
			ft_putstr_fd("declare -x ", STDOUT_FILENO);
			ft_putstr_fd(current->key, STDOUT_FILENO);
			if (current->value)
			{
				ft_putstr_fd("=\"", STDOUT_FILENO);
				ft_putstr_fd(current->value, STDOUT_FILENO);
				ft_putstr_fd("\"", STDOUT_FILENO);
			}
			ft_putstr_fd("\n", STDOUT_FILENO);
			current = current->next;
		}
		return (SUCCESS);
	}
	
	i = 1;
	while (cmd->args[i])
	{
		if (parse_variable_assignment(cmd->args[i], &key, &value) == ERROR)
			return (ERROR);
		
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
			if (set_env_value(shell->env_list, key, value) == ERROR)
				status = ERROR;
			free(key);
			if (value)
				free(value);
		}
		i++;
	}
	
	return (status);
}

/**
 * Built-in unset command - removes environment variables
 * @param cmd Command structure containing arguments
 * @param shell Shell structure
 * @return Success or error code
 */
int	builtin_unset(t_command *cmd, t_shell *shell)
{
	int	i;
	int	status;

	status = SUCCESS;
	
	if (!cmd->args[1])
		return (SUCCESS);
	
	i = 1;
	while (cmd->args[i])
	{
		if (!is_valid_variable_name(cmd->args[i]))
		{
			print_error("unset", cmd->args[i], "not a valid identifier");
			status = ERROR;
		}
		else
		{
			if (unset_env_value(&shell->env_list, cmd->args[i]) == ERROR)
				status = ERROR;
		}
		i++;
	}
	
	return (status);
}

/**
 * Built-in env command - displays environment variables
 * @param cmd Command structure containing arguments
 * @param shell Shell structure
 * @return Success or error code
 */
int	builtin_env(t_command *cmd, t_shell *shell)
{
	t_env	*current;

	(void)cmd;
	
	current = shell->env_list;
	while (current)
	{
		if (current->value)  // Only print variables with values
		{
			ft_putstr_fd(current->key, STDOUT_FILENO);
			ft_putstr_fd("=", STDOUT_FILENO);
			ft_putstr_fd(current->value, STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
		current = current->next;
	}
	
	return (SUCCESS);
}

/**
 * Check if a string is a valid numeric value
 * @param str String to check
 * @return 1 if numeric, 0 otherwise
 */
static int	is_numeric(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	
	if (!str[i])  // Only a sign
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
 * @param cmd Command structure containing arguments
 * @param shell Shell structure
 * @return Exit status code
 */
int	builtin_exit(t_command *cmd, t_shell *shell)
{
	int	exit_code;

	ft_putendl_fd("exit", STDOUT_FILENO);
	
	if (!cmd->args[1])
	{
		// No arguments, use current exit status
		exit_code = shell->exit_status;
	}
	else if (!is_numeric(cmd->args[1]))
	{
		// Not a number
		print_error("exit", cmd->args[1], "numeric argument required");
		exit_code = 255;
	}
	else if (cmd->args[2])
	{
		// Too many arguments
		print_error("exit", NULL, "too many arguments");
		return (ERROR);  // Don't exit
	}
	else
	{
		// Use provided exit code
		exit_code = ft_atoi(cmd->args[1]);
		// Ensure it fits in 8 bits (POSIX standard)
		exit_code = exit_code & 255;
	}
	
	shell->running = 0;  // Signal that the shell should exit
	return (exit_code);
}

