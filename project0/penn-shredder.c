/* include these header files to use their functions */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tokenizer.h"
#include <fcntl.h>

/* Macro to universaly define the size of the input.
 *
 * The compiler will replace all instances of this macro with the value.
 *
 * This helps avoid "magic numbers", which you should avoid in your code.
 */
#define INPUT_SIZE 1024

/* This is declared outside of main, so it is a global variable.
 *
 * This is the ONLY global variable you are allowed to use.
 *
 * You must ensure that this value is updated when appropriate.
 *
 * In general, avoid the use of global variables in any code you write.
 */


/* In C, you must declare the function before
 * it is used elsewhere in your program.
 *
 * By defining them at the top of the program, you avoid
 * implied declaration warnings (the compiler will guess that the return value is an int).
 *
 * This is normally implemented as a header (.h) file.
 *
 * You may choose to refactor this into a header file,
 * as long as you update your makefile orrectly.
 */

void executeShell(void);
char *getCommandFromInput();
void registerSignalHandlers();
void writeToStdout(char *text);
pid_t childPid = -1;

/* This is the main function.
 *
 * It will register the signal handlers via function call,
 * check for a timeout (for project1b),
 * and call `executeShell` in a loop indefinitely.
 *
 * DO NOT modify this function, it is correctly implemented for you.
 */
int main(int argc, char **argv) {
    registerSignalHandlers();

    while (1) {
        executeShell();
    }

    return 0;
}

/* Sends SIGKILL signal to the child process identified by `childPid`.
 *
 * It will check for system call errors and exit penn-shredder program if
 * there is an error.
 *
 * DO NOT modify this function, it is correctly implemented for you.
 */
void killChildProcess() {
    if (childPid > 0 && kill(childPid, SIGKILL) == -1) {
        perror("Error in kill");
        exit(EXIT_FAILURE);
    }
}






/* Registers SIGALRM and SIGINT handlers with corresponding functions.
 *
 * Checks for system call failure and exits program if
 * there is an error.
 *
 * TODO: implement the SIGARLM portion in project1b.
 *
 * The SIGINT portion is implemented correctly for you.
 */
void registerSignalHandlers() {
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        perror("invalid: signal");
        exit(EXIT_FAILURE);
    }
}

void executeShell(void) {
    char *command;
    int status;

    writeToStdout("penn-sh> ");

    command = getCommandFromInput();
    if (command == NULL) {
        return;
    }
    if (command[0] == '\0') {
        free(command);
        return;
    }

    TOKENIZER *tz = init_tokenizer(command);
    if (tz == NULL) {
        perror("invalid: tokenizer init");
        free(command);
        return;
    }

    char *argvL[100];
    char *argvR[100];
    int argcL = 0;
    int argcR = 0;

    char *infileL = NULL;
    char *outfileL = NULL;
    char *infileR = NULL;
    char *outfileR = NULL;

    int invalid = 0;
    int sawPipe = 0;
    int side = 0;   /* 0 = left, 1 = right */

    char *tok;

    while ((tok = get_next_token(tz)) != NULL) {

        /* PIPE */
        if (strcmp(tok, "|") == 0) {
            free(tok);

            if (sawPipe) {
                writeToStdout("invalid: multiple pipes\n");
                invalid = 1;
                break;
            }
            if (argcL == 0) {
                writeToStdout("invalid: pipe in invalid location\n");
                invalid = 1;
                break;
            }

            sawPipe = 1;
            side = 1;
            continue;
        }

        /* INPUT REDIRECT */
        if (strcmp(tok, "<") == 0) {
            free(tok);

            if (side == 0) {
                if (infileL != NULL) {
                    writeToStdout("invalid: multiple standard input redirects or redirect in invalid location\n");
                    invalid = 1;
                    break;
                }
                char *nextFile = get_next_token(tz);
                if (nextFile == NULL ||
                    strcmp(nextFile, "<") == 0 ||
                    strcmp(nextFile, ">") == 0 ||
                    strcmp(nextFile, "|") == 0) {
                    if (nextFile) free(nextFile);
                    writeToStdout("invalid: Invalid standard input redirect\n");
                    invalid = 1;
                    break;
                }
                infileL = nextFile;
            } else {
                if (infileR != NULL) {
                    writeToStdout("invalid: multiple standard input redirects or redirect in invalid location\n");
                    invalid = 1;
                    break;
                }
                char *nextFile = get_next_token(tz);
                if (nextFile == NULL ||
                    strcmp(nextFile, "<") == 0 ||
                    strcmp(nextFile, ">") == 0 ||
                    strcmp(nextFile, "|") == 0) {
                    if (nextFile) free(nextFile);
                    writeToStdout("invalid: Invalid standard input redirect\n");
                    invalid = 1;
                    break;
                }
                infileR = nextFile;
            }
            continue;
        }

        /* OUTPUT REDIRECT */
        if (strcmp(tok, ">") == 0) {
            free(tok);

            if (side == 0) {
                if (outfileL != NULL) {
                    writeToStdout("invalid: Multiple standard output redirects\n");
                    invalid = 1;
                    break;
                }
                char *nextOut = get_next_token(tz);
                if (nextOut == NULL ||
                    strcmp(nextOut, "<") == 0 ||
                    strcmp(nextOut, ">") == 0 ||
                    strcmp(nextOut, "|") == 0) {
                    if (nextOut) free(nextOut);
                    writeToStdout("invalid: Invalid standard output redirect\n");
                    invalid = 1;
                    break;
                }
                outfileL = nextOut;
            } else {
                if (outfileR != NULL) {
                    writeToStdout("invalid: Multiple standard output redirects\n");
                    invalid = 1;
                    break;
                }
                char *nextOut = get_next_token(tz);
                if (nextOut == NULL ||
                    strcmp(nextOut, "<") == 0 ||
                    strcmp(nextOut, ">") == 0 ||
                    strcmp(nextOut, "|") == 0) {
                    if (nextOut) free(nextOut);
                    writeToStdout("invalid: Invalid standard output redirect\n");
                    invalid = 1;
                    break;
                }
                outfileR = nextOut;
            }
            continue;
        }

        /* NORMAL ARGUMENT TOKEN */
        if (side == 0) {
            if (argcL >= 99) {
                free(tok);
                writeToStdout("invalid: too many arguments\n");
                invalid = 1;
                break;
            }
            argvL[argcL++] = tok;
        } else {
            if (argcR >= 99) {
                free(tok);
                writeToStdout("invalid: too many arguments\n");
                invalid = 1;
                break;
            }
            argvR[argcR++] = tok;
        }
    }

    free_tokenizer(tz);

    /* pipe cannot end without a right command */
    if (!invalid && sawPipe && argcR == 0) {
        writeToStdout("invalid: pipe in invalid location\n");
        invalid = 1;
    }

    /* overlap rules with pipe */
    if (!invalid && sawPipe) {
        if (outfileL != NULL) {
            writeToStdout("invalid: Invalid output redirection\n");
            invalid = 1;
        }
        if (infileR != NULL) {
            writeToStdout("invalid: Invalid standard input redirect\n");
            invalid = 1;
        }
    }

    if (invalid || argcL == 0) {
        for (int i = 0; i < argcL; i++) free(argvL[i]);
        for (int i = 0; i < argcR; i++) free(argvR[i]);
        if (infileL) free(infileL);
        if (outfileL) free(outfileL);
        if (infileR) free(infileR);
        if (outfileR) free(outfileR);
        free(command);
        return;
    }

    argvL[argcL] = NULL;
    if (sawPipe) {
        argvR[argcR] = NULL;
    }

    /* ---------------- EXECUTION ---------------- */

    if (!sawPipe) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("invalid: fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
                perror("invalid: signal");
                exit(EXIT_FAILURE);
            }

            if (infileL) {
                int fd = open(infileL, O_RDONLY);
                if (fd < 0) {
                    perror("invalid: Invalid standard input redirect");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDIN_FILENO) < 0) {
                    perror("invalid: dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }

            if (outfileL) {
                int fd = open(outfileL, O_WRONLY | O_TRUNC | O_CREAT, 0644);
                if (fd < 0) {
                    perror("invalid: Invalid standard output redirect");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("invalid: dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }

            execvp(argvL[0], argvL);
            perror("invalid: execvp");
            exit(EXIT_FAILURE);
        }

        childPid = pid;

        while (waitpid(pid, &status, 0) >= 0 &&
               !WIFEXITED(status) &&
               !WIFSIGNALED(status)) {
            /* loop */
        }

        childPid = -1;

    } else {
        int fds[2];
        if (pipe(fds) < 0) {
            perror("invalid: pipe");
            for (int i = 0; i < argcL; i++) free(argvL[i]);
            for (int i = 0; i < argcR; i++) free(argvR[i]);
            if (infileL) free(infileL);
            if (outfileL) free(outfileL);
            if (infileR) free(infileR);
            if (outfileR) free(outfileR);
            free(command);
            return;
        }

        pid_t leftPid = fork();
        if (leftPid < 0) {
            perror("invalid: fork");
            close(fds[0]);
            close(fds[1]);
            for (int i = 0; i < argcL; i++) free(argvL[i]);
            for (int i = 0; i < argcR; i++) free(argvR[i]);
            if (infileL) free(infileL);
            if (outfileL) free(outfileL);
            if (infileR) free(infileR);
            if (outfileR) free(outfileR);
            free(command);
            return;
        }

        if (leftPid == 0) {
            if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
                perror("invalid: signal");
                exit(EXIT_FAILURE);
            }

            if (dup2(fds[1], STDOUT_FILENO) < 0) {
                perror("invalid: dup2");
                exit(EXIT_FAILURE);
            }

            close(fds[0]);
            close(fds[1]);

            if (infileL) {
                int fd = open(infileL, O_RDONLY);
                if (fd < 0) {
                    perror("invalid: Invalid standard input redirect");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDIN_FILENO) < 0) {
                    perror("invalid: dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }

            execvp(argvL[0], argvL);
            perror("invalid: execvp");
            exit(EXIT_FAILURE);
        }

        pid_t rightPid = fork();
        if (rightPid < 0) {
            perror("invalid: fork");
            close(fds[0]);
            close(fds[1]);
            waitpid(leftPid, &status, 0);
            for (int i = 0; i < argcL; i++) free(argvL[i]);
            for (int i = 0; i < argcR; i++) free(argvR[i]);
            if (infileL) free(infileL);
            if (outfileL) free(outfileL);
            if (infileR) free(infileR);
            if (outfileR) free(outfileR);
            free(command);
            return;
        }

        if (rightPid == 0) {
            if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
                perror("invalid: signal");
                exit(EXIT_FAILURE);
            }

            if (dup2(fds[0], STDIN_FILENO) < 0) {
                perror("invalid: dup2");
                exit(EXIT_FAILURE);
            }

            close(fds[0]);
            close(fds[1]);

            if (outfileR) {
                int fd = open(outfileR, O_WRONLY | O_TRUNC | O_CREAT, 0644);
                if (fd < 0) {
                    perror("invalid: Invalid standard output redirect");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("invalid: dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }

            execvp(argvR[0], argvR);
            perror("invalid: execvp");
            exit(EXIT_FAILURE);
        }

        close(fds[0]);
        close(fds[1]);

        /* wait for both children */
        waitpid(leftPid, &status, 0);
        waitpid(rightPid, &status, 0);

        childPid = -1;
    }

    /* cleanup memory */
    for (int i = 0; i < argcL; i++) free(argvL[i]);
    for (int i = 0; i < argcR; i++) free(argvR[i]);
    if (infileL) free(infileL);
    if (outfileL) free(outfileL);
    if (infileR) free(infileR);
    if (outfileR) free(outfileR);
    free(command);
}



/* Writes particular text to standard output.
 *
 * Exits penn-shredder if the system call `write` fails.
 *
 * DO NOT modify this function, it is correctly implemnted for you.
 */
void writeToStdout(char *text) {
    if (write(STDOUT_FILENO, text, strlen(text)) == -1) {
        perror("invalid: write");
        exit(EXIT_FAILURE);
    }
}

/* Reads input from standard input using `read` system call.
 * You are not permitted to use other functions like `fgets`.
 *
 * If the user enters Control+D from the keyboard,
 * penn-shredder exits immediately.
 *
 * If the user enters text followed by Control+D,
 * penn-shredder does not exit and does not report an error.
 *
 * Note specifically that Control+D represents End-Of-File (EOF).
 * This is not a character that can be read.  It just tells `read` that
 * there is no more data to read from the keyboard.
 *
 * The leading and trailing spaces must be removed from the user input.
 *
 * The string must be NULL-terminated.
 *
 * Note that the starter code is hardcoded to return "/bin/ls",
 * which will cause an infinite loop as provided.
 *
 * TODO: implement this function for project1a.
 */
char *getCommandFromInput() {
    static char stash[INPUT_SIZE];
    static size_t stash_len = 0;

    char line[INPUT_SIZE];
    size_t line_len = 0;

    while (1) {
        /* 1) Look for newline already in stash */
        int found_nl = 0;
        size_t nl_pos = 0;

        for (size_t i = 0; i < stash_len; i++) {
            if (stash[i] == '\n') {
                found_nl = 1;
                nl_pos = i;
                break;
            }
        }

        if (found_nl) {
            /* Copy up to newline into line */
            size_t take = nl_pos;
            if (take > INPUT_SIZE - 1) take = INPUT_SIZE - 1;

            for (size_t j = 0; j < take; j++) {
                line[j] = stash[j];
            }
            line[take] = '\0';
            line_len = take;

            /* Shift remaining bytes after newline to front of stash */
            size_t remain = stash_len - (nl_pos + 1);
            for (size_t j = 0; j < remain; j++) {
                stash[j] = stash[nl_pos + 1 + j];
            }
            stash_len = remain;
            break;
        }

        /* 2) Need more bytes: read into a temp buffer then append to stash */
        char buffer[INPUT_SIZE];
        ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer));

        if (n == 0) {
            /* EOF */
            if (stash_len == 0) {
                exit(0);
            }

            /* Return whatever is left as final line */
            size_t take = stash_len;
            if (take > INPUT_SIZE - 1) take = INPUT_SIZE - 1;

            for (size_t j = 0; j < take; j++) {
                line[j] = stash[j];
            }
            line[take] = '\0';
            line_len = take;
            stash_len = 0;
            break;
        }

        if (n < 0) {
            perror("invalid: read");
            exit(1);
        }

        /* Append to stash, truncating if stash is full */
        for (ssize_t i = 0; i < n; i++) {
            if (stash_len < INPUT_SIZE - 1) {
                stash[stash_len++] = buffer[i];
            } else {
                /* If line is absurdly long, we just stop buffering more */
                break;
            }
        }
    }

    (void)line_len; /* line_len is set but not strictly needed below */

    /* Trim leading whitespace (space/tab/newline/\r) */
    char *start = line;
    while (*start != '\0' &&
           (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r')) {
        start++;
    }

    /* Find end */
    char *end = start;
    while (*end != '\0') end++;

    /* Trim trailing whitespace */
    while (end > start &&
           (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\n' || end[-1] == '\r')) {
        end--;
    }
    *end = '\0';

    char *out = malloc(strlen(start) + 1);
    if (out == NULL) {
        perror("invalid: malloc");
        exit(1);
    }
    strcpy(out, start);
    return out;
}

