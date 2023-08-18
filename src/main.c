#include "stdio.h"
#include <curses.h>
#include <stdint.h>
#include "stringdyn.h"

#define BUF_INPUT_STR_SIZE 512
#define INPUT_PROMPT_TEXT ">>> "



int main(int argc, char **argv)
{
    printf("-----------------------------------\n");
    // struct inputstring *inputstr;
    // inputstr = inputstring_new("testerken");
    // printf("-----------------------------------\n");
    // printf("content = %s\n", inputstring_get_cstring(inputstr));
    // printf("-----------------------------------\n");
    // inputstring_concat_cstring(inputstr, " Dit is een test.");
    // printf("-----------------------------------\n");
    // printf("content = %s\n", inputstring_get_cstring(inputstr));
    // printf("-----------------------------------\n");
    // inputstring_append_char(inputstr,'t');
    // printf("-----------------------------------\n");
    // printf("content = %s\n", inputstring_get_cstring(inputstr));
    // printf("-----------------------------------\n");
    // inputstring_concat_cstring(inputstr, "very long string to go over the limit....");
    // printf("-----------------------------------\n");
    // printf("content = %s\n", inputstring_get_cstring(inputstr));
    // printf("-----------------------------------\n");
    // inputstring_insert_char(inputstr,'@', 2);
    // printf("-----------------------------------\n");
    // printf("content = %s\n", inputstring_get_cstring(inputstr));
    // printf("-----------------------------------\n");
    // inputstring_insert_cstring(inputstr,"####", 5);
    // printf("-----------------------------------\n");
    // printf("content = %s\n", inputstring_get_cstring(inputstr));
    // printf("-----------------------------------\n");
    // inputstring_print_internals(inputstr);
    // return 0;

    initscr();
    clear();
    curs_set(0);
    WINDOW *ioscreen = newwin(LINES - 10, COLS-4, 1, 2);
    WINDOW *inputscreen = newwin(5, COLS-4, LINES - 7, 2);
    // wrefresh(ioscreen);
    // wrefresh(inputscreen);
    // refresh();
    // box(ioscreen, 0, 0);
    // box(ioscreen, '|', '-');
    // wborder(ioscreen, '|', '|', '-', '-', '+', '+', '+', '+');
    // noecho();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    keypad(inputscreen, TRUE);

    scrollok(ioscreen, true);

    wclear(inputscreen);

    // box(inputscreen, 0, 0);
    // box(inputscreen, '-', '|');
    // wborder(inputscreen, '|', '|', '-', '-', '+', '+', '+', '+');

    wrefresh(ioscreen);
    wrefresh(inputscreen);
    refresh();
    wmove(inputscreen, 1, 1);
    wprintw(inputscreen, INPUT_PROMPT_TEXT);


    // char buff[BUF_INPUT_STR_SIZE];
    // unsigned int buff_index = 0;
    struct inputstring *inputstr = inputstring_new("");
    int position = 0;


    // mvwgetnstr(inputscreen, 0, 1, buff, BUF_INPUT_STR_SIZE);
    // wrefresh(ioscreen);
    // wprintw(ioscreen, "The entered string is: %s\n", buff);
    // wrefresh(ioscreen);
    int ch;
    int x, y;
    for (;;) {
    // box(ioscreen, 0, 0);
    // box(inputscreen, 0, 0);
    // int ch = getch();
    ch = wgetch(inputscreen);
    // wprintw(ioscreen, "KEY = %d\n", ch);
    switch (ch) {
    case KEY_ENTER:
    case 10:
        wprintw(ioscreen, "The entered command is: `%s`\n", inputstring_get_cstring(inputstr));
	position = 0;
        inputstring_reset(inputstr);
        wclear(inputscreen);
        wprintw(inputscreen, INPUT_PROMPT_TEXT);
        break;
    case KEY_UP:
        wprintw(ioscreen, "up\n");
        break;
    case KEY_DOWN:
        wprintw(ioscreen, "down\n");
        break;
    case KEY_LEFT:
        wprintw(ioscreen, "LEFT\n");
        if (position > 0) position--;
        else beep();
        break;
    case KEY_RIGHT:
        wprintw(ioscreen, "RIGHT\n");
        if (position < inputstring_get_length(inputstr)) position++;
        else beep();
        break;
    
    case KEY_BACKSPACE:
    case 127:
    case '\b':
        wprintw(ioscreen, "BACKSPACE\n");
	inputstring_delete_char(inputstr, position);
	position--;
        // getyx(inputscreen, y, x);
        // mvwinch(inputscreen, y, x - 1);
        // waddch(inputscreen, ' ');
        // mvwinch(inputscreen, y, x - 1);
        // buff[buff_index--] = '\0';
        break;
    default:
        wprintw(ioscreen, "CHAR: %d\n", ch);
	inputstring_insert_char(inputstr, ch, position);
	position++;
        // buff[buff_index] = ch;
        // buff_index++;
        // buff[buff_index] = '\0';
        // wmove(inputscreen, 0, buff_index + 5);
    
        // waddch(inputscreen, ch);
        break;
    
    }
    // wmove(inputscreen, 1, buff_index + 2);
    wclear(inputscreen);
    wprintw(inputscreen, INPUT_PROMPT_TEXT" %s", inputstring_get_cstring(inputstr));
    wrefresh(ioscreen);
    wrefresh(inputscreen);
    refresh();
    }
    endwin();
}




/*
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


	int ret = execvp(argv[1], &argv[1]);
	if (ret == -1) {
	    perror("execvp");
	}


    }

    return 0;
}
*/
