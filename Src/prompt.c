/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:29:58 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:29:58 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Get the current directory for display in prompt
 * @return Formatted directory string or NULL on error
 */
char	*get_current_dir(void)
{
	char	cwd[4096];
	char	*dir_name;

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (ft_strdup("(unknown)"));
	dir_name = strrchr(cwd, '/');
	if (dir_name && *(dir_name + 1) != '\0')
		return (ft_strdup(dir_name + 1));
	else if (dir_name && *(dir_name + 1) == '\0')
		return (ft_strdup("/"));
	else
		return (ft_strdup(cwd));
}

/**
 * Format the prompt string with directory and status
 * @param username Username from environment
 * @param dir Current directory name
 * @param status Exit status of last command
 * @return Formatted prompt string or NULL on error
 */
char	*format_prompt_string(char *username, char *dir, int status)
{
	char	*prompt;
	char	*status_indicator;
	size_t	prompt_size;

	if (status == 0)
		status_indicator = "\033[32m✓\033[0m";
	else
		status_indicator = "\033[31m✗\033[0m";
	prompt_size = ft_strlen(username) + ft_strlen(dir) + 
		ft_strlen(status_indicator) + 40;
	prompt = ft_malloc(prompt_size);
	if (!prompt)
		return (NULL);
	if (snprintf(prompt, prompt_size, "\033[36m%s\033[0m:\033[34m%s\033[0m %s $ ", 
		username, dir, status_indicator) < 0)
	{
		free(prompt);
		return (NULL);
	}
	return (prompt);
}

/**
 * Create a custom prompt with username, directory, and exit status
 * @param shell Shell structure
 * @return Dynamically allocated prompt string, or NULL on error
 */
char	*create_prompt(t_shell *shell)
{
	char	*username;
	char	*dir;
	char	*prompt;

	username = get_env_value(shell->env_list, "USER");
	if (!username)
		username = "user";
	dir = get_current_dir();
	if (!dir)
		return (ft_strdup("minishell$ "));
	prompt = format_prompt_string(username, dir, shell->exit_status);
	free(dir);
	if (!prompt)
		return (ft_strdup("minishell$ "));
	return (prompt);
}

/**
 * Display shell prompt and read input
 * @param shell Shell structure
 * @return Command line input string or NULL on EOF/error
 */
char	*get_shell_input(t_shell *shell)
{
	char	*prompt;
	char	*input;

	prompt = create_prompt(shell);
	if (!prompt)
		prompt = ft_strdup("minishell$ ");
	if (!prompt)
		prompt = "minishell$ ";
	set_signal_mode(shell, 0);
	input = readline(prompt);
	if (prompt && prompt != "minishell$ ")
		free(prompt);
	if (!input)
	{
		ft_putendl_fd("exit", STDOUT_FILENO);
		shell->running = 0;
		return (NULL);
	}
	return (input);
}

