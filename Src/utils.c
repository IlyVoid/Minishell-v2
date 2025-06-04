/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: quint <quint@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:53:02 by quint             #+#    #+#             */
/*   Updated: 2025/06/02 18:16:49 by quvan-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/minishell.h"

/**
 * Safe malloc wrapper with error checking
 * @param size Size of memory to allocate
 * @return Allocated memory pointer or NULL on failure
 */
void	*ft_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		perror("minishell: malloc");
		return (NULL);
	}
	return (ptr);
}

int	is_whitespace(char c)
{
	if (c == ' ' || c == '\t' || c == '\f' || c == '\r' || c == '\v')
		return (1);
	return (0);
}

int	is_delimiter(char *str)
{
	if (str == NULL)
		return (1);
	if (str[0] == '\0')
		return (0);
	if (str[0] == '>' && str[1] == '\0')
		return (1);
	if (str[0] == '<' && str[1] == '\0')
		return (1);
	if (str[0] == '|' && str[1] == '\0')
		return (1);
	if (str[0] == ';' && str[1] == '\0')
		return (1);
	if (str[0] == '>' && str[1] == '>' && str[2] == '\0')
		return (1);
	if (str[0] == '<' && str[1] == '<' && str[2] == '\0')
		return (1);
	return (0);
}

/**
 * Calculate the length of a string
 * @param s The string to measure
 * @return Length of the string
 */
size_t	ft_strlen(const char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

/**
 * Create a duplicate of a string
 * @param s The string to duplicate
 * @return Newly allocated string or NULL on failure
 */
char	*ft_strdup(const char *s)
{
	char	*dup;
	size_t	len;
	size_t	i;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	dup = (char *)malloc(sizeof(char) * (len + 1));
	if (!dup)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

/**
 * Extract a substring from a string
 * @param s The source string
 * @param start Starting index of the substring
 * @param len Length of the substring
 * @return Newly allocated substring or NULL on failure
 */
char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*substr;
	size_t	s_len;
	size_t	i;

	if (!s)
		return (NULL);
	s_len = ft_strlen(s);
	if (start >= s_len)
		return (ft_strdup(""));
	if (len > s_len - start)
		len = s_len - start;
	substr = (char *)malloc(sizeof(char) * (len + 1));
	if (!substr)
		return (NULL);
	i = 0;
	while (i < len && s[start + i])
	{
		substr[i] = s[start + i];
		i++;
	}
	substr[i] = '\0';
	return (substr);
}

/**
 * Compare two strings
 * @param s1 First string
 * @param s2 Second string
 * @return Difference between the first differing characters
 */
int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	if (!s1 && !s2)
		return (0);
	if (!s1)
		return (-1);
	if (!s2)
		return (1);
	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}


char	*ft_strrchr(const char *s, int c)
{
	int	i;

	i = ft_strlen(s);
	while (s[i] != (char)c && i >= 0)
		i--;
	if (s[i] == (char)c)
		return ((char *)&s[i]);
	return (NULL);
}

char	*ft_strstr(char *str, char *to_find)
{
	int len;
	int i;
	int j;

	len = 0;
	while (to_find[len])
	{
		len++;
	}
	if (len == 0)
		return (str);
	i = 0;
	j = 0;
	while (str[i])
	{
		while (to_find[j] == str[i + j])
		{
			if (j + 1 == len)
				return (str + i);
			j++;
		}
		j = 0;
		i++;
	}
	return (NULL);
}

/**
 * Compare at most n characters of two strings
 * @param s1 First string
 * @param s2 Second string
 * @param n Maximum number of characters to compare
 * @return Difference between the first differing characters
 */
int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	if (n == 0)
		return (0);
	if (!s1 && !s2)
		return (0);
	if (!s1)
		return (-1);
	if (!s2)
		return (1);
	i = 0;
	while (i < n - 1 && s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	if (i == n)
		return (0);
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

/**
 * Concatenate two strings
 * @param s1 First string
 * @param s2 Second string
 * @return Newly allocated concatenated string or NULL on failure
 */
char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*joined;
	size_t	len1;
	size_t	len2;
	size_t	i;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	joined = (char *)malloc(sizeof(char) * (len1 + len2 + 1));
	if (!joined)
		return (NULL);
	i = 0;
	while (i < len1)
	{
		joined[i] = s1[i];
		i++;
	}
	i = 0;
	while (i < len2)
	{
		joined[len1 + i] = s2[i];
		i++;
	}
	joined[len1 + len2] = '\0';
	return (joined);
}

/**
 * Count the number of words in a string delimited by a character
 * @param s String to count words in
 * @param c Delimiter character
 * @return Number of words
 */
static size_t	count_words(char const *s, char c)
{
	size_t	count;
	size_t	i;
	int		in_word;

	count = 0;
	i = 0;
	in_word = 0;
	while (s[i])
	{
		if (s[i] != c && !in_word)
		{
			in_word = 1;
			count++;
		}
		else if (s[i] == c)
			in_word = 0;
		i++;
	}
	return (count);
}

/**
 * Get the length of a word delimited by a character
 * @param s String containing the word
 * @param c Delimiter character
 * @return Length of the word
 */
static size_t	get_word_len(char const *s, char c)
{
	size_t	len;

	len = 0;
	while (s[len] && s[len] != c)
		len++;
	return (len);
}

/**
 * Free allocated memory in case of error
 * @param split Array of strings to free
 * @param i Number of strings to free
 */
static void	free_split(char **split, size_t i)
{
	size_t	j;

	j = 0;
	while (j < i)
	{
		free(split[j]);
		j++;
	}
	free(split);
}

/**
 * Split a string into an array of strings using a delimiter
 * @param s String to split
 * @param c Delimiter character
 * @return Array of strings or NULL on failure
 */
char	**ft_split(char const *s, char c)
{
	char	**split;
	size_t	word_count;
	size_t	word_len;
	size_t	i;

	if (!s)
		return (NULL);
	word_count = count_words(s, c);
	split = (char **)malloc(sizeof(char *) * (word_count + 1));
	if (!split)
		return (NULL);
	i = 0;
	while (i < word_count)
	{
		while (*s && *s == c)
			s++;
		word_len = get_word_len(s, c);
		split[i] = ft_substr(s, 0, word_len);
		if (!split[i])
		{
			free_split(split, i);
			return (NULL);
		}
		s += word_len;
		i++;
	}
	split[i] = NULL;
	return (split);
}

/**
 * Write a string to a file descriptor
 * @param s String to write
 * @param fd File descriptor to write to
 */
void	ft_putstr_fd(char *s, int fd)
{
	if (!s || fd < 0)
		return ;
	write(fd, s, ft_strlen(s));
}

/**
 * Write a string followed by a newline to a file descriptor
 * @param s String to write
 * @param fd File descriptor to write to
 */
void	ft_putendl_fd(char *s, int fd)
{
	if (!s || fd < 0)
		return ;
	ft_putstr_fd(s, fd);
	write(fd, "\n", 1);
}

/**
 * Check if a character is an alphabetic character
 * @param c Character to check
 * @return Non-zero if the character is alphabetic, zero otherwise
 */
int	ft_isalpha(int c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

/**
 * Check if a character is a digit
 * @param c Character to check
 * @return Non-zero if the character is a digit, zero otherwise
 */
int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

/**
 * Check if a character is alphanumeric
 * @param c Character to check
 * @return Non-zero if the character is alphanumeric, zero otherwise
 */
int	ft_isalnum(int c)
{
	return (ft_isalpha(c) || ft_isdigit(c));
}

/**
 * Count the number of digits in a number
 * @param n Number to count digits in
 * @return Number of digits
 */
static int	count_digits(int n)
{
	int	count;

	count = 0;
	if (n <= 0)
		count = 1;
	while (n != 0)
	{
		n /= 10;
		count++;
	}
	return (count);
}

/**
 * Convert an integer to a string
 * @param n Integer to convert
 * @return Newly allocated string representation of the integer
 */
char	*ft_itoa(int n)
{
	char	*str;
	int		digits;
	long	num;

	digits = count_digits(n);
	str = (char *)malloc(sizeof(char) * (digits + 1));
	if (!str)
		return (NULL);
	str[digits] = '\0';
	num = n;
	if (num < 0)
	{
		str[0] = '-';
		num = -num;
	}
	if (num == 0)
		str[0] = '0';
	while (num > 0)
	{
		str[--digits] = (num % 10) + '0';
		num /= 10;
	}
	return (str);
}

/**
 * Convert a string to an integer
 * @param str String to convert
 * @return Integer representation of the string
 */
int	ft_atoi(const char *str)
{
	int	result;
	int	sign;
	int	i;

	if (!str)
		return (0);
	result = 0;
	sign = 1;
	i = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

/**
 * Print an error message to stderr
 * @param cmd Command that caused the error
 * @param arg Argument that caused the error
 * @param message Error message
 */
void	print_error(char *cmd, char *arg, char *message)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (cmd)
	{
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	if (arg)
	{
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	if (message)
		ft_putstr_fd(message, STDERR_FILENO);
	else
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

/**
 * Print a syntax error message to stderr
 * @param token Token that caused the syntax error
 */
void	syntax_error(char *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
	if (token)
		ft_putstr_fd(token, STDERR_FILENO);
	else
		ft_putstr_fd("newline", STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
}


char	*ft_strchr(const char *s, int c)
{
	while (*s != '\0')
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if (*s == (char)c)
		return ((char *)s);
	return (NULL);
}

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*ptr;

	ptr = (unsigned char *)b;
	while (len--)
		*ptr++ = (unsigned char)c;
	return (b);
}

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}