/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:53:02 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 11:53:02 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Create a new token node
 * @param type Type of the token
 * @param value Value of the token
 * @return Newly created token node
 */

static t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (NULL);
		
	token->type = type;
	token->next = NULL;
	
	// Handle value separately
	if (value)
	{
		token->value = ft_strdup(value);
		if (!token->value)
		{
			free(token);
			return (NULL);
		}
	}
	else
	{
		token->value = NULL;
	}
	
	return (token);
}

/**
 * Add a token to the end of the token list
 * @param tokens Pointer to the token list
 * @param new_token Token to add
 */
static void	add_token(t_token **tokens, t_token *new_token)
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
 * Check if a character is a token delimiter
 * @param c Character to check
 * @return 1 if the character is a delimiter, 0 otherwise
 */
int	is_delimiter(char c)
{
	return (is_whitespace(c) || c == '|' || c == '<' || c == '>' || c == '\0');
}

/**
 * Parse a quoted string
 * @param input Input string
 * @param i Pointer to the current position in the input
 * @param quote Quote character (' or ")
 * @return Parsed quoted string or NULL on error
 */
static char	*parse_quoted_string(char *input, int *i, char quote)
{
	char	*value;
	int		start;
	int		len;

	if (!input || !i)
		return (NULL);
		
	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != quote)
		(*i)++;
		
	if (!input[*i])
	{
		ft_putstr_fd("minishell: syntax error: unclosed ", STDERR_FILENO);
		ft_putchar_fd(quote, STDERR_FILENO);
		ft_putstr_fd(" quote\n", STDERR_FILENO);
		return (NULL);
	}
	
	len = *i - start;
	if (len > 4096) // Reasonable maximum for quoted string
	{
		ft_putstr_fd("minishell: quoted string too long\n", STDERR_FILENO);
		return (NULL);
	}
	
	value = ft_substr(input, start, len);
	if (!value)
	{
		ft_putstr_fd("minishell: memory allocation failed\n", STDERR_FILENO);
		return (NULL);
	}
	
	(*i)++;
	return (value);
}

/**
 * Parse a word token
 * @param input Input string
 * @param i Pointer to the current position in the input
 * @return Parsed word token or NULL on error
 */
static char	*parse_word(char *input, int *i)
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
			char	*quoted;
			char	*tmp;
			
			len = *i - start;
			value = ft_substr(input, start, len);
			if (!value)
				return (NULL);
			
			quoted = parse_quoted_string(input, i, input[*i - 1]);
			if (!quoted)
			{
				free(value);
				return (NULL);
			}
			
			tmp = ft_strjoin(value, quoted);
			free(value);
			free(quoted);
			
			if (!tmp)
				return (NULL);
			
			start = *i;
			value = tmp;
		}
		else
			(*i)++;
	}
	
	if (*i == start)
		return (ft_strdup(""));
	
	len = *i - start;
	if (value)
	{
		char *tmp = ft_substr(input, start, len);
		char *result = ft_strjoin(value, tmp);
		free(value);
		free(tmp);
		return (result);
	}
	
	return (ft_substr(input, start, len));
}

/**
 * Parse a redirection token
 * @param input Input string
 * @param i Pointer to the current position in the input
 * @return Token type of the redirection
 */
static t_token_type	parse_redirection(char *input, int *i)
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
	t_token_type	type;

	if (!input)
		return (NULL);
		
	// Check for maximum command length to prevent buffer issues
	if (ft_strlen(input) > 10000) // Reasonable maximum for command line
	{
		ft_putstr_fd("minishell: command line too long\n", STDERR_FILENO);
		// No cleanup needed since no memory has been allocated yet
		return (NULL);
	}
	
	tokens = NULL;
	i = 0;
	while (input[i])
	{
		if (is_whitespace(input[i]))
		{
			i++;
			continue ;
		}
		else if (input[i] == '|' || input[i] == '<' || input[i] == '>')
		{
			type = parse_redirection(input, &i);
			new_token = create_token(type, NULL);
		}
		else
		{
			value = parse_word(input, &i);
			if (!value)
			{
				free_tokens(tokens);
				return (NULL);
			}
			new_token = create_token(TOKEN_WORD, value);
			free(value);
		}
		
		if (!new_token)
		{
			free_tokens(tokens);
			return (NULL);
		}
		add_token(&tokens, new_token);
	}
	return (tokens);
}

/**
 * Create a new redirection
 * @param type Type of the redirection
 * @param file File for the redirection
 * @return Newly created redirection
 */
static t_redirection	*create_redirection(t_token_type type, char *file)
{
	t_redirection	*redirection;

	redirection = (t_redirection *)malloc(sizeof(t_redirection));
	if (!redirection)
		return (NULL);
	redirection->type = type;
	redirection->file = ft_strdup(file);
	if (!redirection->file)
	{
		free(redirection);
		return (NULL);
	}
	redirection->next = NULL;
	return (redirection);
}

/**
 * Add a redirection to a command
 * @param cmd Command to add the redirection to
 * @param type Type of the redirection
 * @param file File for the redirection
 * @return Success or error code
 */
static int	add_redirection(t_command *cmd, t_token_type type, char *file)
{
	t_redirection	*redirection;
	t_redirection	*current;
	int				redir_count = 0;

	if (!cmd || !file)
		return (ERROR);
		
	// Check filename length
	if (ft_strlen(file) > 255)
	{
		ft_putstr_fd("minishell: filename too long\n", STDERR_FILENO);
		return (ERROR);
	}
	
	// Check for pipe character in filename (basic validation)
	if (ft_strchr(file, '|'))
	{
		ft_putstr_fd("minishell: invalid character in redirection filename\n", STDERR_FILENO);
		return (ERROR);
	}
	
	redirection = create_redirection(type, file);
	if (!redirection)
		return (ERROR);
	
	if (!cmd->redirections)
	{
		cmd->redirections = redirection;
		return (SUCCESS);
	}
	
	current = cmd->redirections;
	redir_count = 1;
	
	while (current->next)
	{
		current = current->next;
		redir_count++;
		
		// Limit number of redirections
		if (redir_count > 16)
		{
			ft_putstr_fd("minishell: too many redirections\n", STDERR_FILENO);
			free(redirection->file);
			free(redirection);
			return (ERROR);
		}
	}
	
	current->next = redirection;
	return (SUCCESS);
}

/**
 * Create a new command
 * @return Newly created command
 */
static t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = (t_command *)malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirections = NULL;
	cmd->next = NULL;
	cmd->pipe_out = 0;
	return (cmd);
}

/**
 * Add an argument to a command
 * @param cmd Command to add the argument to
 * @param arg Argument to add
 * @return Success or error code
 */
static int	add_argument(t_command *cmd, char *arg)
{
	char	**new_args;
	int		i;

	if (!cmd || !arg || !*arg)
		return (SUCCESS);
	
	// Check for maximum argument length
	if (ft_strlen(arg) > 4096)
	{
		ft_putstr_fd("minishell: argument too long\n", STDERR_FILENO);
		return (ERROR);
	}
	
	if (!cmd->args)
	{
		cmd->args = (char **)malloc(sizeof(char *) * 2);
		if (!cmd->args)
			return (ERROR);
		cmd->args[0] = ft_strdup(arg);
		if (!cmd->args[0])
		{
			free(cmd->args);
			cmd->args = NULL;
			return (ERROR);
		}
		cmd->args[1] = NULL;
		return (SUCCESS);
	}
	
	i = 0;
	while (cmd->args[i])
		i++;
	
	// Check for maximum arguments
	if (i >= 1024)
	{
		ft_putstr_fd("minishell: too many arguments\n", STDERR_FILENO);
		return (ERROR);
	}
	
	new_args = (char **)malloc(sizeof(char *) * (i + 2));
	if (!new_args)
		return (ERROR);
	
	i = 0;
	while (cmd->args[i])
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	
	new_args[i] = ft_strdup(arg);
	if (!new_args[i])
	{
		// Clean up on error
		int j = 0;
		while (j < i)
		{
			new_args[j] = NULL; // Don't free these - they're still in cmd->args
			j++;
		}
		free(new_args);
		return (ERROR);
	}
	
	new_args[i + 1] = NULL;
	free(cmd->args);
	cmd->args = new_args;
	
	return (SUCCESS);
}

/**
 * Check if the token list has valid syntax
 * @param tokens Token list to check
 * @return Success or error code
 */
static int	validate_syntax(t_token *tokens)
{
	t_token	*current;
	int		expecting_word;
	int		pipe_count;

	if (!tokens)
		return (ERROR);
	
	current = tokens;
	expecting_word = 0;
	pipe_count = 0;
	
	// Check if first token is a pipe
	if (current->type == TOKEN_PIPE)
	{
		syntax_error("|");
		return (ERROR);
	}
	
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
		
		// Validate pipe syntax
		if (current->type == TOKEN_PIPE)
		{
			pipe_count++;
			if (pipe_count > 16) // More reasonable limit for a basic shell
			{
				ft_putstr_fd("minishell: too many pipes\n", STDERR_FILENO);
				return (ERROR);
			}
			
			if (!current->next || current->next->type == TOKEN_PIPE)
			{
				syntax_error("|");
				return (ERROR);
			}
		}
		
		// Additional syntax validation for redirections
		if ((current->type == TOKEN_REDIRECT_IN || 
			 current->type == TOKEN_REDIRECT_OUT || 
			 current->type == TOKEN_REDIRECT_APPEND || 
			 current->type == TOKEN_HEREDOC) && 
			(!current->next || current->next->type != TOKEN_WORD))
		{
			syntax_error("newline");
			return (ERROR);
		}
		
		current = current->next;
	}
	
	if (expecting_word)
	{
		syntax_error(NULL);
		return (ERROR);
	}
	
	return (SUCCESS);
}

/**
 * Free a command and all its resources
 * @param cmd Command to free
 */
static void	free_command(t_command *cmd)
{
	t_redirection	*current_redir;
	t_redirection	*next_redir;
	int				i;

	if (!cmd)
		return ;
	
	if (cmd->args)
	{
		i = 0;
		while (cmd->args[i])
		{
			free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
	}
	
	current_redir = cmd->redirections;
	while (current_redir)
	{
		next_redir = current_redir->next;
		if (current_redir->file)
			free(current_redir->file);
		free(current_redir);
		current_redir = next_redir;
	}
	
	free(cmd);
}

/**
 * Free all commands in a command list
 * @param commands Command list to free
 */
void	free_commands(t_command *commands)
{
	t_command	*current;
	t_command	*next;

	current = commands;
	while (current)
	{
		next = current->next;
		free_command(current);
		current = next;
	}
}

/**
 * Parse tokens into commands
 * @param tokens Token list to parse
 * @param shell Shell structure containing environment and state
 * @return Command list or NULL on error
 */
t_command	*parse_tokens(t_token *tokens, t_shell *shell)
{
	t_command	*commands;
	t_command	*current_cmd;
	t_token		*current_token;

	if (!tokens || validate_syntax(tokens) != SUCCESS)
		return (NULL);
	
	commands = NULL;
	current_cmd = NULL;
	current_token = tokens;
	
	while (current_token)
	{
		if (!current_cmd)
		{
			current_cmd = create_command();
			if (!current_cmd)
			{
				free_commands(commands);
				return (NULL);
			}
			
			if (!commands)
				commands = current_cmd;
			else
			{
				t_command *tmp = commands;
				while (tmp->next)
					tmp = tmp->next;
				tmp->next = current_cmd;
				tmp->pipe_out = 1;
			}
		}
		
		if (current_token->type == TOKEN_WORD)
		{
			if (add_argument(current_cmd, current_token->value) != SUCCESS)
			{
				free_commands(commands);
				return (NULL);
			}
		}
		else if (current_token->type == TOKEN_PIPE)
		{
			current_cmd = NULL;
		}
		else
		{
			t_token_type redir_type = current_token->type;
			current_token = current_token->next;
			
			if (!current_token || current_token->type != TOKEN_WORD)
			{
				free_commands(commands);
				return (NULL);
			}
			
			// Handle heredoc specially
			if (redir_type == TOKEN_HEREDOC)
			{
				// Set up heredoc signal handling
				setup_heredoc_signals();
				
				char *heredoc_file = handle_heredoc(current_token->value, 
					shell->env_list, shell->exit_status);
				
				// Reset signal handling for interactive mode
				// Always restore signals, regardless of heredoc success
				setup_signals();
				
				// Check if heredoc was interrupted by signal
				if (g_received_signal)
				{
					shell->exit_status = 128 + g_received_signal;
					g_received_signal = 0;
					free_commands(commands);
					return (NULL);
				}
				
				if (!heredoc_file)
				{
					free_commands(commands);
					return (NULL);
				}
				
				// Add as a regular input redirection but with the temp file
				if (add_redirection(current_cmd, TOKEN_REDIRECT_IN, heredoc_file) != SUCCESS)
				{
					// Always cleanup the heredoc file on error
					cleanup_heredoc(heredoc_file);
					free(heredoc_file);
					free_commands(commands);
					// Ensure signals are reset (redundant but safe)
					setup_signals();
					return (NULL);
				}
				
				// We need to free the filename but the file itself will be cleaned up
				// after command execution or on error
				free(heredoc_file);
			}
			else if (add_redirection(current_cmd, redir_type, current_token->value) != SUCCESS)
			{
				free_commands(commands);
				return (NULL);
			}
		}
		
		current_token = current_token->next;
	}
	
	// Basic validation of commands
	if (commands)
	{
		t_command *cmd = commands;
		while (cmd)
		{
			// Check if any command has empty arguments after redirections
			if (!cmd->args || !cmd->args[0])
			{
				if (cmd->redirections)
				{
					// If it has redirections, add a dummy command
					if (add_argument(cmd, "") != SUCCESS)
					{
						free_commands(commands);
						return (NULL);
					}
				}
			}
			cmd = cmd->next;
		}
	}
	
	return (commands);
}

/**
 * Check if a character is valid in a variable name
 * @param c Character to check
 * @return 1 if the character is valid, 0 otherwise
 */
static int	is_valid_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

/**
 * Extract a variable name from a string
 * @param str String containing the variable name
 * @return Variable name or NULL on error
 */
static char	*extract_var_name(char *str)
{
	int		i;
	char	*var_name;

	if (!str)
		return (NULL);
		
	i = 0;
	while (str[i] && is_valid_var_char(str[i]))
		i++;
	
	// Limit variable name length to avoid excessive memory usage
	if (i > 256)
	{
		ft_putstr_fd("minishell: variable name too long\n", STDERR_FILENO);
		return (NULL);
	}
	
	var_name = ft_substr(str, 0, i);
	if (!var_name)
		return (NULL);
	
	return (var_name);
}

/**
 * Expand a variable in a token value
 * @param token Token to expand
 * @param var_pos Position of the variable in the token value
 * @param env_list Environment variable list
 * @param exit_status Last command exit status
 * @return Success or error code
 */
static int	expand_var(t_token *token, int var_pos, t_env *env_list, int exit_status)
{
	char	*before = NULL;
	char	*var_name = NULL;
	char	*var_value = NULL;
	char	*after = NULL;
	char	*result = NULL;
	int		i;

	// Extract the part before the variable
	before = ft_substr(token->value, 0, var_pos);
	if (!before)
		return (ERROR);
	
	// Handle special case for $?
	if (token->value[var_pos + 1] == '?')
	{
		var_value = ft_itoa(exit_status);
		i = 2;
	}
	else
	{
		// Extract variable name
		var_name = extract_var_name(token->value + var_pos + 1);
		if (!var_name)
		{
			free(before);
			return (ERROR);
		}
		
		// Get variable value or empty string if not found
		char *env_value = get_env_value(env_list, var_name);
		if (!env_value)
			var_value = ft_strdup("");
		else
			var_value = ft_strdup(env_value);
		
		i = ft_strlen(var_name) + 1;
		free(var_name);
		var_name = NULL;
	}
	
	// Check if variable value retrieval failed
	if (!var_value)
	{
		free(before);
		return (ERROR);
	}
	
	// Extract the part after the variable
	after = ft_strdup(token->value + var_pos + i);
	if (!after)
	{
		free(before);
		free(var_value);
		return (ERROR);
	}
	
	// Join the parts
	result = ft_strjoin(before, var_value);
	free(before);
	before = NULL;
	free(var_value);
	var_value = NULL;
	
	if (!result)
	{
		free(after);
		return (ERROR);
	}
	
	// Join with the part after
	before = ft_strjoin(result, after);
	free(result);
	result = NULL;
	free(after);
	after = NULL;
	
	if (!before)
		return (ERROR);
	
	// Replace token value with expanded string
	free(token->value);
	token->value = before;
	
	return (SUCCESS);
}

/**
 * Expand variables in token values
 * @param tokens Token list to expand
 * @param env_list Environment variable list
 * @param exit_status Last command exit status
 * @return Success or error code
 */
int	expand_variables(t_token *tokens, t_env *env_list, int exit_status)
{
	t_token	*current;
	int		i;
	int		in_single_quotes;

	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_WORD)
		{
			i = 0;
			in_single_quotes = 0;
			
			while (current->value && current->value[i])
			{
				if (current->value[i] == '\'')
					in_single_quotes = !in_single_quotes;
				else if (current->value[i] == '$' && !in_single_quotes
					&& current->value[i + 1] && (is_valid_var_char(current->value[i + 1])
					|| current->value[i + 1] == '?'))
				{
					if (expand_var(current, i, env_list, exit_status) != SUCCESS)
						return (ERROR);
					i = -1;  // Start over since the string has changed
				}
				i++;
			}
		}
		current = current->next;
	}
	
	return (SUCCESS);
}