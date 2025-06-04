/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quvan-de <quvan-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 12:04:24 by quvan-de          #+#    #+#             */
/*   Updated: 2025/05/25 12:04:24 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"
#include <signal.h>

/* Global variable to store received signal number only */
volatile sig_atomic_t	g_received_signal = 0;

/**
 * Safe write function with error checking
 * @param fd File descriptor to write to
 * @param buf Buffer to write
 * @param count Number of bytes to write
 * @return Number of bytes written or -1 on error
 */
static ssize_t	safe_write(int fd, const void *buf, size_t count)
{
	ssize_t	ret;

	ret = write(fd, buf, count);
	if (ret == -1)
		perror("minishell: write");
	return (ret);
}

/**
 * Handle SIGINT (Ctrl+C) signal in interactive mode
 * @param sig Signal number
 */
void	handle_sigint_interactive(int sig)
{
	g_received_signal = sig;
	if (safe_write(STDERR_FILENO, "\n", 1) == -1)
		return;
	
	/* Readline functions don't typically return error values */
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/**
 * Handle SIGINT (Ctrl+C) signal in execution mode
 * @param sig Signal number
 */
void	handle_sigint_exec(int sig)
{
	g_received_signal = sig;
	safe_write(STDERR_FILENO, "\n", 1);
}

/**
 * Handle SIGINT (Ctrl+C) signal in heredoc mode
 * @param sig Signal number
 */
void	handle_sigint_heredoc(int sig)
{
	g_received_signal = sig;
	safe_write(STDERR_FILENO, "\n", 1);
	if (close(STDIN_FILENO) == -1)
		perror("minishell: close");
}

/**
 * Handle SIGQUIT (Ctrl+\) signal in interactive mode
 * @param sig Signal number
 */
void	handle_sigquit_interactive(int sig)
{
	g_received_signal = sig;
	/* Do nothing in interactive mode as per requirements */
}

/**
 * Handle SIGQUIT (Ctrl+\) signal in execution mode
 * @param sig Signal number
 */
void	handle_sigquit_exec(int sig)
{
	g_received_signal = sig;
	safe_write(STDERR_FILENO, "Quit: 3\n", 8);
}

/**
 * Set up signal handlers for interactive mode
 */
void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	g_received_signal = 0;
	sa_int.sa_handler = handle_sigint_interactive;
	sa_int.sa_flags = SA_RESTART; /* Restart interrupted system calls */
	sigemptyset(&sa_int.sa_mask);
	/* Add other signals to block during handler execution */
	sigaddset(&sa_int.sa_mask, SIGQUIT);
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
	{
		perror("minishell: sigaction");
		return;
	}
	
	sa_quit.sa_handler = handle_sigquit_interactive;
	sa_quit.sa_flags = SA_RESTART;
	sigemptyset(&sa_quit.sa_mask);
	sigaddset(&sa_quit.sa_mask, SIGINT);
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
	{
		perror("minishell: sigaction");
		return;
	}
}

/**
 * Set up signal handlers for command execution mode
 */
void	setup_exec_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	g_received_signal = 0;
	sa_int.sa_handler = handle_sigint_exec;
	sa_int.sa_flags = 0; /* Don't restart system calls during execution */
	sigemptyset(&sa_int.sa_mask);
	sigaddset(&sa_int.sa_mask, SIGQUIT);
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
	{
		perror("minishell: sigaction");
		return;
	}
	
	sa_quit.sa_handler = handle_sigquit_exec;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaddset(&sa_quit.sa_mask, SIGINT);
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
	{
		perror("minishell: sigaction");
		return;
	}
}

/**
 * Set up signal handlers for heredoc mode
 */
void	setup_heredoc_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	g_received_signal = 0;
	sa_int.sa_handler = handle_sigint_heredoc;
	sa_int.sa_flags = 0; /* Don't restart when reading heredoc */
	sigemptyset(&sa_int.sa_mask);
	sigaddset(&sa_int.sa_mask, SIGQUIT);
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
	{
		perror("minishell: sigaction");
		return;
	}
	
	sa_quit.sa_handler = SIG_IGN;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
	{
		perror("minishell: sigaction");
		return;
	}
}

/**
 * Reset signals to default behavior
 */
void	reset_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	g_received_signal = 0;
	sa_int.sa_handler = SIG_DFL;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
	{
		perror("minishell: sigaction");
		return;
	}
	
	sa_quit.sa_handler = SIG_DFL;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
	{
		perror("minishell: sigaction");
		return;
	}
}

/**
 * Disable signals (used during critical operations)
 */
void	disable_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	sigset_t		block_set;
	
	/* Block both signals during critical sections */
	sigemptyset(&block_set);
	sigaddset(&block_set, SIGINT);
	sigaddset(&block_set, SIGQUIT);
	sigprocmask(SIG_BLOCK, &block_set, NULL);
	
	g_received_signal = 0;
	sa_int.sa_handler = SIG_IGN;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
	{
		perror("minishell: sigaction");
		return;
	}
	
	sa_quit.sa_handler = SIG_IGN;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
	{
		perror("minishell: sigaction");
		return;
	}
}

void	handle_interrupted_execution(t_shell *shell)
{
	if (shell)
	{
		shell->heredoc_active = 0;
		shell->exit_status = 130;
	}
}