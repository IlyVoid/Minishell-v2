/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:29:58 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:29:58 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Clean up command resources (tokens and commands)
 * @param shell Shell structure
 */
void	cleanup_command_resources(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
	if (shell->commands)
	{
		free_commands(shell->commands);
		shell->commands = NULL;
	}
}

/**
 * Clean up terminal state and history
 * @param shell Shell structure
 */
void	cleanup_terminal_state(t_shell *shell)
{
	if (!shell)
		return ;
	clear_history();
	restore_terminal(shell);
	reset_signals();
}

/**
 * Clean up active process resources
 * @param shell Shell structure
 */
void	cleanup_active_process(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->heredoc_active)
	{
		cleanup_interrupted_heredoc(shell);
		shell->heredoc_active = 0;
		if (shell->heredoc_file)
		{
			free(shell->heredoc_file);
			shell->heredoc_file = NULL;
		}
	}
	set_signal_mode(shell, 0);
}

/**
 * Clean up all shell resources
 * @param shell Shell structure to clean up
 */
void	cleanup_shell(t_shell *shell)
{
	if (!shell)
		return ;
	cleanup_command_resources(shell);
	if (shell->env_list)
		free_env(shell->env_list);
	cleanup_active_process(shell);
	cleanup_terminal_state(shell);
	free(shell);
}

