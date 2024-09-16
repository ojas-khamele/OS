#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

// Function to execute a single command
void executeCommand(char **s) {
    // Handle 'cd' command
    if (strcmp(s[0], "cd") == 0) {
        if (chdir(s[1]) != 0) {
            printf("Shell: Incorrect command\n");
        }
        return;
    }

    // Fork a new process to execute the command
    if (fork() == 0) {
        signal(SIGTSTP, SIG_DFL);  // Reset SIGTSTP signal handling
        signal(SIGINT, SIG_DFL);   // Reset SIGINT signal handling
        execvp(s[0], s);          // Execute the command
        printf("Shell: Incorrect command\n"); // Error message for command execution failure
        exit(1);  // Exit child process
    } else {
        wait(NULL);  // Wait for the child process to complete
    }
}

// Function to execute multiple commands in parallel
void executeParallelCommands(char **s, int i) {
    for (int j = 0; j < i; j += 2) {
        if (fork() == 0) {
            char *temp[2];
            temp[0] = s[j];
            temp[1] = NULL;
            executeCommand(temp); // Execute each command
            exit(1);  // Exit child process
        }
    }
    // Wait for all child processes to complete
    for (int j = 0; j < i / 2; j++) {
        wait(NULL);
    }
}

// Function to execute multiple commands sequentially
void executeSequentialCommands(char **s, int i) {
    for (int j = 0; j < i; j += 2) {
        if (fork() == 0) {
            char *temp[2];
            temp[0] = s[j];
            temp[1] = NULL;
            executeCommand(temp); // Execute each command
            exit(1);  // Exit child process
        } else {
            wait(NULL);  // Wait for the child process to complete
        }
    }
}

// Function to handle command output redirection
void executeCommandRedirection(char **s, int i) {
    FILE *file;
    
    // Open file for output redirection based on the operator
    if (strcmp(s[1], ">") == 0) {
        file = fopen(s[2], "w"); // Redirecting
    } else {
        file = fopen(s[2], "a"); // Appending to the file
    }

    if (file == NULL) {
        printf("Shell: Incorrect command\n");
        return;
    }
    
    dup2(fileno(file), STDOUT_FILENO); // Redirect output to the file
    fclose(file);

    if (fork() == 0) {
        signal(SIGTSTP, SIG_DFL);  // Reset SIGTSTP signal handling
        signal(SIGINT, SIG_DFL);   // Reset SIGINT signal handling
        s[1] = NULL;  // Remove redirection operator from arguments
        execvp(s[0], s);  // Execute the command
        printf("Shell: Incorrect command\n"); // Error message for command execution failure
        exit(1);  // Exit child process
    } else {
        wait(NULL);  // Wait for the child process to complete
    }

    // Restore stdout to terminal
    freopen("/dev/tty", "w", stdout);
}

// Placeholder for handling SIGINT (Ctrl + C)
void handle_sigint() {
    // Custom handling code here (if needed)
}

// Placeholder for handling SIGTSTP (Ctrl + Z)
void handle_sigtstp() {
    // Custom handling code here (if needed)
}

int main() {
    while (1) {
        // Setup signal handlers
        signal(SIGINT, handle_sigint);    // Handle Ctrl + C
        signal(SIGTSTP, handle_sigtstp);  // Handle Ctrl + Z
        
        char buffer[100];
        
        // Print the shell prompt with current working directory
        if (getcwd(buffer, sizeof(buffer))) {
            printf("%s$", buffer);
        } else {
            printf("Shell: Incorrect command\n");
        }

        // Read input from user
        char *inputBuff;
        size_t bufSize = 50;
        size_t characters;
        inputBuff = (char*) malloc(sizeof(char) * bufSize);
        
        if (inputBuff == NULL) {
            printf("Shell: Incorrect command\n");
            break;
        }

        characters = getline(&inputBuff, &bufSize, stdin);
        char *token[10];
        int i = 0;
        
        // Tokenize the input string
        while ((token[i] = strsep(&inputBuff, " ")) != NULL) {
            if (strlen(token[i]) == 0) continue; // Skip empty tokens
            i++;
        }

        // Remove newline characters from tokens
        for (int j = 0; j < i; j++) {
            token[j] = strsep(&token[j], "\n");
        }

        // Handle exit command
        if (i > 0 && strcmp(token[0], "exit") == 0) {
            printf("Exiting shell...\n");
            break;
        }

        // Execute commands based on delimiters
        if (i > 2 && strcmp(token[1], "&&") == 0) {
            executeParallelCommands(token, i);
        } else if (i > 2 && strcmp(token[1], "##") == 0) {
            executeSequentialCommands(token, i);
        } else if (i > 2 && (strcmp(token[1], ">") == 0 || strcmp(token[1], ">>") == 0)) {
            executeCommandRedirection(token, i);
        } else {
            executeCommand(token);
        }

        free(inputBuff); // Free allocated memory
    }
    return 0;
}
