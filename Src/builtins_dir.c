/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_dir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 18:10:58 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/* Handles changing to HOME directory */
static int	cd_to_home(t_shell *shell)
{
	char	*path;

	path = get_env_value(shell->env_list, "HOME");
	if (!path)
	{
		print_error("cd", NULL, "HOME not set");
		return (ERROR);
	}
	return (chdir(path));
}

/* Handles changing to previous directory */
static int	cd_to_previous(t_shell *shell)
{
	char	*path;

	path = get_env_value(shell->env_list, "OLDPWD");
	if (!path)
	{
		print_error("cd", NULL, "OLDPWD not set");
		return (ERROR);
	}
	ft_putendl_fd(path, STDOUT_FILENO);
	return (chdir(path));
}

/* Updates PWD and OLDPWD environment variables */
static int	update_pwd_vars(t_shell *shell, char *old_pwd)
{
	char	current_dir[4096];

	if (getcwd(current_dir, sizeof(current_dir)) == NULL)
	{
		print_error("cd", NULL, "getcwd error");
		free(old_pwd);
		return (ERROR);
	}
	set_env_value(shell->env_list, "OLDPWD", old_pwd);
	set_env_value(shell->env_list, "PWD", current_dir);
	return (SUCCESS);
}

/* Built-in cd command - changes current directory */
int	builtin_cd(t_command *cmd, t_shell *shell)
{
	char	*old_pwd;
	char	current_dir[4096];

	if (getcwd(current_dir, sizeof(current_dir)) == NULL)
	{
		print_error("cd", NULL, "getcwd error");
		return (ERROR);
	}
	old_pwd = ft_strdup(current_dir);
	if (!old_pwd)
		return (ERROR);
	if (!cmd->args[1])
	{
		if (cd_to_home(shell) != 0)
		{
			free(old_pwd);
			return (ERROR);
		}
	}
	else if (ft_strcmp(cmd->args[1], "-") == 0)
	{
		if (cd_to_previous(shell) != 0)
		{
			free(old_pwd);
			return (ERROR);
		}
	}
	else if (chdir(cmd->args[1]) != 0)
	{
		print_error("cd", cmd->args[1], NULL);
		free(old_pwd);
		return (ERROR);
	}
	return (update_pwd_vars(shell, old_pwd));
}

