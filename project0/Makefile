# Compiler
CC = gcc

# Flags
CFLAGS = -g -Wall

# Default target 
default: penn-shredder

# Build both programs 
all: clean penn-shredder token-shell


# Build the shell (THIS is what the autograder wants)
penn-shredder: penn-shredder.c tokenizer.c tokenizer.h
	$(CC) $(CFLAGS) penn-shredder.c tokenizer.c -o $@

# Keep token-shell available (optional)
token-shell: token-shell.c tokenizer.c tokenizer.h
	$(CC) $(CFLAGS) token-shell.c tokenizer.c -o $@

clean:
	$(RM) -f penn-shredder token-shell *.o