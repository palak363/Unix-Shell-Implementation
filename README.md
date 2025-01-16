# Custom Shell Implementation in C

This project implements a **custom shell** in C, featuring essential functionalities such as command execution, history management, signal handling, and support for background processes and piped commands.

## Features

1. **Command Execution**:  
   - Executes single commands and commands with arguments.  
   - Handles piped commands seamlessly.  

2. **Background Process Handling**:  
   - Supports running commands in the background using `&`.  

3. **Command History**:  
   - Maintains a history of up to 200 commands.  
   - Displays command history with timestamps and execution details.

4. **Signal Handling**:  
   - Handles `SIGINT` (Ctrl+C) gracefully.  
   - Safeguards against repeated interruptions, providing user-friendly output.

5. **Error Management**:  
   - Robust error handling for system calls like `fork()`, `execvp()`, and `pipe()`.

6. **Custom Prompt**:  
   - Displays a custom shell prompt: `group_48@aakanksha_palak:~$`.  

## Getting Started

### Prerequisites
- GCC Compiler (for compilation)
- Linux/Unix-based operating system (required for system calls)

### Compilation
To compile the shell, run the following command:
```bash
gcc -o custom_shell shell.c
```

### Execution
Run the shell using:
```bash
./custom_shell
```

## Usage

- **Basic Commands**: Enter any shell command, e.g., `ls`, `pwd`, `echo "Hello World"`.  
- **Background Commands**: Use `&` to run commands in the background, e.g., `sleep 5 &`.  
- **Piped Commands**: Combine commands with `|`, e.g., `ls | grep "file"`.  
- **Command History**: Type `history` to view executed commands.  
- **Exit**: Type `exit` to terminate the shell.

## Implementation Details

### Core Components
1. **Signal Handling**:
   - Captures and responds to `SIGINT` (Ctrl+C) with a custom handler.  
   - Ensures graceful shutdown after multiple interruptions.

2. **Process Management**:
   - Uses `fork()` to create child processes for command execution.  
   - Implements `execvp()` for executing commands within child processes.  

3. **Piping**:
   - Supports multi-stage piped commands using the `pipe()` system call.  
   - Redirects `STDIN` and `STDOUT` for each pipe stage.

4. **History Tracking**:
   - Records command details, including execution time and process IDs.  
   - Limits history size to 200 entries to conserve memory.

5. **Whitespace Handling**:
   - Trims unnecessary whitespace from user input for accurate command parsing.

## File Structure

```
|-- shell.c
|-- README.md
```

- `shell.c`: The main source file containing the implementation.  
- `README.md`: Documentation for the project.

## Example Usage

### Running Commands
```bash
group_48@aakanksha_palak:~$ ls -l
group_48@aakanksha_palak:~$ echo "Custom Shell in C"
group_48@aakanksha_palak:~$ sleep 10 &
```

### Using Pipes
```bash
group_48@aakanksha_palak:~$ ls | grep ".c"
group_48@aakanksha_palak:~$ cat file.txt | wc -l
```

### Viewing History
```bash
group_48@aakanksha_palak:~$ history
```
This project demonstrates the implementation of a simple yet powerful shell in C, integrating core concepts like process management, piping, signal handling, and user-friendly features. It’s an excellent starting point for understanding system-level programming.
The custom shell provides a solid foundation for exploring advanced shell features and operating system concepts. Its modular design allows for future enhancements, making it a versatile tool for learning and development.
