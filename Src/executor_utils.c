/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 17:58:00 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Get proper exit status based on wait() status
 * @param status Status returned by wait/waitpid
 * @return Exit status code
 */
int	get_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
			return (130);
		else if (WTERMSIG(status) == SIGQUIT)
			return (131);
		else
			return (128 + WTERMSIG(status));
	}
	return (ERROR);
}

/**
 * Save the current standard input and output file descriptors
 * @param saved_fds Array to store saved file descriptors
 * @return SUCCESS or ERROR
 */
int	save_std_fds(int saved_fds[2])
{
	if (!saved_fds)
		return (ERROR);
		
	// Save stdin with EINTR handling
	do {
		saved_fds[0] = dup(STDIN_FILENO);
	} while (saved_fds[0] == -1 && errno == EINTR);
	
	if (saved_fds[0] == -1)
	{
		print_error("dup", NULL, "failed to save stdin");
		return (ERROR);
	}
	
	// Save stdout with EINTR handling
	do {
		saved_fds[1] = dup(STDOUT_FILENO);
	} while (saved_fds[1] == -1 && errno == EINTR);
	
	if (saved_fds[1] == -1)
	{
		print_error("dup", NULL, "failed to save stdout");
		// Close stdin if already saved
		if (saved_fds[0] != -1)
		{
			close(saved_fds[0]);
			saved_fds[0] = -1;
		}
		return (ERROR);
	}
	
	return (SUCCESS);
}

/**
 * Restore the standard input and output file descriptors
 * @param saved_fds Array containing saved file descriptors
 * @return SUCCESS or ERROR
 */
int	restore_std_fds(int saved_fds[2])
{
	int	status;
	int	dup_result;

	if (!saved_fds)
		return (ERROR);
		
	status = SUCCESS;
	
	// Restore stdin if it was saved
	if (saved_fds[0] != -1)
	{
		// Restore with EINTR handling
		do {
			dup_result = dup2(saved_fds[0], STDIN_FILENO);
		} while (dup_result == -1 && errno == EINTR);
		
		if (dup_result == -1)
		{
			print_error("dup2", NULL, "failed to restore stdin");
			status = ERROR;
		}
		
		// Close saved fd with error checking
		if (close(saved_fds[0]) == -1)
		{
			print_error("close", NULL, "failed to close saved stdin");
			status = ERROR;
		}
		saved_fds[0] = -1;
	}
	
	// Restore stdout if it was saved
	if (saved_fds[1] != -1)
	{
		// Restore with EINTR handling
		do {
			dup_result = dup2(saved_fds[1], STDOUT_FILENO);
		} while (dup_result == -1 && errno == EINTR);
		
		if (dup_result == -1)
		{
			print_error("dup2", NULL, "failed to restore stdout");
			status = ERROR;
		}
		
		// Close saved fd with error checking
		if (close(saved_fds[1]) == -1)
		{
			print_error("close", NULL, "failed to close saved stdout");
			status = ERROR;
		}
		saved_fds[1] = -1;
	}
	
	return (status);
}

/**
 * Free array of strings
 * @param arr Array of strings to free
 * @return SUCCESS or ERROR
 */
int	free_string_array(char **arr)
{
	int	i;

	if (!arr)
		return (SUCCESS);
		
	i = 0;
	while (arr[i])
	{
		if (arr[i])
		{
			free(arr[i]);
			arr[i] = NULL;
		}
		i++;
	}
	
	free(arr);
	return (SUCCESS);
}

