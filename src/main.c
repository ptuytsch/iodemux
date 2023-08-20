#define _GNU_SOURCE
#include <curses.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include "stringdyn.h"
#include <poll.h>
#include <fcntl.h>

#define BUF_INPUT_STR_SIZE 512
#define INPUT_PROMPT_TEXT ">>>"
#define INPUT_PROMPT_TEXT_LENGTH (strlen(INPUT_PROMPT_TEXT))

void start_parent_app(int stdin_fd, int stdout_fd, int stderr_fd);

int main(int argc, char **argv)
{
    printf("argument count = %u\n", argc);
    for (int i = 0; i < argc; i++) {
	printf("Argument [%u] = %s\n", i, argv[i]);
    }

    int stdin_pipe[2];
    int stdout_pipe[2];
    int stderr_pipe[2];
    int pipe_flags = O_NONBLOCK | O_DIRECT;

    if(pipe2(stdin_pipe, pipe_flags) == -1) {
	perror("Pipe stdin: ");
	return EXIT_FAILURE;
    } else {
	printf("Opened stdin pipe! %d, %d\n", stdin_pipe[0], stdin_pipe[1]);
    }
    if(pipe2(stdout_pipe, pipe_flags) == -1) {
	perror("Pipe stdout: ");
	return EXIT_FAILURE;
    } else {
	printf("Opened stdout pipe! %d, %d\n", stdout_pipe[0], stdout_pipe[1]);
    }
    if(pipe2(stderr_pipe, pipe_flags) == -1) {
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

	start_parent_app(stdin_pipe[1], stdout_pipe[0], stderr_pipe[0]);


    } else {
	// CHILD
	close(stdin_pipe[1]);  // close write side
	close(stdout_pipe[0]);  // close read side
	close(stderr_pipe[0]);  // close read side

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
	// printf(" -*****- hello from child -*****-\n");

	fprintf(stderr, "Running the following command\n");
	fprintf(stderr, " - argv[0] = %s\n", argv[1]);
	for (int i = 2; argv[i] != NULL; i++) {
	    fprintf(stderr, " - argv[%d] = %s\n", i - 1, argv[i]);
	}
	fprintf(stderr, "-------------------\n");

	int ret = execvp(argv[1], &argv[1]);
	if (ret == -1) {
	    fprintf(stderr, " -*****- error exec -*****-\n");
	    perror("execvp");
	}
    }


    return 0;
}


void print_debug(WINDOW *debugscreen, const char *fmt, ...)
{
    /*
    va_list args;
    va_start(args, fmt);
    vwprintw(debugscreen, fmt, args);
    wrefresh(debugscreen);
    va_end(args);
    */
}


void start_parent_app(int stdin_fd, int stdout_fd, int stderr_fd)
{

    struct pollfd *pfds;
    int nfds = 3;
    int num_open_fds = 2;
    pfds = calloc(nfds, sizeof(struct pollfd));
    if (pfds == NULL) {
	perror("POLL CALLOC");
    }

    pfds[0].fd = stdout_fd;
    pfds[0].events = POLLIN;
    pfds[1].fd = stderr_fd;
    pfds[1].events = POLLIN;
    pfds[2].fd = STDIN_FILENO;
    pfds[2].events = POLLIN;

    printf("About to poll()\n");
    int ready;
    int n;
    char buffer[1024];



    printf("Setting up ncurses\n");


    
    WINDOW *ioscreen;
    WINDOW *inputscreen;
    initscr();
    clear();
    refresh();
    curs_set(1);
    ioscreen = newwin(LINES - 4, COLS, 0, 0);
    inputscreen = newwin(2, COLS, LINES -2, 0);
    move(LINES - 4, 0);
    hline('=', COLS);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    keypad(inputscreen, TRUE);
    scrollok(ioscreen, true);


    struct inputstring *inputstr = inputstring_new("");
    int position = 0;
    int ch;
    int x, y;


    wclear(inputscreen);
    wprintw(inputscreen, INPUT_PROMPT_TEXT" %s", inputstring_get_cstring(inputstr));
    wmove(inputscreen, 0, INPUT_PROMPT_TEXT_LENGTH + position + 1);  //place the cursor behind the cursor
    wrefresh(ioscreen);
    wrefresh(inputscreen);
    refresh();




    while(num_open_fds > 0) {
	print_debug(ioscreen, " ----- POLL ----- \n");
	ready = poll(pfds, nfds, -1);
        if (ready == -1)
	    perror("poll");
	wmove(inputscreen, 0, INPUT_PROMPT_TEXT_LENGTH + position + 1);  //place the cursor behind the cursor
	wrefresh(ioscreen);
	wrefresh(inputscreen);
	refresh();

	// FD index 0 == stdout pipe
	if (pfds[0].revents != 0) {
	    print_debug(ioscreen, "  stdOUT; events: %s%s%s\n",
		   (pfds[0].revents & POLLIN)  ? "POLLIN "  : "",
		   (pfds[0].revents & POLLHUP) ? "POLLHUP " : "",
		   (pfds[0].revents & POLLERR) ? "POLLERR " : "");
	    if (pfds[0].revents & POLLIN) {
		while ((n = read(stdout_fd, buffer, 1024)) > 0) {
		    buffer[n] = '\0';
		    // wprintw(ioscreen, "Read %d stdout characters: \n%s\n", n, buffer);
		    wprintw(ioscreen, "%s", buffer);
		    wrefresh(ioscreen);
		    wmove(inputscreen, 0, INPUT_PROMPT_TEXT_LENGTH + position + 1);  //place the cursor behind the cursor
		    
		    wrefresh(inputscreen);
		}
	    }
	    if (pfds[0].revents & POLLHUP) {
		print_debug(ioscreen, "STDOUT pipe closed\n");
		num_open_fds--;
	    }
	}

	// FD index 0 == stdout pipe
	if (pfds[1].revents != 0) {
	    print_debug(ioscreen, "  stdERR; events: %s%s%s\n",
		   (pfds[1].revents & POLLIN)  ? "POLLIN "  : "",
		   (pfds[1].revents & POLLHUP) ? "POLLHUP " : "",
		   (pfds[1].revents & POLLERR) ? "POLLERR " : "");
	    if (pfds[1].revents & POLLIN) {
		while ((n = read(stderr_fd, buffer, 1024)) > 0) {
		    buffer[n] = '\0';
		    // wprintw(ioscreen, "Read %d stdout characters: \n%s\n", n, buffer);
		    wprintw(ioscreen, "%s", buffer);
		    wrefresh(ioscreen);
		    wmove(inputscreen, 0, INPUT_PROMPT_TEXT_LENGTH + position + 1);  //place the cursor behind the cursor
		    wrefresh(inputscreen);
		}
		// n = read(stderr_fd, buffer, 1024);
		// buffer[n] = '\0';
		// // printf("Read %d stderr characters: \n%s\n", n, buffer);
		// // wprintw(ioscreen, "Read %d stderr characters: \n%s\n", n, buffer);
		// wprintw(ioscreen, "%s", buffer);
		// wmove(inputscreen, 0, INPUT_PROMPT_TEXT_LENGTH + position + 1);  //place the cursor behind the cursor
		// wrefresh(ioscreen);
		// wrefresh(inputscreen);
	    }
	    if (pfds[1].revents & POLLHUP) {
		print_debug(ioscreen, "STDERR pipe closed\n");
		num_open_fds--;
	    }
	}

	// FD index 2 == stdin
	if (pfds[2].revents != 0) {
	    print_debug(ioscreen, "  stdIN; events: %s%s%s\n",
		   (pfds[2].revents & POLLIN)  ? "POLLIN "  : "",
		   (pfds[2].revents & POLLHUP) ? "POLLHUP " : "",
		   (pfds[2].revents & POLLERR) ? "POLLERR " : "");
	    if (pfds[2].revents & POLLIN) {
		// n = read(stderr_fd, buffer, 1024);
		// buffer[n] = '\0';
		// printf("Read %d stderr characters: \n%s\n", n, buffer);

		ch = getch();
		switch (ch) {
		case KEY_ENTER:
		case 10:
		    write(stdin_fd, inputstring_get_cstring(inputstr), inputstring_get_length(inputstr));
		    // wprintw(ioscreen, "The entered command is: `%s`\n", inputstring_get_cstring(inputstr));
		    position = 0;
		    inputstring_reset(inputstr);
		    wclear(inputscreen);
		    wprintw(inputscreen, INPUT_PROMPT_TEXT);
		    break;
		case KEY_UP:
		    // wprintw(ioscreen, "up\n");
		    break;
		case KEY_DOWN:
		    // wprintw(ioscreen, "down\n");
		    break;
		case KEY_HOME:
		    // wprintw(ioscreen, "HOME\n");
		    position = 0;
		    break;
		case KEY_END:
		    // wprintw(ioscreen, "END\n");
		    position = inputstring_get_length(inputstr);
		    break;
		case KEY_LEFT:
		    // wprintw(ioscreen, "LEFT\n");
		    if (position > 0) position--;
		    else beep();
		    break;
		case KEY_RIGHT:
		    // wprintw(ioscreen, "RIGHT\n");
		    if (position < inputstring_get_length(inputstr)) position++;
		    else beep();
		    break;
		
		case KEY_BACKSPACE:
		case 127:
		case '\b':
		    // wprintw(ioscreen, "BACKSPACE\n");
		    if (position > 0) {
			inputstring_delete_char(inputstr, position);
			position--;
		    }
		    else beep();
		    break;
		default:
		    // wprintw(ioscreen, "CHAR: %d\n", ch);
		    inputstring_insert_char(inputstr, ch, position);
		    position++;
		    break;
	
		}
	    wclear(inputscreen);
	    wprintw(inputscreen, INPUT_PROMPT_TEXT" %s", inputstring_get_cstring(inputstr));
	    // wrefresh(ioscreen);
	    wrefresh(inputscreen);
	    wmove(inputscreen, 0, INPUT_PROMPT_TEXT_LENGTH + position + 1);  //place the cursor behind the cursor
	    wrefresh(inputscreen);
	    }
	    if (pfds[1].revents & POLLHUP) {
		print_debug(ioscreen, "STDIN pipe closed\n");
		num_open_fds--;
	    }
	}
    }
    endwin();
}

