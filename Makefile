CC = cc
CFLAGS = -Wall -Wextra -Werror
SRCS = main.c Src/parser.c Src/executor.c Src/builtins.c Src/env.c Src/utils.c
OBJS = $(SRCS:.c=.o)
NAME = minishell

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -lreadline

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all