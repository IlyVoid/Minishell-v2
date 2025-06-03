/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:04:24 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:04:24 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Create a temporary file for heredoc content
 * @return Path to the temporary file or NULL on error
 */
char	*create_heredoc_file(void)
{
	char	*filename;
	int		fd;
	static int	heredoc_count = 0;

	// Create a unique filename for the heredoc
	filename = ft_malloc(sizeof(char) * 20);
	if (!filename)
		return (NULL);
	
	sprintf(filename, "/tmp/heredoc_%d", heredoc_count++);
	
	// Create and close the file
	fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
	{
		free(filename);
		return (NULL);
	}
	
	close(fd);
	return (filename);
}

/**
 * Clean up a heredoc temporary file
 * @param filename Path to the temporary file
 */
int cleanup_heredoc(char *filename)
{
	if (!filename)
		return ERROR;
	if (unlink(filename) == -1)
		return ERROR;
	return SUCCESS;
}

/**
 * Expand variables in a heredoc line
 * @param line Line to expand variables in
 * @param env_list Environment variable list
 * @param exit_status Last command exit status
 * @return Expanded line or NULL on error
 */
char	*expand_heredoc(char *line, t_env *env_list, int exit_status)
{
	int		i;
	char	*result;
	int		in_single_quotes;
	char	*var_name;
	char	*var_value;
	
	if (!line)
		return (NULL);
	
	result = ft_strdup(line);
	if (!result)
		return (NULL);
	
	i = 0;
	in_single_quotes = 0;
	
	while (result[i])
	{
		if (result[i] == '\'')
			in_single_quotes = !in_single_quotes;
		else if (result[i] == '$' && !in_single_quotes)
		{
			// Extract variable name
			int j = i + 1;
			while (result[j] && (ft_isalnum(result[j]) || result[j] == '_'))
				j++;
			
			// Handle exit status ($?)
			if (result[i + 1] == '?')
			{
				var_value = ft_itoa(exit_status);
				j = i + 2;
			}
			else if (j > i + 1)
			{
				var_name = ft_substr(result, i + 1, j - i - 1);
				if (!var_name)
				{
					free(result);
					return (NULL);
				}
				
				var_value = get_env_value(env_list, var_name);
				if (var_value)
					var_value = ft_strdup(var_value);
				else
					var_value = ft_strdup("");
				
				free(var_name);
			}
			else
			{
				// $ without a variable name, leave as is
				i++;
				continue;
			}
			
			if (!var_value)
			{
				free(result);
				return (NULL);
			}
			
			// Replace variable in the string
			char *before = ft_substr(result, 0, i);
			char *after = ft_strdup(result + j);
			if (!before || !after)
			{
				free(result);
				free(var_value);
				if (before)
					free(before);
				return (NULL);
			}
			
			char *temp1 = ft_strjoin(before, var_value);
			free(before);
			free(var_value);
			if (!temp1)
			{
				free(result);
				free(after);
				return (NULL);
			}
			
			char *temp2 = ft_strjoin(temp1, after);
			free(temp1);
			free(after);
			if (!temp2)
			{
				free(result);
				return (NULL);
			}
			
			free(result);
			result = temp2;
			i = 0;  // Start over with the new string
			continue;
		}
		i++;
	}
	
	return (result);
}

/**
 * Read heredoc input until delimiter is encountered
 * @param delimiter Delimiter string to end heredoc
 * @param fd File descriptor to write heredoc content to
 * @param env_list Environment variable list
 * @param exit_status Last command exit status
 * @return Success or error code
 */
int	read_heredoc(char *delimiter, int fd, t_env *env_list, int exit_status)
{
	char	*line;
	char	*expanded;
	int		status;

	status = SUCCESS;
	
	// Set up heredoc signal handling
	setup_heredoc_signals();
	
	// Read lines until delimiter is encountered
	while (1)
	{
		line = readline("> ");
		
		// Check for EOF or delimiter
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			if (line)
				free(line);
			break;
		}
		
		// Expand variables in the line
		expanded = expand_heredoc(line, env_list, exit_status);
		free(line);
		
		if (!expanded)
		{
			status = ERROR;
			break;
		}
		
		// Write the line to the file
		ft_putstr_fd(expanded, fd);
		ft_putstr_fd("\n", fd);
		free(expanded);
	}
	
	// Restore signal handling
	reset_signals();
	
	return (status);
}

/**
 * Handle heredoc input processing
 * @param delimiter Delimiter string to end heredoc
 * @param env_list Environment variable list
 * @param exit_status Last command exit status
 * @return Path to the temporary file containing heredoc content or NULL on error
 */
char	*handle_heredoc(char *delimiter, t_env *env_list, int exit_status)
{
	char	*filename;
	int		fd;
	int		prev_stdin;
	int		status;

	// Create a temporary file for heredoc content
	filename = create_heredoc_file();
	if (!filename)
		return (NULL);
	
	fd = open(filename, O_WRONLY | O_APPEND, 0644);
	if (fd == -1)
	{
		free(filename);
		return (NULL);
	}
	
	// Save stdin fd to restore later
	prev_stdin = dup(STDIN_FILENO);
	if (prev_stdin == -1)
	{
		close(fd);
		cleanup_heredoc(filename);
		free(filename);
		return (NULL);
	}
	
	// Process heredoc input
	status = read_heredoc(delimiter, fd, env_list, exit_status);
	
	// Restore stdin
	dup2(prev_stdin, STDIN_FILENO);
	close(prev_stdin);
	close(fd);
	
	if (status == ERROR)
	{
		cleanup_heredoc(filename);
		free(filename);
		return (NULL);
	}
	
	return (filename);
}

