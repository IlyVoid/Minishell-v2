/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 17:58:00 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/* Check if a file is a heredoc temporary file */
int	is_heredoc_file(char *filename)
{
	return (filename && strncmp(filename, "/tmp/heredoc_", 13) == 0);
}

/* Set up file redirections for a command */
int	setup_redirections(t_redirection *redirections)
{
	t_redirection	*current;
	int				fd;

	if (!redirections)
		return (SUCCESS);
	current = redirections;
	while (current)
	{
		if (current->type == TOKEN_REDIRECT_IN)
		{
			fd = open(current->file, O_RDONLY);
			if (fd == -1)
			{
				print_error(NULL, current->file, NULL);
				if (is_heredoc_file(current->file))
					cleanup_heredoc(current->file);
				return (ERROR);
			}
			if (dup2(fd, STDIN_FILENO) == -1)
			{
				close(fd);
				return (ERROR);
			}
			close(fd);
			if (is_heredoc_file(current->file))
				cleanup_heredoc(current->file);
		}
		else if (current->type == TOKEN_REDIRECT_OUT)
		{
			fd = open(current->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
			{
				print_error(NULL, current->file, NULL);
				return (ERROR);
			}
			if (dup2(fd, STDOUT_FILENO) == -1)
			{
				close(fd);
				return (ERROR);
			}
			close(fd);
		}
		else if (current->type == TOKEN_REDIRECT_APPEND)
		{
			fd = open(current->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
			{
				print_error(NULL, current->file, NULL);
				return (ERROR);
			}
			if (dup2(fd, STDOUT_FILENO) == -1)
			{
				close(fd);
				return (ERROR);
			}
			close(fd);
		}
		current = current->next;
	}
	return (SUCCESS);
}

/* Cleanup heredoc files used in a command */
void	cleanup_heredoc_files(t_command *cmd)
{
	t_redirection	*redir;

	if (!cmd)
		return ;
	redir = cmd->redirections;
	while (redir)
	{
		if (is_heredoc_file(redir->file))
			cleanup_heredoc(redir->file);
		redir = redir->next;
	}
}

/* Cleanup all heredoc files in a command list */
void	cleanup_all_heredocs(t_command *commands)
{
	t_command	*current;

	current = commands;
	while (current)
	{
		cleanup_heredoc_files(current);
		current = current->next;
	}
}

