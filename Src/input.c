/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:29:58 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:29:58 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Check if a string contains only whitespace
 * @param str String to check
 * @return 1 if whitespace only, 0 otherwise
 */
static int	is_whitespace_only(char *str)
{
	int	i;

	if (!str)
		return (1);
	i = 0;
	while (str[i])
	{
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
			return (0);
		i++;
	}
	return (1);
}

/**
 * Handle parser errors
 * @param shell Shell structure
 * @param error_type Type of error
 */
void	handle_parse_error(t_shell *shell, int error_type)
{
	if (!shell)
		return ;
	if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
	if (error_type == SYNTAX_ERROR)
		ft_putstr_fd("minishell: syntax error\n", STDERR_FILENO);
	else
		ft_putstr_fd("minishell: parse error\n", STDERR_FILENO);
	shell->exit_status = error_type;
}

/**
 * Parse input into tokens and commands
 * @param input Input string
 * @param shell Shell structure
 * @return SUCCESS or error code
 */
int	parse_input(char *input, t_shell *shell)
{
	shell->tokens = tokenize_input(input);
	if (!shell->tokens)
		return (ERROR);
	if (expand_variables(shell->tokens, shell->env_list,
			shell->exit_status) != SUCCESS)
	{
		handle_parse_error(shell, ERROR);
		return (ERROR);
	}
	shell->commands = parse_tokens(shell->tokens, shell);
	if (!shell->commands)
	{
		handle_parse_error(shell, SYNTAX_ERROR);
		return (SYNTAX_ERROR);
	}
	return (SUCCESS);
}

/**
 * Execute parsed commands
 * @param shell Shell structure
 * @return Exit status of commands
 */
int	execute_input(t_shell *shell)
{
	int	status;

	set_signal_mode(shell, 1);
	status = execute_commands(shell->commands, shell);
	set_signal_mode(shell, 0);
	cleanup_command_resources(shell);
	return (status);
}

/**
 * Process a command line input
 * @param input Command line input
 * @param shell Shell structure
 * @return SUCCESS or error code
 */
int	process_input(char *input, t_shell *shell)
{
	int	status;

	if (!input || is_whitespace_only(input))
		return (SUCCESS);
	handle_history(input);
	status = parse_input(input, shell);
	if (status != SUCCESS)
		return (status);
	shell->exit_status = execute_input(shell);
	return (SUCCESS);
}

