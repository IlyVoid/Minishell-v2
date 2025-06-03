/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 17:58:00 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Free array of strings and return NULL
 * @param paths Array of strings to free
 * @return NULL always
 */
static char	*free_paths_and_return_null(char **paths)
{
	int	i;

	if (!paths)
		return (NULL);
		
	i = 0;
	while (paths[i])
	{
		free(paths[i]);
		i++;
	}
	free(paths);
	return (NULL);
}

/**
 * Check if a path is valid and has reasonable length
 * @param path Path to check
 * @return 1 if valid, 0 otherwise
 */
static int	is_valid_path(char *path)
{
	if (!path)
		return (0);
		
	// Check reasonable path length limit
	if (ft_strlen(path) > 4096 || ft_strlen(path) == 0)
		return (0);
		
	return (1);
}

/**
 * Try to find command in given path
 * @param path Directory path to search in
 * @param cmd Command name to look for
 * @return Full path if found and executable, NULL otherwise
 */
static char	*try_path(char *path, char *cmd)
{
	char	*tmp;
	char	*full_path;

	// Skip empty path segments
	if (!path || !*path || !cmd || !*cmd)
		return (NULL);
		
	// Create full path
	tmp = ft_strjoin(path, "/");
	if (!tmp)
		return (NULL);
		
	full_path = ft_strjoin(tmp, cmd);
	free(tmp);
	if (!full_path)
		return (NULL);
		
	// Check if file exists and is executable
	if (access(full_path, F_OK) == 0)
	{
		if (access(full_path, X_OK) == 0)
			return (full_path);
		
		// File exists but not executable
		print_error(full_path, NULL, "Permission denied");
	}
	
	free(full_path);
	return (NULL);
}

/**
 * Find the path of an executable command
 * @param cmd Command to find
 * @param env_list Environment variable list for PATH lookup
 * @return Full path to command or NULL if not found
 */
char	*find_command_path(char *cmd, t_env *env_list)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	// Basic command validation
	if (!cmd || !*cmd)
		return (NULL);
		
	// Check path length
	if (!is_valid_path(cmd))
	{
		print_error(cmd, NULL, "Path too long");
		return (NULL);
	}
	
	// Handle absolute or relative paths directly
	if (cmd[0] == '/' || cmd[0] == '.' || ft_strchr(cmd, '/'))
	{
		// Check if the file exists and is executable
		if (access(cmd, F_OK) != 0)
		{
			print_error(cmd, NULL, "No such file or directory");
			return (NULL);
		}
		if (access(cmd, X_OK) != 0)
		{
			print_error(cmd, NULL, "Permission denied");
			return (NULL);
		}
		return (ft_strdup(cmd));
	}
	
	// Search in PATH environment variable
	path_env = get_env_value(env_list, "PATH");
	if (!path_env || !*path_env)
	{
		print_error(cmd, NULL, "No such file or directory");
		return (NULL);
	}
	
	// Split PATH into components
	paths = ft_split(path_env, ':');
	if (!paths)
	{
		print_error(cmd, NULL, "Memory allocation failed");
		return (NULL);
	}
	
	// Try each PATH component
	i = 0;
	while (paths[i])
	{
		full_path = try_path(paths[i], cmd);
		if (full_path)
		{
			free_paths_and_return_null(paths);
			return (full_path);
		}
		i++;
	}
	
	// Command not found in any PATH component
	return (free_paths_and_return_null(paths));
}

