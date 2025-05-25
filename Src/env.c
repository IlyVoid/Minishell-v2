/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quint <quint@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:53:02 by quint             #+#    #+#             */
/*   Updated: 2025/05/25 11:53:02 by quint            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Create a new environment variable node
 * @param key Environment variable name
 * @param value Environment variable value
 * @return Newly created environment node
 */
static t_env	*create_env_node(char *key, char *value)
{
	t_env	*new_node;

	new_node = (t_env *)malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = ft_strdup(key);
	if (!new_node->key)
	{
		free(new_node);
		return (NULL);
	}
	if (value)
	{
		new_node->value = ft_strdup(value);
		if (!new_node->value)
		{
			free(new_node->key);
			free(new_node);
			return (NULL);
		}
	}
	else
		new_node->value = NULL;
	new_node->next = NULL;
	return (new_node);
}

/**
 * Parse a single environment variable string into key and value
 * @param env_str Environment variable string in format KEY=VALUE
 * @param key Pointer to store the extracted key
 * @param value Pointer to store the extracted value
 * @return Success or error code
 */
static int	parse_env_var(char *env_str, char **key, char **value)
{
	int		i;
	char	*equal_sign;

	if (!env_str || !key || !value)
		return (ERROR);
	
	equal_sign = strchr(env_str, '=');
	if (!equal_sign)
	{
		*key = ft_strdup(env_str);
		*value = NULL;
		return (SUCCESS);
	}
	
	i = equal_sign - env_str;
	*key = ft_substr(env_str, 0, i);
	if (!(*key))
		return (ERROR);
	
	*value = ft_strdup(equal_sign + 1);
	if (!(*value))
	{
		free(*key);
		return (ERROR);
	}
	
	return (SUCCESS);
}

/**
 * Initialize environment variables from envp array
 * @param envp Array of environment variable strings
 * @return Linked list of environment variables
 */
t_env	*init_env(char **envp)
{
	t_env	*env_list;
	t_env	*current;
	t_env	*new_node;
	char	*key;
	char	*value;
	int		i;

	if (!envp || !envp[0])
		return (NULL);
	
	env_list = NULL;
	i = 0;
	while (envp[i])
	{
		if (parse_env_var(envp[i], &key, &value) == ERROR)
		{
			free_env(env_list);
			return (NULL);
		}
		
		new_node = create_env_node(key, value);
		free(key);
		free(value);
		
		if (!new_node)
		{
			free_env(env_list);
			return (NULL);
		}
		
		if (!env_list)
			env_list = new_node;
		else
		{
			current = env_list;
			while (current->next)
				current = current->next;
			current->next = new_node;
		}
		i++;
	}
	
	return (env_list);
}

/**
 * Free all memory associated with environment variables
 * @param env_list Linked list of environment variables
 */
void	free_env(t_env *env_list)
{
	t_env	*current;
	t_env	*next;

	current = env_list;
	while (current)
	{
		next = current->next;
		if (current->key)
			free(current->key);
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
}

/**
 * Get the value of an environment variable
 * @param env_list Linked list of environment variables
 * @param key Name of the environment variable to retrieve
 * @return Value of the environment variable or NULL if not found
 */
char	*get_env_value(t_env *env_list, char *key)
{
	t_env	*current;

	if (!env_list || !key)
		return (NULL);
	
	current = env_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
			return (current->value);
		current = current->next;
	}
	
	return (NULL);
}

/**
 * Set or update an environment variable
 * @param env_list Linked list of environment variables
 * @param key Name of the environment variable to set
 * @param value Value to assign to the environment variable
 * @return Success or error code
 */
int	set_env_value(t_env *env_list, char *key, char *value)
{
	t_env	*current;
	t_env	*new_node;
	t_env	*last;

	if (!env_list || !key)
		return (ERROR);
	
	current = env_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (current->value)
				free(current->value);
			if (value)
				current->value = ft_strdup(value);
			else
				current->value = NULL;
			return (SUCCESS);
		}
		if (!current->next)
			last = current;
		current = current->next;
	}
	
	new_node = create_env_node(key, value);
	if (!new_node)
		return (ERROR);
	
	last->next = new_node;
	return (SUCCESS);
}

/**
 * Remove an environment variable
 * @param env_list Pointer to the linked list of environment variables
 * @param key Name of the environment variable to remove
 * @return Success or error code
 */
int	unset_env_value(t_env **env_list, char *key)
{
	t_env	*current;
	t_env	*prev;
	t_env	*to_remove;

	if (!env_list || !*env_list || !key)
		return (ERROR);
	
	prev = NULL;
	current = *env_list;
	
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*env_list = current->next;
			
			to_remove = current;
			free(to_remove->key);
			if (to_remove->value)
				free(to_remove->value);
			free(to_remove);
			
			return (SUCCESS);
		}
		prev = current;
		current = current->next;
	}
	
	return (ERROR);
}

/**
 * Count the number of environment variables in the list
 * @param env_list Linked list of environment variables
 * @return Number of environment variables
 */
static int	count_env_vars(t_env *env_list)
{
	int		count;
	t_env	*current;

	count = 0;
	current = env_list;
	while (current)
	{
		count++;
		current = current->next;
	}
	
	return (count);
}

/**
 * Convert environment linked list to array format
 * @param env_list Linked list of environment variables
 * @return Array of environment variable strings in format KEY=VALUE
 */
char	**env_to_array(t_env *env_list)
{
	char	**env_array;
	t_env	*current;
	char	*tmp;
	int		count;
	int		i;

	count = count_env_vars(env_list);
	env_array = (char **)malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	
	current = env_list;
	i = 0;
	while (current)
	{
		if (current->value)
		{
			tmp = ft_strjoin(current->key, "=");
			if (!tmp)
			{
				while (--i >= 0)
					free(env_array[i]);
				free(env_array);
				return (NULL);
			}
			
			env_array[i] = ft_strjoin(tmp, current->value);
			free(tmp);
		}
		else
			env_array[i] = ft_strdup(current->key);
		
		if (!env_array[i])
		{
			while (--i >= 0)
				free(env_array[i]);
			free(env_array);
			return (NULL);
		}
		
		current = current->next;
		i++;
	}
	
	env_array[i] = NULL;
	return (env_array);
}

