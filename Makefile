CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I./Inc
READLINE = -lreadline

# Source files
SRC_DIR = Src/
SRC_FILES = builtins.c cleanup.c env.c executor.c heredoc.c init.c input.c \
           parser.c parser_syntax.c parser_tokens.c prompt.c signals.c \
           terminal.c utils.c

SRCS = main.c $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJS = $(SRCS:.c=.o)
NAME = minishell

# Colors for better output
GREEN = \033[0;32m
RESET = \033[0m

all: $(NAME)

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(READLINE)
	@echo "$(GREEN)$(NAME) successfully compiled!$(RESET)"

clean:
	@rm -f $(OBJS)
	@echo "$(GREEN)Object files removed!$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(GREEN)$(NAME) removed!$(RESET)"

re: fclean all

.PHONY: all clean fclean re
