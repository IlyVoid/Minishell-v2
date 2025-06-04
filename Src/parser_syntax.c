/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:29:58 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:29:58 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Check if a character is a whitespace
 * @param c Character to check
 * @return 1 if the character is a whitespace, 0 otherwise
 */
int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f'
		|| c == '\r');
}

/**
 * Parse a quoted string
 * @param input Input string
 * @param i Pointer to the current position in the input
 * @param quote Quote character (' or ")
 * @return Parsed quoted string or NULL on error
 */
char	*parse_quoted_string(char *input, int *i, char quote)
{
	char	*value;
	int		start;
	int		len;

	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != quote)
		(*i)++;
	if (!input[*i])
	{
		ft_putstr_fd("minishell: syntax error: unclosed quote\n", STDERR_FILENO);
		return (NULL);
	}
	len = *i - start;
	value = ft_substr(input, start, len);
	(*i)++;
	return (value);
}

/**
 * Handle quoted string parsing
 * @param value Current value to append to
 * @param input Input string
 * @param i Pointer to current position
 * @return SUCCESS or ERROR
 */
int	handle_quoted_word(char **value, char *input, int *i)
{
	char	quote;
	char	*quoted;
	char	*tmp;

	quote = input[*i];
	quoted = parse_quoted_string(input, i, quote);
	if (!quoted)
	{
		free(*value);
		*value = NULL;
		return (0);
	}
	tmp = ft_strjoin(*value, quoted);
	free(*value);
	free(quoted);
	if (!tmp)
		return (0);
	*value = tmp;
	return (1);
}

/**
 * Finalize a word token by handling the remaining part
 * @param value Current value (may be NULL)
 * @param input Input string
 * @param start Start position
 * @param end End position
 * @return Completed word or NULL on error
 */
char	*finalize_word(char *value, char *input, int start, int end)
{
	char	*tmp;
	char	*result;
	int		len;

	len = end - start;
	if (value == NULL)
	{
		if (len == 0)
			return (ft_strdup(""));
		return (ft_substr(input, start, len));
	}
	if (len > 0)
	{
		tmp = ft_substr(input, start, len);
		result = ft_strjoin(value, tmp);
		free(value);
		free(tmp);
		return (result);
	}
	return (value);
}

/**
 * Parse a redirection operator
 * @param input Input string
 * @param i Pointer to the current position in the input
 * @return Token type of the redirection
 */
t_token_type	parse_operator(char *input, int *i)
{
	t_token_type	type;

	if (input[*i] == '<')
	{
		(*i)++;
		if (input[*i] == '<')
		{
			type = TOKEN_HEREDOC;
			(*i)++;
		}
		else
			type = TOKEN_REDIRECT_IN;
	}
	else if (input[*i] == '>')
	{
		(*i)++;
		if (input[*i] == '>')
		{
			type = TOKEN_REDIRECT_APPEND;
			(*i)++;
		}
		else
			type = TOKEN_REDIRECT_OUT;
	}
	else
	{
		type = TOKEN_PIPE;
		(*i)++;
	}
	return (type);
}

/**
 * Validate operator syntax
 * @param token Current token to check
 * @return 1 if valid, 0 if invalid
 */
int	validate_operator_syntax(t_token *token)
{
	if (token->type == TOKEN_PIPE &&
		(!token->next || token->next->type == TOKEN_PIPE))
	{
		syntax_error("|");
		return (0);
	}
	if ((token->type == TOKEN_REDIRECT_IN ||
		token->type == TOKEN_REDIRECT_OUT ||
		token->type == TOKEN_REDIRECT_APPEND ||
		token->type == TOKEN_HEREDOC) &&
		(!token->next || token->next->type != TOKEN_WORD))
	{
		syntax_error("newline");
		return (0);
	}
	return (1);
}

/**
 * Validate token syntax
 * @param tokens Token list to validate
 * @return SUCCESS or ERROR
 */
int	validate_syntax(t_token *tokens)
{
	t_token	*current;
	int		expecting_word;

	if (!tokens)
		return (ERROR);
	current = tokens;
	expecting_word = 0;
	while (current)
	{
		if (current->type != TOKEN_WORD && expecting_word)
		{
			syntax_error(current->value);
			return (ERROR);
		}
		if (current->type != TOKEN_WORD)
			expecting_word = 1;
		else if (expecting_word)
			expecting_word = 0;
		if (!validate_operator_syntax(current))
			return (ERROR);
		current = current->next;
	}
	if (expecting_word)
	{
		syntax_error(NULL);
		return (ERROR);
	}
	return (SUCCESS);
}