# Minishell Refactoring

This document summarizes the refactoring changes made to comply with the norminette rules while maintaining all functionality.

## Files Split

### Builtins
- **builtins.c** split into:
  - `builtins_basic.c`: echo and pwd commands
  - `builtins_dir.c`: cd command
  - `builtins_env.c`: export, unset, and env commands
  - `builtins_exit.c`: exit command
  - `builtins_utils.c`: utility functions for builtins

### Executor
- **executor.c** split into:
  - `executor_core.c`: main execution logic and built-in handling
  - `executor_pipe.c`: pipe handling and command chaining
  - `executor_redir.c`: redirection handling and heredoc cleanup
  - `executor_path.c`: command path resolution
  - `executor_utils.c`: utility functions for execution

## Style Fixes
- Replaced C-style `//` comments with norminette-compliant `/* */` comments
- Ensured proper header comments for all functions
- Fixed whitespace and indentation

## Function Refactoring
- Broke down long functions into smaller ones to comply with the 25-line limit:
  - Split `builtin_cd` into multiple functions
  - Split `builtin_export` to handle different cases
  - Refactored `setup_redirections` into specialized functions
  - Split `execute_single_command` and `execute_commands` 
  - Reduced `find_command_path` complexity

## Other Changes
- Updated header file with new function prototypes
- Updated Makefile to include the new source files
- Ensured each file contains at most 5 functions

All these changes maintain the original functionality while complying with the norminette rules.

