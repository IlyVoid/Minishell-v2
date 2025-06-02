/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_basic.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 17:58:00 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/* Built-in echo command - prints arguments with optional newline suppression */
int	builtin_echo(t_command *cmd, t_shell *shell)
{
	int	i;
	int	n_flag;

	(void)shell;
	if (!cmd || !cmd->args)
		return (ERROR);
	n_flag = 0;
	i = 1;
	if (cmd->args[i] && ft_strcmp(cmd->args[i], "-n") == 0)
	{
		n_flag = 1;
		i++;
	}
	while (cmd->args[i])
	{
		ft_putstr_fd(cmd->args[i], STDOUT_FILENO);
		if (cmd->args[i + 1])
			ft_putstr_fd(" ", STDOUT_FILENO);
		i++;
	}
	if (!n_flag)
		ft_putstr_fd("\n", STDOUT_FILENO);
	return (SUCCESS);
}

/* Built-in pwd command - prints current working directory */
int	builtin_pwd(t_command *cmd, t_shell *shell)
{
	char	current_dir[4096];

	(void)cmd;
	(void)shell;
	if (getcwd(current_dir, sizeof(current_dir)) == NULL)
	{
		print_error("pwd", NULL, NULL);
		return (ERROR);
	}
	ft_putendl_fd(current_dir, STDOUT_FILENO);
	return (SUCCESS);
}

