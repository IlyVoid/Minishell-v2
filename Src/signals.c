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

/* Global variable to store received signal number only */
int	g_received_signal = 0;

/**
 * Handle SIGINT (Ctrl+C) signal in interactive mode
 * @param sig Signal number
 */
void	handle_sigint_interactive(int sig)
{
	g_received_signal = sig;
	write(STDERR_FILENO, "\n", 1);
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
	write(STDERR_FILENO, "\n", 1);
}

/**
 * Handle SIGINT (Ctrl+C) signal in heredoc mode
 * @param sig Signal number
 */
void	handle_sigint_heredoc(int sig)
{
	g_received_signal = sig;
	write(STDERR_FILENO, "\n", 1);
	close(STDIN_FILENO);
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
	write(STDERR_FILENO, "Quit: 3\n", 8);
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
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	
	sa_quit.sa_handler = handle_sigquit_interactive;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
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
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	
	sa_quit.sa_handler = handle_sigquit_exec;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
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
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	
	sa_quit.sa_handler = SIG_IGN;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
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
	sigaction(SIGINT, &sa_int, NULL);
	
	sa_quit.sa_handler = SIG_DFL;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/**
 * Disable signals (used during critical operations)
 */
void	disable_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	g_received_signal = 0;
	sa_int.sa_handler = SIG_IGN;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	
	sa_quit.sa_handler = SIG_IGN;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

