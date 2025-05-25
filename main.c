/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:53:02 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:40:51 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Inc/minishell.h"

/**
 * Main shell loop
 * @param shell Shell structure
 */
void	shell_loop(t_shell *shell)
{
	char	*input;
	int		status;

	while (shell->running)
	{
		if (shell->heredoc_active)
			cleanup_interrupted_heredoc(shell);
		input = get_shell_input(shell);
		if (!input)
			continue;
		status = process_input(input, shell);
		if (status != SUCCESS)
			handle_interrupted_execution(shell);
		free(input);
	}
}

/**
 * Main function
 * @param argc Argument count
 * @param argv Argument vector
 * @param envp Environment variables
 * @return Exit status
 */

int	main(int argc, char **argv, char **envp)
{
	t_shell	*shell;
	int		exit_status;

	(void)argc;
	(void)argv;
	shell = init_shell(envp);
	if (!shell)
	{
		ft_putstr_fd("minishell: Failed to initialize shell\n", STDERR_FILENO);
		return (ERROR);
	}
	set_signal_mode(shell, 0);
	if (verify_shell_state(shell) != SUCCESS)
	{
		ft_putstr_fd("minishell: Shell verification failed\n", STDERR_FILENO);
		cleanup_shell(shell);
		return (ERROR);
	}
	shell_loop(shell);
	exit_status = shell->exit_status;
	cleanup_shell(shell);
	return (exit_status);
}

