/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_exit.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 18:13:15 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/* Check if a string is a valid numeric value */
int	is_numeric(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

/* Built-in exit command - exits the shell with optional status */
int	builtin_exit(t_command *cmd, t_shell *shell)
{
	int	exit_code;

	ft_putendl_fd("exit", STDOUT_FILENO);
	if (!cmd->args[1])
		exit_code = shell->exit_status;
	else if (!is_numeric(cmd->args[1]))
	{
		print_error("exit", cmd->args[1], "numeric argument required");
		exit_code = 255;
	}
	else if (cmd->args[2])
	{
		print_error("exit", NULL, "too many arguments");
		return (ERROR);
	}
	else
	{
		exit_code = ft_atoi(cmd->args[1]);
		exit_code = exit_code & 255;
	}
	shell->running = 0;
	return (exit_code);
}

