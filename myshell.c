/********************************************************************************************
 * myshell.c - Assignment 1
 *
 * Supports:
 *   - Single commands
 *   - Built-in: cd <path>, exit
 *   - Parallel commands (&&)
 *   - Sequential commands (##)
 *   - Output redirection (>)
 *   - Prompt: <cwd>$
 *   - Ignores Ctrl+C / Ctrl+Z in shell, restores default in child
 *
 * Messages required:
 *   Shell: Incorrect command\n
 *   Exiting shell...\n
 ********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

/* ----------------- Parser ----------------- */
char **parseInput(char *input) {
    char **args = malloc(64 * sizeof(char *));
    int idx = 0;
    char *tok = strtok(input, " ");
    while (tok != NULL) {
        args[idx++] = tok;
        tok = strtok(NULL, " ");
    }
    args[idx] = NULL;
    return args;
}

int contains(char **args, const char *sym) {
    for (int i = 0; args[i]; i++)
        if (strcmp(args[i], sym) == 0) return 1;
    return 0;
}

/* ----------------- Execute single command ----------------- */
void executeCommand(char **args) {
    if (!args[0]) return;

    // built-in cd
    if (strcmp(args[0], "cd") == 0) {
        if (!args[1] || chdir(args[1]) == -1)
            printf("Shell: Incorrect command\n");
        return;
    }

    // built-in exit
    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }

    // external
    pid_t pid = fork();
    if (pid < 0) {
        printf("Shell: Incorrect command\n");
    } else if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        if (execvp(args[0], args) < 0)
            printf("Shell: Incorrect command\n");
        exit(0);
    } else {
        wait(NULL);
    }
}

/* ----------------- Parallel (&&) ----------------- */
void executeParallelCommands(char **args) {
    char *cmds[20][50];
    int cIdx = 0, aIdx = 0, total = 1;

    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], "&&") == 0) {
            cmds[cIdx][aIdx] = NULL;
            cIdx++; aIdx = 0; total++;
        } else {
            cmds[cIdx][aIdx++] = args[i];
        }
    }
    cmds[cIdx][aIdx] = NULL;

    pid_t pids[20];
    for (int j = 0; j < total; j++) {
        pids[j] = fork();
        if (pids[j] == 0) {
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            if (execvp(cmds[j][0], cmds[j]) < 0)
                printf("Shell: Incorrect command\n");
            exit(0);
        }
    }
    for (int j = 0; j < total; j++) wait(NULL);
}

/* ----------------- Sequential (##) ----------------- */
void executeSequentialCommands(char **args) {
    char *cmd[50];
    int idx = 0;
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], "##") == 0) {
            cmd[idx] = NULL;
            executeCommand(cmd);
            idx = 0;
        } else {
            cmd[idx++] = args[i];
        }
    }
    cmd[idx] = NULL;
    if (idx > 0) executeCommand(cmd);
}

/* ----------------- Redirection (>) ----------------- */
void executeCommandRedirection(char **args) {
    char *outfile = NULL;
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], ">") == 0) {
            args[i] = NULL;
            outfile = args[i+1];
            break;
        }
    }
    if (!outfile) {
        printf("Shell: Incorrect command\n");
        return;
    }

    int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        printf("Shell: Incorrect command\n");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        dup2(fd, STDOUT_FILENO);
        close(fd);
        if (execvp(args[0], args) < 0)
            printf("Shell: Incorrect command\n");
        exit(0);
    } else {
        close(fd);
        wait(NULL);
    }
}

/* ----------------- Main ----------------- */
int main() {
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    while (1) {
        char cwd[128];
        getcwd(cwd, sizeof(cwd));
        printf("%s$", cwd);

        char *line = NULL;
        size_t cap = 0;
        getline(&line, &cap, stdin);
        if (line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';
        if (strlen(line) == 0) { free(line); continue; }

        if (strcmp(line, "exit") == 0) {
            printf("Exiting shell...\n");
            free(line);
            break;
        }

        char **args = parseInput(line);

        if (contains(args, "&&")) executeParallelCommands(args);
        else if (contains(args, "##")) executeSequentialCommands(args);
        else if (contains(args, ">"))  executeCommandRedirection(args);
        else executeCommand(args);

        free(args);
        free(line);
    }
    return 0;
}
