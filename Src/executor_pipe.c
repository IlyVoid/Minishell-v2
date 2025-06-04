/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 18:14:16 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/* External global signal variable */
extern volatile sig_atomic_t g_received_signal;

/* Execute builtin directly (without forking) */
int	execute_builtin_directly(t_command *cmd, t_shell *shell, int out_fd)
{
	int	status;
	int	saved_fds[2];

	saved_fds[0] = -1;
	saved_fds[1] = -1;
	
	// Save standard file descriptors with error checking
	if (save_std_fds(saved_fds) != SUCCESS)
	{
		print_error("execute_builtin", NULL, "failed to save file descriptors");
		return (ERROR);
	}
	
	// Redirect output if needed
	if (out_fd != STDOUT_FILENO)
	{
		if (dup2(out_fd, STDOUT_FILENO) == -1)
		{
			print_error("execute_builtin", NULL, "dup2 failed");
			restore_std_fds(saved_fds);
			return (ERROR);
		}
	}
	
	// Setup redirections
	if (setup_redirections(cmd->redirections) != SUCCESS)
	{
		restore_std_fds(saved_fds);
		return (ERROR);
	}
	status = execute_builtin(cmd, shell);
	restore_std_fds(saved_fds);
	return (status);
}

/**
 * Execute child process after fork
 * @param cmd Command to execute
 * @param shell Shell structure
 * @param in_fd Input file descriptor
 * @param out_fd Output file descriptor
 */
int	execute_child_process(t_command *cmd, t_shell *shell,
	int in_fd, int out_fd)
{
	char	*cmd_path;
	char	**env_array;

	// Redirect input if needed
	if (in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
			exit(ERROR);
		close(in_fd);
	}
	
	// Redirect output if needed
	if (out_fd != STDOUT_FILENO)
	{
		if (dup2(out_fd, STDOUT_FILENO) == -1)
			exit(ERROR);
		close(out_fd);
	}
	if (setup_redirections(cmd->redirections) != SUCCESS)
		exit(ERROR);
	if (is_builtin(cmd->args[0]))
		exit(execute_builtin(cmd, shell));
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
	
	// Execute the command
	execve(cmd_path, cmd->args, env_array);
	
	// If execve fails, we reach here
	print_error(cmd_path, NULL, NULL);
	
	// Clean up all resources before exit
	free(cmd_path);
	free_string_array(env_array);
	
	// Close any redirected file descriptors
	if (in_fd != STDIN_FILENO)
		close(STDIN_FILENO);
	if (out_fd != STDOUT_FILENO)
		close(STDOUT_FILENO);
		
	exit(ERROR);
}

/* Execute a single command */
int	execute_single_command(t_command *cmd, t_shell *shell,
	int in_fd, int out_fd)
{
	int		status;
	pid_t	pid;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (ERROR);
		
	// Handle builtins directly if possible
	if (is_builtin(cmd->args[0]) && !cmd->pipe_out && in_fd == STDIN_FILENO)
		return (execute_builtin_directly(cmd, shell, out_fd));
	
	// Set up signal handlers for execution
	setup_exec_signals()
	
	// Create child process
	pid = fork();
	if (pid == -1)
	{
		print_error("fork", NULL, NULL);
		return (ERROR);
	}
	
	// Child process
	if (pid == 0)
		execute_child_process(cmd, shell, in_fd, out_fd);
	// Parent process
	else
	{
		int wait_result;
		
		// Reset signal flag
		g_received_signal = 0;
		
		// Wait for child process with error handling for interruption
		wait_result = waitpid(pid, &status, 0);
		while (wait_result == -1 && errno == EINTR)
		{
			// If interrupted by signal, try again
			wait_result = waitpid(pid, &status, 0);
		}
		
		// Restore interactive mode signals
		setup_signals();
		
		// Check if waitpid failed for a reason other than interruption
		if (wait_result == -1)
		{
			print_error("waitpid", NULL, NULL);
			return (ERROR);
		}
		
		return (get_exit_status(status));
	}
	return (ERROR);
}

