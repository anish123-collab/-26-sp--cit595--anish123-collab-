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
    if (kill(childPid, SIGKILL) == -1) {
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
        perror("Error in signal");
        exit(EXIT_FAILURE);
    }
}

/* Prints the shell prompt and waits for input from user.
 *
 * It then creates a child process which executes the command.
 *
 * The parent process waits for the child and checks that
 * it was either signalled or exited.
 *
 * TODO: you may modify this function for project1a and project1b.
 *
 * TODO: implement the alarm portion in project1b.
 * Use the timeout argument to start an alarm of that timeout period.
 * */
void executeShell(voidf) {
    char *command;
    int status;
    writeToStdout("penn-sh>");

    command = getCommandFromInput();
    if(command == NULL){
        return;
    }
    if(command[0] == '\0'){
        free(command);
        continue;
    }
     // Tokenize
    TOKENIZER *tz = init_tokenizer(command);
    if (tz == NULL) {
        perror("invalid: tokenizer init");
        free(command);
        return;
    }

    char *argv[100];
    int argc = 0;
    char *infile = NULL;
    char *outfile = NULL;

    int invalid = 0;
    char *tok;
     while( (tok = get_next_token(tz)) != NULL ){
        if(strcmp(tok,'<') = 0){
            free(tok);
            if(infile != NULL){
                 writeToStdout("Invalid reirection");
                 invalid = 1;
                 break;

            }
            char *nextFile = get_next_token(tz);
            if(nextFile == NULL|| strcmp(nextFile,'<')== 0|| strcmp(nextFile, '>') == 0){
                if(nextFile){
                    free(nextFile);
                }
                writeToStdout("Invalid reirection-file");
                invalid = 1;
                break;

            }
            infile = nextFile;
        } else if( strcmp(tok,'>') = 0){
            free(tok);
            if(outfile != NULL){
                 writeToStdout("Invalid reirection");
                 invalid = 1;
                 break;

            }
            char *nextOutFile = get_next_token(tz);
            if(nextOutFile == null || strcmp(nextOutFile,'<')== 0|| strcmp(nexOutFile, '>') == 0){
                if(nextFile){
                    free(nextFile);
                }
                writeToStdout("Invalid reirection-file");
                invalid = 1;
                break;

            }
           outfile = nextOutFile;
        }
        else{
            if (argc >= 99) {
                free(tok);
                writeToStdout("invalid: too many arguments\n");
                invalid = 1;
                break;
            }
            argv[argc++] = tok;
        }
        
     }
     free_tokenizer(tz);
     if(invalid == 1|| argc == 0){
        for(int i = 0; i < argc; i++ ){
            free(argc[i])
            if(infile){
                free(infile);
            }
            if(outfile){
                free(outfile);
            }
            free(command);
            return;
        }
     }
     argv[argc] = NULL;

    pid_t pid = fork();

        if (pid< 0) {
            perror("invalid: Error in creating child process");
            for(int i = 0; i < argc; i++ ){
            free(argc[i])
            if(infile){
                free(infile);
            }
            if(outfile){
                free(outfile);
            }
            free(command);
            exit(EXIT_FAILURE);
            return;
        }

        if (pid == 0) {
            f (signal(SIGINT, SIG_DFL) == SIG_ERR) {
            perror("invalid: signal");
            exit(EXIT_FAILURE);
         }

        if(infile){
            int number;
            number = open(infile,O_RDONLY);
            if(number < 0){
                perror("Invalid opening infile");
                exit(EXIT_FAILURE);
            }
            int dupNum
            dupNum = dup2(number,STDIN_FILENO)
            if(dupNum < 0){
                perror("Invalid opening dup2");
                exit(EXIT_FAILURE);
            }
            close(fd);
        }
        if(outfile){
            int number1;
            number1 = open(outfile, O_WRONLY | O_TRUNC | O_CREAT, 0644);
            if(number1 < 0){
                perror("Invalid opening infile");
                exit(EXIT_FAILURE);
            }
            int dupNum1
            dupNum1 = dup2(number1,STDOUT_FILENO)
            if(dupNum1 < 0){
                perror("Invalid opening dup2");
                exit(EXIT_FAILURE);
            }
            close(fd);
        }
         

            
            execvp(argv[0],argv);
            perror("invalid: Error in execve");
            free(command);
            exit(EXIT_FAILURE);
        } else {
            do {
                if (waitpid(pid,&status, 0) < 0) {
                    perror("invalid: Error in child process termination");
                    free(command);
                    exit(EXIT_FAILURE);
                    break;
                }
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        for(int i = 0; i < argc; i++ ){
            free(argc[i])
            if(infile){
                free(infile);
            }
            if(outfile){
                free(outfile);
            }
            free(command);
    }
    
}

/* Writes particular text to standard output.
 *
 * Exits penn-shredder if the system call `write` fails.
 *
 * DO NOT modify this function, it is correctly implemnted for you.
 */
void writeToStdout(char *text) {
    if (write(STDOUT_FILENO, text, strlen(text)) == -1) {
        perror("Error in write");
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
    char buffer[INPUT_SIZE];
    char line[INPUT_SIZE];
    size_t total = 0;
    ssize_t number;
    int done =0;
    line[0] = '\0';
    while(!done){
    number = read(STDIN_FILENO,buffer,sizeof(buffer));
    if(number == 0){
        if(total == 0){
        exit(0);
        }
        break;
    }
    if(number == -1){
        perror("invalid: read");
        exit(1);

    }
     for(ssize_t i = 0; i < number; i++){
        if(buffer[i] == '\n'){
            done = 1;
            break;
        }
        if(total < INPUT_SIZE -1){
            line[total++] = buffer[i];
        } else{
            done = 1;
            break;
        }
    }
    }
    

    line[total] = '\0';

    char* start = line;
    while(*start != '\0' &&( *start == ' ' || *start == '\t' || *start == '\n' || *start == '\r')){
        start++;
         
        
    }
    char* end = line;
    while(*end != '\0'){
        end++;
    }
    while(end > start && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\n' || end[-1] =='\r')){
        end--;
    }

    *end = '\0';

    char* pointer = malloc(strlen(start) + 1);
    if(pointer == NULL){
        perror("invalid: malloc");
        exit(1);
    }
    strcpy(pointer,start);

    return pointer;

}


