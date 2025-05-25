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

/* Global variable to store the current mode */
static int	g_sigmode = 0;

/**
 * Handle SIGINT (Ctrl+C) signal
 * @param sig Signal number
 */
void	handle_sigint(int sig)
{
	(void)sig;
	
	if (g_sigmode == 0)  // Interactive mode
	{
		// Add a newline, then clear the current line and redisplay prompt
		write(STDERR_FILENO, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	else if (g_sigmode == 1)  // Execution mode
	{
		// Just add a newline
		write(STDERR_FILENO, "\n", 1);
	}
	else if (g_sigmode == 2)  // Heredoc mode
	{
		// Cancel the heredoc input
		write(STDERR_FILENO, "\n", 1);
		// We can't use exit() here as it would exit the entire program
		// Instead, we close stdin to force the heredoc read to end
		close(STDIN_FILENO);
	}
}

/**
 * Handle SIGQUIT (Ctrl+\) signal
 * @param sig Signal number
 */
void	handle_sigquit(int sig)
{
	(void)sig;
	
	if (g_sigmode == 0)  // Interactive mode
	{
		// Do nothing in interactive mode
		rl_on_new_line();
		rl_redisplay();
	}
	else if (g_sigmode == 1)  // Execution mode
	{
		// Print "Quit: 3" and a newline
		write(STDERR_FILENO, "Quit: 3\n", 8);
	}
	// In heredoc mode, do nothing
}

/**
 * Set up signal handlers for interactive mode
 */
void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	// Set up SIGINT handler
	sa_int.sa_handler = handle_sigint;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	
	// Set up SIGQUIT handler
	sa_quit.sa_handler = handle_sigquit;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
	
	// Set mode to interactive
	g_sigmode = 0;
}

/**
 * Set up signal handlers for command execution mode
 */
void	setup_exec_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	// Set up SIGINT handler
	sa_int.sa_handler = handle_sigint;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	
	// Set up SIGQUIT handler
	sa_quit.sa_handler = handle_sigquit;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
	
	// Set mode to execution
	g_sigmode = 1;
}

/**
 * Set up signal handlers for heredoc mode
 */
void	setup_heredoc_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	// Set up SIGINT handler
	sa_int.sa_handler = handle_sigint;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	
	// Ignore SIGQUIT in heredoc mode
	sa_quit.sa_handler = SIG_IGN;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
	
	// Set mode to heredoc
	g_sigmode = 2;
}

/**
 * Reset signals to default behavior
 */
void	reset_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	// Reset SIGINT to default
	sa_int.sa_handler = SIG_DFL;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	
	// Reset SIGQUIT to default
	sa_quit.sa_handler = SIG_DFL;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
	
	// Reset mode
	g_sigmode = 0;
}

/**
 * Disable signals (used during critical operations)
 */
void	disable_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	// Ignore SIGINT
	sa_int.sa_handler = SIG_IGN;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	
	// Ignore SIGQUIT
	sa_quit.sa_handler = SIG_IGN;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

