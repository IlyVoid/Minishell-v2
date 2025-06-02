# Minishell Subject Compliance Report

This document verifies that the project complies with all requirements specified in the subject.

## 1. Basic Shell Requirements ✓

- **Prompt Display**: The shell correctly displays a prompt when waiting for a new command. This is handled in `get_shell_input()` in `prompt.c`.
- **History**: Working history implementation using readline's history functionality.
- **Executable Search**: The shell correctly searches for and launches executables based on the PATH variable or using relative/absolute paths. This is implemented in `find_command_path()` in `executor_path.c`.
- **Global Variable**: Only one global variable (`g_received_signal`) is used to indicate received signals, as required.

## 2. Quote Handling ✓

- **Single Quotes (')**: Prevent the shell from interpreting metacharacters in the quoted sequence.
- **Double Quotes (")**: Prevent the shell from interpreting metacharacters except for `$`.
- The quote handling is properly implemented in the parser.

## 3. Redirections ✓

- **Input Redirection (<)**: Properly implemented in `handle_input_redirection()` in `executor_redir.c`.
- **Output Redirection (>)**: Properly implemented in `handle_output_redirection()` in `executor_redir.c`.
- **Heredoc (<<)**: Properly implemented in `handle_heredoc()` in `heredoc.c`. The implementation correctly reads input until a delimiter is seen.
- **Append Output (>>)**: Properly implemented in `handle_output_redirection()` with the append flag set to 1.

## 4. Pipes ✓

- **Pipe Implementation**: The shell correctly handles pipes using the `|` character. This is implemented in `execute_commands()` and `execute_piped_command()` in `executor_pipe.c`.
- **Command Chaining**: The output of each command in the pipeline is correctly connected to the input of the next command via a pipe.

## 5. Environment Variables ✓

- **Variable Expansion**: The shell correctly expands environment variables ($ followed by characters) to their values. This is handled in `expand_variables()` in the parser and in `expand_heredoc_vars()` for heredocs.
- **Exit Status ($?)**: The shell correctly expands `$?` to the exit status of the most recently executed foreground pipeline.

## 6. Signal Handling ✓

- **Ctrl-C**: Displays a new prompt on a new line in interactive mode. This is implemented in `handle_sigint_interactive()` in `signals.c`.
- **Ctrl-D**: Exits the shell. This is handled in the input reading functions.
- **Ctrl-\\**: Does nothing in interactive mode. This is implemented in `handle_sigquit_interactive()` which is an empty function as required.

## 7. Built-in Commands ✓

All required built-in commands are properly implemented and split into separate files for better organization:

- **echo with -n option**: Implemented in `builtin_echo()` in `builtins_basic.c`.
- **cd with relative/absolute path**: Implemented in `builtin_cd()` in `builtins_dir.c`.
- **pwd without options**: Implemented in `builtin_pwd()` in `builtins_basic.c`.
- **export without options**: Implemented in `builtin_export()` in `builtins_env.c`.
- **unset without options**: Implemented in `builtin_unset()` in `builtins_env.c`.
- **env without options/arguments**: Implemented in `builtin_env()` in `builtins_env.c`.
- **exit without options**: Implemented in `builtin_exit()` in `builtins_exit.c`.

## 8. Memory Management ✓

- **Memory Allocation**: All heap-allocated memory spaces are properly freed when necessary.
- **File Descriptors**: File descriptors are consistently closed after use.
- **Readline Leaks**: As allowed by the subject, readline memory leaks are not fixed, but all other memory management is handled correctly.

## 9. Makefile Requirements ✓

- **Required Rules**: The Makefile includes all required rules: `NAME`, `all`, `clean`, `fclean`, and `re`.
- **Compilation Flags**: The Makefile compiles with the flags `-Wall`, `-Wextra`, and `-Werror`.
- **No Relink**: The Makefile does not relink unnecessarily.

## 10. Code Organization ✓

The code has been refactored to improve organization and comply with 42's norm rules:

- **Function Length**: All functions are now within the 25-line limit.
- **Functions per File**: All files now contain at most 5 functions.
- **Comment Style**: Comments follow the proper format for norminette.

## Conclusion

The project fully complies with all the mandatory requirements specified in the subject. The bonus features (&&, || with parenthesis, wildcards) were not implemented as requested.

