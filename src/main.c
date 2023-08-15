#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char **argv)
{
    printf("Hello, World!\n");
    printf("argument count = %u\n", argc);
    for (int i = 0; i < argc; i++) {
	printf("Argument [%u] = %s\n", i, argv[i]);
    }

    int stdin_pipe[2];
    int stdout_pipe[2];
    int stderr_pipe[2];
    if(pipe(stdin_pipe) == -1) {
	perror("Pipe stdin: ");
	return EXIT_FAILURE;
    } else {
	printf("Opened stdin pipe! %d, %d\n", stdin_pipe[0], stdin_pipe[1]);
    }
    if(pipe(stdout_pipe) == -1) {
	perror("Pipe stdout: ");
	return EXIT_FAILURE;
    } else {
	printf("Opened stdout pipe! %d, %d\n", stdout_pipe[0], stdout_pipe[1]);
    }
    if(pipe(stderr_pipe) == -1) {
	perror("Pipeerr: ");
	return EXIT_FAILURE;
    } else {
	printf("Opened stderr pipe! %d, %d\n", stderr_pipe[0], stderr_pipe[1]);
    }


    pid_t pid = fork();
    if (pid == -1) {
	perror("fork");
    }

    if (pid) {
	// PARENT
	printf("hello from parent PID = %d\n", pid);

	// close the unneeded ends of the pipe
	
	// close the sending end of stdin
	close(stdin_pipe[0]);
	// close the receiving end of stdout
	close(stdout_pipe[1]);
	// close the receiving end of stderr
	close(stderr_pipe[1]);
	
	wait(NULL);
	int n;
	char buffer[1024];
	printf("Reading...\n");
	while((n = read(stdout_pipe[0], buffer, 1024)) > 0) {
	    buffer[n] = '\0';
	    printf("Reading from stdout pipe: %s\n", buffer);
	}

	while((n = read(stderr_pipe[0], buffer, 1024)) > 0) {
	    buffer[n] = '\0';
	    printf("Reading from stderr pipe: %s\n", buffer);
	}
	printf("DONE!\n");


    } else {
	// CHILD
	close(stdin_pipe[1]);
	close(stdout_pipe[0]);
	close(stderr_pipe[0]);

	// map the receiving end of the stdin pipe to the child stdin
	dup2(stdin_pipe[0], fileno(stdin));
	close(stdin_pipe[0]);

	// map the sending end of the stdout pipe to the child stdout
	dup2(stdout_pipe[1], fileno(stdout));
	close(stdout_pipe[1]);

	// map the sending end of the stderr pipe to the child stderr
	dup2(stderr_pipe[1], fileno(stderr));
	close(stderr_pipe[1]);

	// this should be read by the other end of hte pipe
	// this will also not be printed when the other end does not read the pipe
	printf(" -*****- hello from child PID = %d -*****-\n", pid);
	char *msg =  " -*****- CHILD DONE! -*****-\n";
	(void)write(fileno(stderr), msg, strlen(msg));


	/*
	char * const arg[] = {
	    "-ah",
	    "-l",
	    NULL
	};
	int ret = execvp("ls", arg);
	*/
	int ret = execvp(argv[1], &argv[1]);
	if (ret == -1) {
	    perror("execvp");
	}


    }

    return 0;
}
