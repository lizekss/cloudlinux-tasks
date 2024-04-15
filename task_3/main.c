#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

static void exit_error(char *info) {
    perror(info);
    exit(EXIT_FAILURE);
}

/* Execute the given program and return its exit status */
static int exec_with_status(char *prog1) {
	// Fork-exec technique
    int pid = fork();

	if (pid == 0) {
		// Child process
        execlp(prog1, prog1, NULL);
        exit_error("execlp"); // Exec only returns on failure
	} else if (pid < 0) {
		// Error
        exit_error("fork");
    } else {
    	// Parent process
        int status;
		wait(&status);
        
        return status;
    }
    
    return 0;
}

/* Execute the program and redirect its output to the given pipe */
static void exec_pipe_out(char *prog2, int *pipe_fd) {
    int pid = fork();

    if (pid == 0) {
        close(pipe_fd[0]); // Close read end of pipe

        // Redirect stdout to write end of pipe
        close(STDOUT_FILENO);
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            exit_error("dup_2");
        }

        // Execute prog2
        execlp(prog2, prog2, NULL);
        exit_error("execlp");
    } else if (pid < 0) {
        exit_error("fork");
    } else {
        wait(NULL);
        close(pipe_fd[1]); // Close write end of pipe in parent process
    }
    
}

/* Execute the program, redirect its input to the given pipe and its output to the given file */
static void exec_pipe_in_file_out(char *prog3, int *pipe_fd, char *file) {
    int pid = fork();
    if (pid == 0) {
        close(pipe_fd[1]); // Close write end of pipe
        // Redirect stdin to read end of pipe
        close(STDIN_FILENO); 
        if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
            exit_error("dup2");
        }

        int fd = open(file, O_CREAT | O_RDWR, 0666); // Open the file for writing
        if (fd < 0) {
            exit_error("open");
        }

        // Redirect stdout to file descriptor
        close(STDOUT_FILENO);
        if (dup2(fd, STDOUT_FILENO) == -1) {
            exit_error("dup2");
        }

        // Execute prog3
        execlp(prog3, prog3, NULL); 
        exit_error("execlp");
    } else if (pid < 0) {
        exit_error("fork");
    } else {
        wait(NULL);
        close(pipe_fd[0]); // Close read end of pipe in parent process
    }
}

/* Implementation of the equivalent to the shell command 'prog1 && prog2 | prog3 > file' */
static void simulate_command(char *prog1, char *prog2, char *prog3, char* file) {
    if (exec_with_status(prog1) != 0) {
        return; // Return if prog1 execution was unsuccessful
    }

    // Here if prog1 was successful - how && should work

    // Create a pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        exit_error("pipe");
    }

    // Execute prog2 and write its output to the pipe (prog2 | ...)
    exec_pipe_out(prog2, pipe_fd);
    
    // Execute prog3, read input from the pipe, and write output to the file (... | prog3 > file)
    exec_pipe_in_file_out(prog3, pipe_fd, file);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s prog1 prog2 prog3 file\n", argv[0]);
        return 1;
    }

    simulate_command(argv[1], argv[2], argv[3], argv[4]);
    return 0;
}
