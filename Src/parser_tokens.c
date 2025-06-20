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

t_token	*handle_operator_token(const char *str, int *index)
{
	t_token	*token;
	char	op[3] = {0};
	int		len = 0;

	if ((str[*index] == '<' && str[*index + 1] == '<')
		|| (str[*index] == '>' && str[*index + 1] == '>'))
	{
		op[0] = str[*index];
		op[1] = str[*index + 1];
		len = 2;
	}
	else if (str[*index] == '<' || str[*index] == '>' || str[*index] == '|')
	{
		op[0] = str[*index];
		len = 1;
	}
	else
		return (NULL);
	*index += len;
	token = create_token(TOKEN_OPERATOR, op);
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

int	add_token_safely(t_token **tokens, t_token *new_token)
{
	if (!new_token)
		return (0);
	add_token(tokens, new_token);
	return (1);
}

t_token	*free_tokens_return_null(t_token *tokens)
{
	free_tokens(tokens);
	return (NULL);
}

int	handle_word_token(char *input, int *i, t_token **tokens)
{
	char	*value;
	t_token	*new_token;

	value = parse_word(input, i);
	if (!value)
		return (0);
	new_token = create_token(TOKEN_WORD, value);
	free(value);
	if (!new_token || !add_token_safely(tokens, new_token))
		return (0);
	return (1);
}