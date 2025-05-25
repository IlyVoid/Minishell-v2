/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:29:58 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:29:58 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Initialize environment variables for the shell
 * @param shell Shell structure
 * @param envp Environment variables array
 * @return SUCCESS or ERROR
 */
int	init_shell_env(t_shell *shell, char **envp)
{
	if (!shell)
		return (ERROR);
	shell->env_list = init_env(envp);
	if (!shell->env_list)
	{
		ft_putstr_fd("minishell: Failed to initialize environment\n",
			STDERR_FILENO);
		return (ERROR);
	}
	return (SUCCESS);
}

/**
 * Initialize terminal settings for the shell
 * @param shell Shell structure
 * @return SUCCESS or ERROR
 */
int	init_shell_terminal(t_shell *shell)
{
	if (!shell)
		return (ERROR);
	if (setup_terminal(shell) != SUCCESS)
		return (ERROR);
	return (SUCCESS);
}

/**
 * Verify shell state consistency
 * @param shell Shell structure to verify
 * @return SUCCESS if state is consistent, ERROR otherwise
 */
int	verify_shell_state(t_shell *shell)
{
	if (!shell)
		return (ERROR);
	if (shell->signal_state < 0 || shell->signal_state > 2)
	{
		shell->signal_state = 0;
		ft_putstr_fd("minishell: Warning: Invalid signal state\n", STDERR_FILENO);
	}
	if (shell->heredoc_active && !shell->heredoc_file)
	{
		shell->heredoc_active = 0;
		ft_putstr_fd("minishell: Warning: Inconsistent heredoc state\n",
			STDERR_FILENO);
	}
	if (!shell->env_list)
		return (ERROR);
	return (SUCCESS);
}

/**
 * Initialize the shell structure with default values
 * @param envp Environment variables array
 * @return Initialized shell structure or NULL on error
 */
t_shell	*init_shell(char **envp)
{
	t_shell	*shell;

	shell = (t_shell *)malloc(sizeof(t_shell));
	if (!shell)
		return (NULL);
	ft_memset(shell, 0, sizeof(t_shell));
	shell->exit_status = 0;
	shell->running = 1;
	if (init_shell_env(shell, envp) != SUCCESS)
	{
		free(shell);
		return (NULL);
	}
	if (init_shell_terminal(shell) != SUCCESS)
	{
		if (recover_terminal_error(shell) != SUCCESS)
		{
			free_env(shell->env_list);
			free(shell);
			return (NULL);
		}
	}
	return (shell);
}

