/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 17:58:00 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/* Print all environment variables in export format */
static void	print_exported_env(t_env *env_list)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(current->key, STDOUT_FILENO);
		if (current->value)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(current->value, STDOUT_FILENO);
			ft_putstr_fd("\"", STDOUT_FILENO);
		}
		ft_putstr_fd("\n", STDOUT_FILENO);
		current = current->next;
	}
}

/* Built-in export command - sets environment variables */
int	builtin_export(t_command *cmd, t_shell *shell)
{
	int		i;
	char	*key;
	char	*value;
	int		status;

	status = SUCCESS;
	if (!cmd->args[1])
	{
		print_exported_env(shell->env_list);
		return (SUCCESS);
	}
	i = 1;
	while (cmd->args[i])
	{
		if (parse_variable_assignment(cmd->args[i], &key, &value) == ERROR)
			return (ERROR);
		if (!is_valid_variable_name(key))
		{
			print_error("export", cmd->args[i], "not a valid identifier");
			free(key);
			if (value)
				free(value);
			status = ERROR;
		}
		else
		{
			if (set_env_value(shell->env_list, key, value) == ERROR)
				status = ERROR;
			free(key);
			if (value)
				free(value);
		}
		i++;
	}
	return (status);
}

/* Built-in unset command - removes environment variables */
int	builtin_unset(t_command *cmd, t_shell *shell)
{
	int	i;
	int	status;

	status = SUCCESS;
	if (!cmd->args[1])
		return (SUCCESS);
	i = 1;
	while (cmd->args[i])
	{
		if (!is_valid_variable_name(cmd->args[i]))
		{
			print_error("unset", cmd->args[i], "not a valid identifier");
			status = ERROR;
		}
		else if (unset_env_value(&shell->env_list, cmd->args[i]) == ERROR)
			status = ERROR;
		i++;
	}
	return (status);
}

/* Built-in env command - displays environment variables */
int	builtin_env(t_command *cmd, t_shell *shell)
{
	t_env	*current;

	(void)cmd;
	current = shell->env_list;
	while (current)
	{
		if (current->value)
		{
			ft_putstr_fd(current->key, STDOUT_FILENO);
			ft_putstr_fd("=", STDOUT_FILENO);
			ft_putstr_fd(current->value, STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
		current = current->next;
	}
	return (SUCCESS);
}

