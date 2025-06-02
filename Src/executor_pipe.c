/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 17:58:00 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/* External global signal variable */
extern int	g_received_signal;

/* Execute builtin directly (without forking) */
static int	execute_builtin_directly(t_command *cmd, t_shell *shell, int out_fd)
{
	int	status;
	int	saved_fds[2];

	saved_fds[0] = -1;
	saved_fds[1] = -1;
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
	return (status);
}

/* Execute child process after fork */
static void	execute_child_process(t_command *cmd, t_shell *shell,
	int in_fd, int out_fd)
{
	char	*cmd_path;
	char	**env_array;

	if (in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
			exit(ERROR);
		close(in_fd);
	}
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
	execve(cmd_path, cmd->args, env_array);
	print_error(cmd_path, NULL, NULL);
	free(cmd_path);
	free_string_array(env_array);
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
	if (is_builtin(cmd->args[0]) && !cmd->pipe_out && in_fd == STDIN_FILENO)
		return (execute_builtin_directly(cmd, shell, out_fd));
	setup_exec_signals();
	pid = fork();
	if (pid == -1)
	{
		print_error("fork", NULL, NULL);
		return (ERROR);
	}
	if (pid == 0)
		execute_child_process(cmd, shell, in_fd, out_fd);
	else
	{
		g_received_signal = 0;
		waitpid(pid, &status, 0);
		setup_signals();
		return (get_exit_status(status));
	}
	return (ERROR);
}

