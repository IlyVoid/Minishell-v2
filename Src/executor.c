/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:53:02 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 11:53:02 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Check if a command is a built-in shell command
 * @param cmd Command name to check
 * @return 1 if the command is a built-in, 0 otherwise
 */
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (ft_strcmp(cmd, "echo") == 0 || ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0 || ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0 || ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "exit") == 0);
}

/**
 * Execute a built-in shell command
 * @param cmd Command structure containing the command and arguments
 * @param shell Shell structure containing environment and state
 * @return Exit status of the command
 */
int	execute_builtin(t_command *cmd, t_shell *shell)
{
	char	*command;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (ERROR);
	
	command = cmd->args[0];
	
	if (ft_strcmp(command, "echo") == 0)
		return (builtin_echo(cmd, shell));
	else if (ft_strcmp(command, "cd") == 0)
		return (builtin_cd(cmd, shell));
	else if (ft_strcmp(command, "pwd") == 0)
		return (builtin_pwd(cmd, shell));
	else if (ft_strcmp(command, "export") == 0)
		return (builtin_export(cmd, shell));
	else if (ft_strcmp(command, "unset") == 0)
		return (builtin_unset(cmd, shell));
	else if (ft_strcmp(command, "env") == 0)
		return (builtin_env(cmd, shell));
	else if (ft_strcmp(command, "exit") == 0)
		return (builtin_exit(cmd, shell));
	
	return (ERROR);
}

/**
 * Check if a file is a heredoc temporary file
 * @param filename File name to check
 * @return 1 if it's a heredoc file, 0 otherwise
 */
static int	is_heredoc_file(char *filename)
{
	return (filename && strncmp(filename, "/tmp/heredoc_", 13) == 0);
}

/**
 * Set up file redirections for a command
 * @param redirections List of redirections to set up
 * @return Success or error code
 */
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
				// Clean up heredoc file if it exists (even if open failed)
				if (is_heredoc_file(current->file))
					cleanup_heredoc(current->file);
				return (ERROR);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
			// Clean up heredoc file after it's been opened
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
			dup2(fd, STDOUT_FILENO);
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
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		current = current->next;
	}
	
	return (SUCCESS);
}

/**
 * Find the path of an executable command
 * @param cmd Command name to find
 * @param env_list Environment variable list
 * @return Full path to the executable or NULL if not found
 */
static char	*find_command_path(char *cmd, t_env *env_list)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	if (!cmd || !*cmd)
		return (NULL);
	if (cmd[0] == '/' || cmd[0] == '.')
		return (ft_strdup(cmd));
	
	path_env = get_env_value(env_list, "PATH");
	if (!path_env)
		return (NULL);
	
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	
	i = 0;
	while (paths[i])
	{
		char *tmp = ft_strjoin(paths[i], "/");
		if (!tmp)
		{
			while (paths[i])
				free(paths[i++]);
			free(paths);
			return (NULL);
		}
		
		full_path = ft_strjoin(tmp, cmd);
		free(tmp);
		
		if (!full_path)
		{
			while (paths[i])
				free(paths[i++]);
			free(paths);
			return (NULL);
		}
		
		if (access(full_path, X_OK) == 0)
		{
			while (paths[i])
				free(paths[i++]);
			free(paths);
			return (full_path);
		}
		
		free(full_path);
		i++;
	}
	
	i = 0;
	while (paths[i])
		free(paths[i++]);
	free(paths);
	
	return (NULL);
}

/**
 * Save the current standard input and output file descriptors
 * @param saved_fds Array to store the saved file descriptors
 */
static void	save_std_fds(int saved_fds[2])
{
	saved_fds[0] = dup(STDIN_FILENO);
	saved_fds[1] = dup(STDOUT_FILENO);
}

/**
 * Restore the standard input and output file descriptors
 * @param saved_fds Array containing the saved file descriptors
 */
static void	restore_std_fds(int saved_fds[2])
{
	dup2(saved_fds[0], STDIN_FILENO);
	dup2(saved_fds[1], STDOUT_FILENO);
	close(saved_fds[0]);
	close(saved_fds[1]);
}

/**
 * Execute a single command
 * @param cmd Command structure containing the command and arguments
 * @param shell Shell structure containing environment and state
 * @param in_fd Input file descriptor (for pipes)
 * @param out_fd Output file descriptor (for pipes)
 * @return Exit status of the command
 */
static int	execute_single_command(t_command *cmd, t_shell *shell,
	int in_fd, int out_fd)
{
	char	*cmd_path;
	char	**env_array;
	int		status;
	pid_t	pid;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (ERROR);
	
	// Set up signal handling for command execution
	setup_exec_signals();
	
	if (is_builtin(cmd->args[0]) && !cmd->pipe_out && in_fd == STDIN_FILENO)
	{
		int saved_fds[2];
		
		save_std_fds(saved_fds);
		
		if (out_fd != STDOUT_FILENO)
			dup2(out_fd, STDOUT_FILENO);
		
		if (setup_redirections(cmd->redirections) != SUCCESS)
		{
			restore_std_fds(saved_fds);
			return (ERROR);
		}
		
		status = execute_builtin(cmd, shell);
		
		restore_std_fds(saved_fds);
		// Reset signal handling back to interactive mode
		setup_signals();
		return (status);
	}
	
	pid = fork();
	if (pid == -1)
	{
		print_error("fork", NULL, NULL);
		return (ERROR);
	}
	
	if (pid == 0)
	{
		// Child process
		if (in_fd != STDIN_FILENO)
		{
			dup2(in_fd, STDIN_FILENO);
			close(in_fd);
		}
		
		if (out_fd != STDOUT_FILENO)
		{
			dup2(out_fd, STDOUT_FILENO);
			close(out_fd);
		}
		
		if (setup_redirections(cmd->redirections) != SUCCESS)
			exit(ERROR);
		
		if (is_builtin(cmd->args[0]))
		{
			status = execute_builtin(cmd, shell);
			exit(status);
		}
		
		cmd_path = find_command_path(cmd->args[0], shell->env_list);
		if (!cmd_path)
		{
			print_error(cmd->args[0], NULL, "command not found");
			exit(CMD_NOT_FOUND);
		}
		
		env_array = env_to_array(shell->env_list);
		if (!env_array)
		{
			free(cmd_path);
			exit(ERROR);
		}
		
		execve(cmd_path, cmd->args, env_array);
		
		// If execve returns, there was an error
		print_error(cmd_path, NULL, NULL);
		free(cmd_path);
		
		exit(ERROR);
	}
	else
	{
		// Parent process
		waitpid(pid, &status, 0);
		
		// Reset signal handling back to interactive mode
		setup_signals();
		
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
		{
			// Handle signals like Ctrl+C (SIGINT) or Ctrl+\ (SIGQUIT)
			if (WTERMSIG(status) == SIGINT)
				return (130);  // 128 + SIGINT (2)
			else if (WTERMSIG(status) == SIGQUIT)
				return (131);  // 128 + SIGQUIT (3)
			else
				return (128 + WTERMSIG(status));
		}
		else
			return (ERROR);
	}
}

/**
 * Execute a list of commands
 * @param commands List of commands to execute
 * @param shell Shell structure containing environment and state
 * @return Exit status of the last command
 */
/**
 * Cleanup all heredoc files used in command redirections
 * @param cmd Command to clean up heredoc files for
 */
static void	cleanup_heredoc_files(t_command *cmd)
{
	t_redirection	*redir;

	if (!cmd)
		return;
	
	redir = cmd->redirections;
	while (redir)
	{
		if (is_heredoc_file(redir->file))
			cleanup_heredoc(redir->file);
		redir = redir->next;
	}
}

/**
 * Cleanup all heredoc files used in a command list
 * @param commands Command list to clean up heredoc files for
 */
static void	cleanup_all_heredocs(t_command *commands)
{
	t_command	*current;

	current = commands;
	while (current)
	{
		cleanup_heredoc_files(current);
		current = current->next;
	}
}

int	execute_commands(t_command *commands, t_shell *shell)
{
	t_command	*current;
	int			status;
	int			pipefd[2];
	int			prev_pipe_read;

	if (!commands)
		return (ERROR);
	
	current = commands;
	prev_pipe_read = STDIN_FILENO;
	status = ERROR;
	
	while (current)
	{
		if (current->pipe_out)
		{
			if (pipe(pipefd) == -1)
			{
				print_error("pipe", NULL, NULL);
				return (ERROR);
			}
		}
		else
			pipefd[1] = STDOUT_FILENO;
		
		status = execute_single_command(current, shell, prev_pipe_read, pipefd[1]);
		
		if (prev_pipe_read != STDIN_FILENO)
			close(prev_pipe_read);
		
		if (current->pipe_out)
		{
			close(pipefd[1]);
			prev_pipe_read = pipefd[0];
		}
		
		current = current->next;
	}
	
	// Make sure to clean up all heredoc temporary files
	cleanup_all_heredocs(commands);
	
	return (status);
}

