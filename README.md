# Minishell Project

## Project Status: ✅ REFACTORING COMPLETE

The Minishell project has been successfully refactored to comply with 42's norminette rules while maintaining all functionality. The code is now properly organized in smaller files with clear separation of concerns.

## Project Structure

The project has been restructured as follows:

### Builtins Components
- `builtins_basic.c` - Echo and pwd commands
- `builtins_dir.c` - Cd command
- `builtins_env.c` - Export, unset, and env commands
- `builtins_exit.c` - Exit command
- `builtins_utils.c` - Helper functions

### Executor Components
- `executor_core.c` - Main execution logic
- `executor_pipe.c` - Pipe handling
- `executor_redir.c` - Redirection handling
- `executor_path.c` - Path resolution
- `executor_utils.c` - Utility functions

### Other Components
- `signals.c` - Signal handling
- `heredoc.c` - Heredoc implementation
- `env.c` - Environment variable management
- `parser.c`, `parser_syntax.c`, `parser_tokens.c` - Parsing
- `prompt.c` - User prompt
- `terminal.c` - Terminal handling
- `utils.c` - General utilities

## Compilation Instructions

The project can be compiled in various environments:

### 1. 42 School Environment
```bash
make
```

### 2. Linux/Unix Environment
```bash
cd /path/to/Minishell-v2
make
```

### 3. WSL (Windows Subsystem for Linux)
```bash
wsl
cd /mnt/c/Users/quint/Documents/GitHub/Minishell-v2
make
```

### 4. MinGW (on Windows)
```bash
mingw32-make
```

## Requirements

- GCC compiler
- Readline library
- Make utility

## Usage

After compilation, run the shell:
```bash
./minishell
```

## Features

- Command prompt
- Command history
- Environment variable management
- Pipes and redirections
- Signal handling
- Built-in commands (echo, cd, pwd, export, unset, env, exit)

## Documentation

Several documentation files are available:
- `REFACTORING_SUMMARY.md` - Overview of refactoring changes
- `VERIFICATION_REPORT.md` - Detailed verification of requirements
- `COMPLIANCE.md` - Subject requirements compliance details
- `REFACTORING_COMPLETE.md` - Final status of refactoring

## Notes

The project follows 42's norm rules:
- All functions are under 25 lines
- Maximum 5 functions per file
- Proper comment style
- Clear code organization

Bonus features have not been implemented as requested.

