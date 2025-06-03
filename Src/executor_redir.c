/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/03 17:30:22 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Check if a path is valid and safe
 * @param path Path to check
 * @return 1 if valid, 0 otherwise
 */
static int	is_valid_path(char *path)
{
	if (!path)
		return (0);
		
	// Basic path sanity check
	if (ft_strlen(path) > 4096 || ft_strlen(path) == 0)
		return (0);
		
	// Disallow relative paths with .. for security
	if (ft_strstr(path, "../") || ft_strcmp(path, "..") == 0)
		return (0);
		
	return (1);
}

/**
 * Check if parent directory is writable
 * @param file_path File path to check
 * @return SUCCESS or ERROR
 */
static int	check_directory_permission(char *file_path)
{
	char	*dir_path;
	char	*last_slash;
	int		result;

	if (!file_path)
		return (ERROR);
		
	// Validate path
	if (!is_valid_path(file_path))
	{
		print_error(NULL, file_path, "Invalid path");
		return (ERROR);
	}
	
	// Extract directory part
	dir_path = ft_strdup(file_path);
	if (!dir_path)
		return (ERROR);
		
	last_slash = ft_strrchr(dir_path, '/');
	if (last_slash)
	{
		*last_slash = '\0';
		// Empty string means root directory
		if (dir_path[0] == '\0')
		{
			free(dir_path);
			dir_path = ft_strdup("/");
			if (!dir_path)
				return (ERROR);
		}
		
		// Check directory permission
		result = access(dir_path, W_OK);
		if (result == -1)
		{
			print_error(NULL, file_path, "Permission denied");
			free(dir_path);
			return (ERROR);
		}
	}
	
	free(dir_path);
	return (SUCCESS);
}

/**
 * Check if a file is a heredoc temporary file
 * @param filename Filename to check
 * @return 1 if file is a heredoc file, 0 otherwise
 */
int	is_heredoc_file(char *filename)
{
	if (!filename)
		return (0);
		
	// Check if the filename starts with the heredoc prefix
	return (strncmp(filename, "/tmp/heredoc_", 13) == 0);
}

/**
 * Set up file redirections for a command
 * @param redirections List of redirections to set up
 * @return SUCCESS or ERROR
 */
int	setup_redirections(t_redirection *redirections)
{
	t_redirection	*current;
	int				fd;
	struct stat		file_stat;

	if (!redirections)
		return (SUCCESS);
		
	current = redirections;
	while (current)
	{
		if (current->type == TOKEN_REDIRECT_IN)
		{
			// Check file exists and has proper permissions
			if (access(current->file, F_OK) == -1)
			{
				print_error(NULL, current->file, "No such file or directory");
				return (ERROR);
			}
			
			if (access(current->file, R_OK) == -1)
			{
				print_error(NULL, current->file, "Permission denied");
				return (ERROR);
			}
			
			// Check file size
			if (stat(current->file, &file_stat) == 0 && file_stat.st_size > 1000000000)
			{
				print_error(NULL, current->file, "File too large");
				return (ERROR);
			}
			
			// Open file with retry for EINTR
			do {
				fd = open(current->file, O_RDONLY);
			} while (fd == -1 && errno == EINTR);
			
			if (fd == -1)
			{
				print_error(NULL, current->file, NULL);
				if (is_heredoc_file(current->file))
					cleanup_heredoc(current->file);
				return (ERROR);
			}
			
			// Redirect standard input with retry for EINTR
			int dup_result;
			do {
				dup_result = dup2(fd, STDIN_FILENO);
			} while (dup_result == -1 && errno == EINTR);
			
			if (dup_result == -1)
			{
				print_error("dup2", NULL, "failed to redirect input");
				close(fd);
				return (ERROR);
			}
			
			close(fd);
			
			// Clean up heredoc file if needed
			if (is_heredoc_file(current->file))
				cleanup_heredoc(current->file);
		}
		else if (current->type == TOKEN_REDIRECT_OUT)
		{
			// Check directory permissions
			if (check_directory_permission(current->file) != SUCCESS)
				return (ERROR);
			
			// Open file with truncation and retry for EINTR
			do {
				fd = open(current->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			} while (fd == -1 && errno == EINTR);
			if (fd == -1)
			{
				print_error(NULL, current->file, NULL);
				return (ERROR);
			}
			
			// Redirect standard output with retry for EINTR
			int dup_result;
			do {
				dup_result = dup2(fd, STDOUT_FILENO);
			} while (dup_result == -1 && errno == EINTR);
			
			if (dup_result == -1)
			{
				print_error("dup2", NULL, "failed to redirect output");
				close(fd);
				return (ERROR);
			}
			
			close(fd);
		}
		else if (current->type == TOKEN_REDIRECT_APPEND)
		{
			// Check directory permissions
			if (check_directory_permission(current->file) != SUCCESS)
				return (ERROR);
			
			// Open file in append mode with retry for EINTR
			do {
				fd = open(current->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			} while (fd == -1 && errno == EINTR);
			if (fd == -1)
			{
				print_error(NULL, current->file, NULL);
				return (ERROR);
			}
			
			// Redirect standard output with retry for EINTR
			int dup_result;
			do {
				dup_result = dup2(fd, STDOUT_FILENO);
			} while (dup_result == -1 && errno == EINTR);
			
			if (dup_result == -1)
			{
				print_error("dup2", NULL, "failed to redirect output");
				close(fd);
				return (ERROR);
			}
			
			close(fd);
		}
		
		current = current->next;
	}
	
	return (SUCCESS);
}

/**
 * Cleanup heredoc files used in a command
 * @param cmd Command structure containing redirections
 * @return SUCCESS or ERROR
 */
int	cleanup_heredoc_files(t_command *cmd)
{
	t_redirection	*redir;
	int				status;

	if (!cmd)
		return (SUCCESS);
		
	status = SUCCESS;
	redir = cmd->redirections;
	
	while (redir)
	{
		if (is_heredoc_file(redir->file))
		{
			if (cleanup_heredoc(redir->file) != SUCCESS)
				status = ERROR;
		}
		redir = redir->next;
	}
	
	return (status);
}

/**
 * Cleanup all heredoc files in a command list
 * @param commands List of commands
 * @return SUCCESS or ERROR
 */
int	cleanup_all_heredocs(t_command *commands)
{
	t_command	*current;
	int			status;

	if (!commands)
		return (SUCCESS);
		
	status = SUCCESS;
	current = commands;
	
	while (current)
	{
		if (cleanup_heredoc_files(current) != SUCCESS)
			status = ERROR;
		current = current->next;
	}
	
	return (status);
}

