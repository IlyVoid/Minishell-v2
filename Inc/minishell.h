/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:53:02 by quvan-de          #+#    #+#             */
/*   Updated: 2025/06/02 18:17:20 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>

# ifdef _WIN32
/* Windows-specific headers */
#  include <io.h>
#  include <process.h>
#  include <windows.h>
# else
/* Unix-specific headers */
#  include <unistd.h>
#  include <fcntl.h>
#  include <sys/wait.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  include <dirent.h>
#  include <signal.h>
#  include <termios.h>
# endif

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
	int			heredoc_active;
	char		*heredoc_file;
	int			signal_state;
}	t_shell;

/* Global signal variable - stores only the signal number 
 * Volatile ensures it's not optimized out by compiler
 * sig_atomic_t ensures atomic access to prevent race conditions
 */
extern volatile sig_atomic_t g_received_signal;

/* Parser functions */
t_token		*tokenize_input(char *input);
void		free_tokens(t_token *tokens);
t_command	*parse_tokens(t_token *tokens, t_shell *shell);
void		free_commands(t_command *commands);
int			expand_variables(t_token *tokens, t_env *env_list, int exit_status);
char		*finalize_word(char *value, char *input, int start, int end);
int			handle_operator_token(const char *str, int *index);


/* Environment functions */
t_env		*init_env(char **envp);
void		free_env(t_env *env_list);
char		*get_env_value(t_env *env_list, char *key);
int			set_env_value(t_env *env_list, char *key, char *value);
int			unset_env_value(t_env **env_list, char *key);
char		**env_to_array(t_env *env_list);

/* Builtin function declarations - basic commands */
int			builtin_echo(t_command *cmd, t_shell *shell);
int			builtin_pwd(t_command *cmd, t_shell *shell);

/* Builtin function declarations - directory operations */
int			builtin_cd(t_command *cmd, t_shell *shell);

/* Builtin function declarations - environment operations */
int			builtin_export(t_command *cmd, t_shell *shell);
int			builtin_unset(t_command *cmd, t_shell *shell);
int			builtin_env(t_command *cmd, t_shell *shell);

/* Builtin function declarations - shell control */
int			builtin_exit(t_command *cmd, t_shell *shell);

/* Builtin utility functions */
int			is_valid_variable_name(char *var);
int			parse_variable_assignment(char *arg, char **key, char **value);
int			is_numeric(char *str);

/* Executor core functions */
int			execute_commands(t_command *commands, t_shell *shell);
int			execute_builtin(t_command *cmd, t_shell *shell);
int			is_builtin(char *cmd);
int			execute_single_command(t_command *cmd, t_shell *shell, 
				int in_fd, int out_fd);
int			execute_child_process(t_command *cmd, t_shell *shell,
				int in_fd, int out_fd);
int			execute_builtin_directly(t_command *cmd, t_shell *shell, int out_fd);

/* Executor redirection handling */
int			setup_redirections(t_redirection *redirections);
int			cleanup_heredoc_files(t_command *cmd);
int			cleanup_all_heredocs(t_command *commands);
int			is_heredoc_file(char *filename);

/* Executor path resolution */
char		*find_command_path(char *cmd, t_env *env_list); /* Returns NULL if command not found */

/* Executor utility functions */
int			save_std_fds(int saved_fds[2]);
int			restore_std_fds(int saved_fds[2]);
int			get_exit_status(int status);
int			free_string_array(char **arr);

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
char		*ft_strchr(const char *s, int c);
void		*ft_memset(void *b, int c, size_t len);
char		*ft_strrchr(const char *s, int c);
char		*ft_strstr(char *str, char *to_find);
void		ft_putchar_fd(char c, int fd);
int			is_whitespace(char c);
int			is_delimiter(char *str);

/* Signal handling */
int			setup_signals(void);
int			setup_exec_signals(void);
int			setup_heredoc_signals(void);
int			reset_signals(void);
int			disable_signals(void);
void		handle_sigint_interactive(int sig);
void		handle_sigint_exec(int sig);
void		handle_sigint_heredoc(int sig);
void		handle_sigquit_interactive(int sig);
void		handle_sigquit_exec(int sig);
int			set_signal_mode(t_shell *shell, int mode);

/* Shell initialization and management */
t_shell		*init_shell(char **envp);
int			verify_shell_state(t_shell *shell);
int			cleanup_shell(t_shell *shell);
int			process_input(char *input, t_shell *shell);
void		handle_interrupted_execution(t_shell *shell);
void		shell_loop(t_shell *shell);
int			restore_terminal(t_shell *shell);
int			handle_signal_state(t_shell *shell);
int			cleanup_interrupted_heredoc(t_shell *shell);
int			handle_history(char *input);
int			setup_terminal(t_shell *shell);
int			recover_terminal_error(t_shell *shell);
int			start_heredoc(t_shell *shell, char *file);
int			end_heredoc(t_shell *shell);

/* Error handling */
void		print_error(char *cmd, char *arg, char *message);
void		syntax_error(char *token);

/* Heredoc handling */
char		*handle_heredoc(char *delimiter, t_env *env_list, int exit_status);
char		*create_heredoc_file(void); /* Returns NULL on error */
int			cleanup_heredoc(char *filename);

/* Prompts */
char		*get_shell_input(t_shell *shell); /* Returns NULL on error or EOF */

/* Cleanup */
int			cleanup_command_resources(t_shell *shell);

#endif
