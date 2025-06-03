/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_core.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/03 17:22:24 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/* External global signal variable */
extern volatile int	g_received_signal;

/* Check if a command is a built-in shell command */
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (ft_strcmp(cmd, "echo") == 0 || ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0 || ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0 || ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "exit") == 0);
}

/* Execute a built-in shell command */
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

/* Execute a list of commands, handling pipes */
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
	status = SUCCESS;
	while (current)
	{
		if (current->pipe_out && pipe(pipefd) == -1)
		{
			print_error("pipe", NULL, NULL);
			if (prev_pipe_read != STDIN_FILENO)
				close(prev_pipe_read);
			return (ERROR);
		}
		status = execute_single_command(current, shell,
				prev_pipe_read, current->pipe_out ? pipefd[1] : STDOUT_FILENO);
		if (prev_pipe_read != STDIN_FILENO)
			close(prev_pipe_read);
		if (current->pipe_out)
		{
			close(pipefd[1]);
			prev_pipe_read = pipefd[0];
		}
		current = current->next;
	}
	cleanup_all_heredocs(commands);
	return (status);
}

