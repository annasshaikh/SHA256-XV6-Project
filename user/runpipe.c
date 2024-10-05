#include "user.h"

void runpipe(int pfd[]);

int main(int argc, char **argv) {
    int pid, status;
    int fd[2];

    pipe(fd); // Create a pipe

    switch (pid = fork()) {
        case 0: // Child process
            runpipe(fd);
            exit(0); // Exit child process

        case -1: // Fork error
            printf(2, "Error: fork failed\n");
            exit(1);

        default: // Parent process
            while ((pid = wait(&status)) != -1) {
                printf(2, "Process %d exits with %d\n", pid, status);
            }
            break;
    }
    exit(0);
}

void runpipe(int pfd[]) {
    int pid;
    switch (pid = fork()) {
        case 0: // Child process for the first command
            dup(pfd[1]); // Redirect stdout to pipe
            close(pfd[0]); // Close read end of pipe
            close(pfd[1]); // Close original write end
            char *cmd1[] = {"/bin/ls", "-al", "/", 0}; // Command to execute
            exec(cmd1[0], cmd1); // Execute command
            printf(2, "Error: exec cmd1 failed\n");
            exit(1);

        default: // Parent process for the second command
            dup(pfd[0]); // Redirect stdin to pipe
            close(pfd[0]); // Close original read end
            close(pfd[1]); // Close write end of pipe
            char *cmd2[] = {"/usr/bin/tr", "a-z", "A-Z", 0}; // Command to execute
            exec(cmd2[0], cmd2); // Execute command
            printf(2, "Error: exec cmd2 failed\n");
            exit(1);

        case -1: // Fork error
            printf(2, "Error: fork failed\n");
            exit(1);
    }
}
