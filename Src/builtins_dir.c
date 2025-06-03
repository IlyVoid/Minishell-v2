/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_dir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 18:10:58 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Handles changing to HOME directory
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
static int	cd_to_home(t_shell *shell)
{
	char	*path;

	if (!shell || !shell->env_list)
	{
		print_error("cd", NULL, "shell not initialized");
		return (ERROR);
	}
	
	path = get_env_value(shell->env_list, "HOME");
	if (!path)
	{
		print_error("cd", NULL, "HOME not set");
		return (ERROR);
	}
	
	// Check path length
	if (ft_strlen(path) > 4096)
	{
		print_error("cd", NULL, "path too long");
		return (ERROR);
	}
	
	if (chdir(path) != 0)
	{
		print_error("cd", path, NULL);
		return (ERROR);
	}
	
	return (SUCCESS);
}

/**
 * Handles changing to previous directory
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
static int	cd_to_previous(t_shell *shell)
{
	char	*path;

	if (!shell || !shell->env_list)
	{
		print_error("cd", NULL, "shell not initialized");
		return (ERROR);
	}

	path = get_env_value(shell->env_list, "OLDPWD");
	if (!path)
	{
		print_error("cd", NULL, "OLDPWD not set");
		return (ERROR);
	}
	
	// Check path length
	if (ft_strlen(path) > 4096)
	{
		print_error("cd", NULL, "path too long");
		return (ERROR);
	}
	
	// Display the directory we're changing to
	if (write(STDOUT_FILENO, path, ft_strlen(path)) == -1 ||
		write(STDOUT_FILENO, "\n", 1) == -1)
	{
		print_error("cd", NULL, "write error");
		// Continue despite write error
	}
	
	if (chdir(path) != 0)
	{
		print_error("cd", path, NULL);
		return (ERROR);
	}
	
	return (SUCCESS);
}

/**
 * Updates PWD and OLDPWD environment variables
 * @param shell Shell structure
 * @param old_pwd Previous working directory
 * @return SUCCESS or ERROR
 */
static int	update_pwd_vars(t_shell *shell, char *old_pwd)
{
	char	current_dir[4096];

	if (!shell || !shell->env_list || !old_pwd)
	{
		free(old_pwd);
		return (ERROR);
	}

	if (getcwd(current_dir, sizeof(current_dir)) == NULL)
	{
		print_error("cd", NULL, "getcwd error");
		free(old_pwd);
		return (ERROR);
	}
	
	// Update environment variables with error checking
	if (set_env_value(shell->env_list, "OLDPWD", old_pwd) == ERROR)
	{
		print_error("cd", NULL, "failed to update OLDPWD");
		free(old_pwd);
		return (ERROR);
	}
	
	if (set_env_value(shell->env_list, "PWD", current_dir) == ERROR)
	{
		print_error("cd", NULL, "failed to update PWD");
		free(old_pwd);
		return (ERROR);
	}
	
	free(old_pwd);
	return (SUCCESS);
}

/**
 * Built-in cd command - changes current directory
 * @param cmd Command structure
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	builtin_cd(t_command *cmd, t_shell *shell)
{
	char	*old_pwd;
	char	current_dir[4096];

	// Basic error checking
	if (!cmd || !shell || !shell->env_list)
	{
		print_error("cd", NULL, "invalid arguments");
		return (ERROR);
	}
	
	// Get current directory before changing
	if (getcwd(current_dir, sizeof(current_dir)) == NULL)
	{
		print_error("cd", NULL, "getcwd error");
		return (ERROR);
	}
	
	// Save old directory
	old_pwd = ft_strdup(current_dir);
	if (!old_pwd)
	{
		print_error("cd", NULL, "memory allocation failed");
		return (ERROR);
	}
	
	// No argument - go to HOME
	if (!cmd->args[1])
	{
		if (cd_to_home(shell) != SUCCESS)
		{
			free(old_pwd);
			return (ERROR);
		}
	}
	// "-" argument - go to previous directory
	else if (ft_strcmp(cmd->args[1], "-") == 0)
	{
		if (cd_to_previous(shell) != SUCCESS)
		{
			free(old_pwd);
			return (ERROR);
		}
	}
	// Regular path argument
	else
	{
		// Check path length
		if (ft_strlen(cmd->args[1]) > 4096)
		{
			print_error("cd", NULL, "path too long");
			free(old_pwd);
			return (ERROR);
		}
		
		// Try to change directory
		if (chdir(cmd->args[1]) != 0)
		{
			print_error("cd", cmd->args[1], NULL);
			free(old_pwd);
			return (ERROR);
		}
	}
	
	// Update environment variables
	return (update_pwd_vars(shell, old_pwd));
}

