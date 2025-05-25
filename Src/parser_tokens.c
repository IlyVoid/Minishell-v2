/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:29:58 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:29:58 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Create a new token node
 * @param type Type of the token
 * @param value Value of the token
 * @return Newly created token node
 */
t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = ft_strdup(value);
	if (!token->value && value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

/**
 * Add a token to the end of the token list
 * @param tokens Pointer to the token list
 * @param new_token Token to add
 */
void	add_token(t_token **tokens, t_token *new_token)
{
	t_token	*current;

	if (!*tokens)
	{
		*tokens = new_token;
		return ;
	}
	current = *tokens;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

/**
 * Free all tokens in the token list
 * @param tokens Token list to free
 */
void	free_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
}

/**
 * Parse a word token
 * @param input Input string
 * @param i Pointer to the current position in the input
 * @return Parsed word token or NULL on error
 */
char	*parse_word(char *input, int *i)
{
	char	*value;
	int		start;
	int		len;

	start = *i;
	value = NULL;
	while (input[*i] && !is_delimiter(input[*i]))
	{
		if (input[*i] == '\'' || input[*i] == '\"')
		{
			len = *i - start;
			if (len > 0)
				value = ft_substr(input, start, len);
			else
				value = ft_strdup("");
			if (!handle_quoted_word(&value, input, i))
				return (NULL);
			start = *i;
		}
		else
			(*i)++;
	}
	return (finalize_word(value, input, start, *i));
}

/**
 * Tokenize the input string
 * @param input Input string to tokenize
 * @return List of tokens or NULL on error
 */
t_token	*tokenize_input(char *input)
{
	t_token		*tokens;
	t_token		*new_token;
	int			i;
	char		*value;

	if (!input)
		return (NULL);
	tokens = NULL;
	i = 0;
	while (input[i])
	{
		if (is_whitespace(input[i]))
			i++;
		else if (input[i] == '|' || input[i] == '<' || input[i] == '>')
		{
			new_token = handle_operator_token(input, &i);
			if (!new_token || !add_token_safely(&tokens, new_token))
				return (free_tokens_return_null(tokens));
		}
		else if (!handle_word_token(input, &i, &tokens))
			return (NULL);
	}
	return (tokens);
}

