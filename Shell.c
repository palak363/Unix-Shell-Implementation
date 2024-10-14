#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct {
    char cmd[1024];
    pid_t pid;
    bool background;
    time_t execTime;
} commandInHis;

commandInHis history[200];
int historyCnt = 0;

static void my_handler(int signum);
void setupSignalHandler();
void shell_loop();
void read_user_input(char* input);
int pipe_command(char* input);
bool find_background(const char *command);
int launch (char* command , int status);
void trimWhiteSpace(char *str);
int create_process_and_run(char* cmd, int bg);
void terminateHistory();
void showHistory();

static void my_handler(int signum) { // signal handler for SIGINT
    static int counter = 0;
    if (signum == SIGINT) {
        char buff1[23] = "\nCaught SIGINT signal\n";
        write(STDOUT_FILENO, buff1, 23);
        if (counter++ == 1) {
            char buff2[20] = "Cannot handle more\n";
            write(STDOUT_FILENO, buff2, 20);
            terminateHistory();
            exit(0);
        }
    }
}

void setupSignalHandler() {
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = my_handler;

    sigaction(SIGINT, &sig, NULL);
}

void shell_loop() { // for infinite loop running
    int status = 1;
    char input[1024];
    do {
        printf("group_48@aakanksha_palak:~$ ");
        read_user_input(input);
        char* pipe = strchr(input, '|');

        if (pipe != NULL) {
            status = pipe_command(input);
        } else {
            status = launch(input, status);
        }
    } while (status);
}

void read_user_input(char* input) {  // take input
    if (fgets(input, 1024, stdin) != NULL) {
        int length = strlen(input);
        for (int i = 0; input[i] != '\0'; i++) {
            if (input[i] == '\n') {
                input[i] = '\0';
                break;
            }
        }
    } else {
        perror("Error: Unable to take input ");
        exit(0);
    }
}

int pipe_command(char* input) {  // pipe commands
    if (historyCnt < 200) {
            strncpy(history[historyCnt].cmd, input, sizeof(history[historyCnt].cmd) - 1);
            history[historyCnt].cmd[sizeof(history[historyCnt].cmd) - 1] = '\0';
            time(&history[historyCnt].execTime);
            historyCnt++;
    } else {
        perror("Error: History Full.");
    }


    int count_pipe = 0;
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == '|') count_pipe++;
    }
    char* remaining_input = input;
    int original_stdin = dup(STDIN_FILENO);
    int original_stdout = dup(STDOUT_FILENO);

    for (int i = 0; i <= count_pipe; i++) {
        char* command[1024];
        char* pipe_ptr = strchr(remaining_input, '|');

        if (pipe_ptr != NULL) {
            *pipe_ptr = '\0';
            pipe_ptr++;
            while (*pipe_ptr == ' ') pipe_ptr++;
        }
        command[0] = strtok(remaining_input, " ");
        int j = 1;
        while ((command[j] = strtok(NULL, " ")) != NULL) {
            j++;
        }
        command[j] = NULL;

        int fd[2];
        if (pipe(fd) == -1) {
            perror("Error: Pipe failed");
            exit(1);
        }
        int rc = fork();
        if (rc == -1) {
            perror("Error: Fork failed");
            exit(1);
        } else if (rc == 0) {
            if (pipe_ptr != NULL) {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }
            execvp(command[0], command);
            perror("Error executing command");
            exit(1);
        }

        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        wait(NULL);

        if (pipe_ptr != NULL) {
            remaining_input = pipe_ptr;
        }
    }
    dup2(original_stdin, STDIN_FILENO);
    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdin);
    close(original_stdout);

    return 1;
}

bool find_background(const char *command) {  // find background processes
    for (int i = 0; command[i] != '\0'; i++) {
        if (command[i] == '&') {
            return true;
        }
    }
    return false;
}

int launch(char* command, int status) { // launches non piped commands
    if (strcmp(command, "exit") == 0) {
        terminateHistory();
        printf("Shell ended");
        return 0;
    } else if (strcmp(command, "history") == 0) {
        showHistory();
    } else if (find_background(command)) {
        char copy_command[1024];
        strncpy(copy_command, command, sizeof(copy_command) - 1);
        copy_command[sizeof(copy_command) - 1] = '\0';

        char* tok = strtok(copy_command, "&");
        int fg = 0;
        while (tok != NULL) {
            trimWhiteSpace(tok);
            status = create_process_and_run(tok, fg);
            if (status == -1) {
                perror("Error: Unable to create process and run");
                exit(1);
            }
            fg = 1;
            tok = strtok(NULL, "&");
        }
    } else {
        status = create_process_and_run(command, 0);
        if (status == -1) {
            perror("Error: Unable to create process and run ");
            exit(1);
        }
    }
    return 1;
}

void trimWhiteSpace(char *str) {  // trimming white spaces
    int len = strlen(str);
    int s = 0;
    int e = len - 1;

    while (isspace(str[s]) && s < len) s++;
    while (isspace(str[e]) && e >= 0) e--;

    char st[1024];
    int j = 0;
    for (int i = s; i <= e; i++) {
        st[j++] = str[i];
    }
    st[j] = '\0';
    strcpy(str, st);
}

int create_process_and_run(char* cmd, int bg) {  // create and run child process using fork
    int status = fork();
    if (status < 0) {
        perror("Error: Could Not Fork");
        exit(1);
    }
    if (status == 0) {
        if (bg) {
            if (setsid() == -1) {
                perror("Error: Setsid Error");
                exit(1);
            }
        }
        char* parameters[1024];
        char* tok = strtok(cmd, " ");

        int idx = 0;
        while (tok != NULL) {
            parameters[idx++] = tok;
            tok = strtok(NULL, " ");
        }
        parameters[idx] = NULL;

        if (execvp(parameters[0], parameters) == -1) {
            perror("Error: Execution Error");
            exit(1);
        }
    } else {
        int pid;
        if (!bg) {
            int status;
            pid = wait(&status);
            if (pid == -1) {
                perror("Error: Wait Failed");
                exit(1);
            }
        }

        if (historyCnt < 200) {
            strncpy(history[historyCnt].cmd, cmd, sizeof(history[historyCnt].cmd) - 1);
            history[historyCnt].cmd[sizeof(history[historyCnt].cmd) - 1] = '\0';
            history[historyCnt].pid = pid;
            history[historyCnt].background = bg;
            time(&history[historyCnt].execTime);
            historyCnt++;
        } else {
            perror("Error: History Full.");
        }
    }
    return 1;
}

void terminateHistory() {  // history
    printf("Command History:\n");
    for (int i = 0; i < historyCnt; i++) {
        printf("%d: %s\n", history[i].pid, history[i].cmd);
        if (history[i].background) printf("In Background\n");
        else printf("Execution duration: %ld ms\n", time(NULL) - history[i].execTime);
    }
}

void showHistory() {
    printf("Command History:\n");
    for (int i = 0; i < historyCnt; i++) printf("%s\n", history[i].cmd);
}

int main() {
    setupSignalHandler();
    shell_loop();
    return 0;
}
