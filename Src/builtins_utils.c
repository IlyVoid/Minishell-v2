/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:58:00 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 17:58:00 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Checks if a variable name is valid (starts with letter/_, contains alnum/_)
 * @param var Variable name to check
 * @return 1 if valid, 0 otherwise
 */
int	is_valid_variable_name(char *var)
{
	int	i;
	int	len;

	if (!var || !*var)
		return (0);
		
	// Check variable name length (POSIX standard is 255)
	len = ft_strlen(var);
	if (len > 255)
		return (0);
		
	// First character must be a letter or underscore
	if (!ft_isalpha(var[0]) && var[0] != '_')
		return (0);
		
	// Rest must be alphanumeric or underscore
	i = 1;
	while (var[i])
	{
		if (!ft_isalnum(var[i]) && var[i] != '_')
			return (0);
		i++;
	}
	
	return (1);
}

/**
 * Parse a variable assignment string (KEY=VALUE)
 * @param arg String to parse
 * @param key Pointer to store the key
 * @param value Pointer to store the value
 * @return SUCCESS or ERROR
 */
int	parse_variable_assignment(char *arg, char **key, char **value)
{
	char	*equal_sign;
	int		key_len;
	int		value_len;

	if (!arg || !key || !value)
		return (ERROR);
		
	// Use ft_strchr for consistency with other code
	equal_sign = ft_strchr(arg, '=');
	
	// Handle case with no equal sign (no value)
	if (!equal_sign)
	{
		// Check key length
		if (ft_strlen(arg) > 255)
			return (ERROR);
			
		*key = ft_strdup(arg);
		if (!*key)
			return (ERROR);
		*value = NULL;
		return (SUCCESS);
	}
	
	// Calculate lengths and validate
	key_len = equal_sign - arg;
	if (key_len > 255 || key_len == 0)
		return (ERROR);
		
	value_len = ft_strlen(equal_sign + 1);
	if (value_len > 4096) // Reasonable limit for environment variable value
		return (ERROR);
	
	// Extract key and value
	*key = ft_substr(arg, 0, key_len);
	if (!*key)
		return (ERROR);
		
	*value = ft_strdup(equal_sign + 1);
	if (!*value)
	{
		free(*key);
		*key = NULL;
		return (ERROR);
	}
	
	return (SUCCESS);
}

