/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:53:02 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:46:07 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <dirent.h>
# include <string.h>
# include <errno.h>
# include <signal.h>
# include <termios.h>
# include <readline/readline.h>
# include <readline/history.h>

/* Error codes */
# define SUCCESS 0
# define ERROR 1
# define SYNTAX_ERROR 2
# define CMD_NOT_FOUND 127

/* Token types for lexer/parser */
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_IN,
	TOKEN_HEREDOC,
	TOKEN_REDIRECT_OUT,
	TOKEN_REDIRECT_APPEND,
	TOKEN_EOF
}	t_token_type;

/* Token structure for lexical analysis */
typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

/* Command redirection structure */
typedef struct s_redirection
{
	t_token_type			type;
	char					*file;
	struct s_redirection	*next;
}	t_redirection;

/* Command structure */
typedef struct s_command
{
	char				**args;
	t_redirection		*redirections;
	struct s_command	*next;
	int					pipe_out;
}	t_command;

/* Environment variable structure */
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

/* Shell state structure */
typedef struct s_shell
{
	t_env		*env_list;
	int			exit_status;
	int			running;
	t_token		*tokens;
	t_command	*commands;
	struct termios	orig_termios;
	int			term_saved;
	int			signal_state;
	int			heredoc_active;
	char		*heredoc_file;
}	t_shell;

/* Parser functions */
t_token		*tokenize_input(char *input);
void		free_tokens(t_token *tokens);
t_command	*parse_tokens(t_token *tokens, t_shell *shell);
void		free_commands(t_command *commands);
int			expand_variables(t_token *tokens, t_env *env_list, int exit_status);

/* Environment functions */
t_env		*init_env(char **envp);
void		free_env(t_env *env_list);
char		*get_env_value(t_env *env_list, char *key);
int			set_env_value(t_env *env_list, char *key, char *value);
int			unset_env_value(t_env **env_list, char *key);
char		**env_to_array(t_env *env_list);

/* Executor functions */
int			execute_commands(t_command *commands, t_shell *shell);
int			execute_builtin(t_command *cmd, t_shell *shell);
int			is_builtin(char *cmd);
int			setup_redirections(t_redirection *redirections);

/* Builtin functions */
int			builtin_echo(t_command *cmd, t_shell *shell);
int			builtin_cd(t_command *cmd, t_shell *shell);
int			builtin_pwd(t_command *cmd, t_shell *shell);
int			builtin_export(t_command *cmd, t_shell *shell);
int			builtin_unset(t_command *cmd, t_shell *shell);
int			builtin_env(t_command *cmd, t_shell *shell);
int			builtin_exit(t_command *cmd, t_shell *shell);

/* Utility functions */
void		*ft_malloc(size_t size);
char		*ft_strdup(const char *s);
char		*ft_substr(char const *s, unsigned int start, size_t len);
int			ft_strcmp(const char *s1, const char *s2);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
char		*ft_strjoin(char const *s1, char const *s2);
size_t		ft_strlen(const char *s);
char		**ft_split(char const *s, char c);
void		ft_putstr_fd(char *s, int fd);
void		ft_putendl_fd(char *s, int fd);
int			ft_isalpha(int c);
int			ft_isalnum(int c);
int			ft_isdigit(int c);
char		*ft_itoa(int n);
int			ft_atoi(const char *str);

/* Signal handling */
void		setup_signals(void);
void		setup_exec_signals(void);
void		setup_heredoc_signals(void);
void		reset_signals(void);
void		disable_signals(void);
void		handle_sigint(int sig);
void		handle_sigquit(int sig);

/* Terminal handling */
int			setup_terminal(t_shell *shell);
void		restore_terminal(t_shell *shell);
void		handle_signal_state(t_shell *shell);
void		cleanup_interrupted_heredoc(t_shell *shell);
int			handle_history(char *input);

/* Error handling */
void		print_error(char *cmd, char *arg, char *message);
void		syntax_error(char *token);

/* Heredoc handling */
char		*handle_heredoc(char *delimiter, t_env *env_list, int exit_status);
char		*create_heredoc_file(void);
void		cleanup_heredoc(char *filename);

#endif
