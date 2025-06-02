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

/* Checks if a variable name is valid (starts with letter/_, contains alnum/_) */
int	is_valid_variable_name(char *var)
{
	int	i;

	if (!var || !*var)
		return (0);
	if (!ft_isalpha(var[0]) && var[0] != '_')
		return (0);
	i = 1;
	while (var[i])
	{
		if (!ft_isalnum(var[i]) && var[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/* Parse a variable assignment string (KEY=VALUE) */
int	parse_variable_assignment(char *arg, char **key, char **value)
{
	char	*equal_sign;
	int		key_len;

	equal_sign = strchr(arg, '=');
	if (!equal_sign)
	{
		*key = ft_strdup(arg);
		if (!*key)
			return (ERROR);
		*value = NULL;
		return (SUCCESS);
	}
	key_len = equal_sign - arg;
	*key = ft_substr(arg, 0, key_len);
	if (!*key)
		return (ERROR);
	*value = ft_strdup(equal_sign + 1);
	if (!*value)
	{
		free(*key);
		return (ERROR);
	}
	return (SUCCESS);
}

