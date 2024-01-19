#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "stringdyn.h"
#include "log.h"
#include "history.h"


struct hist_elem {
    char *command;
    struct hist_elem *next;
    struct hist_elem *prev;
};

struct hist_elem *current = NULL;
struct hist_elem *last = NULL;

static void history_log_list(int n)
{
    struct hist_elem *walker = last;
    log_debug("Last element is set to pointer: %p", walker);
    while (walker != NULL && n > 0) {
        log_debug("hisory line: [%p] %s %p %p", walker, walker->command, walker->prev, walker->next);
        walker = walker->prev;
        n--;
    }
}


int history_append(const char  *const cmd)
{
    size_t cmd_length = strlen(cmd);

    if (cmd_length == 1) {
        return 0;
    }

    const char *previous = history_get_previous_input_ptr();
    if (previous != NULL &&
        strcmp(cmd, previous) == 0) {
        log_debug("Identical string to the previous, skip!");
        return 0;
    }

    char *input_line = malloc(cmd_length);
    if (input_line == NULL) {
        perror("malloc: history append, input_line");
        return -1;
    }

    struct hist_elem *elem = malloc(sizeof(struct hist_elem));
    if (elem == NULL) {
        perror("malloc: history append, elem");
        free(input_line);
        return -1;
    }


    // memcpy(input_line, cmd, cmd_length);

    strcpy(input_line, cmd);
    elem->command = input_line;

    // set the next of the current last to the new node
    if (last != NULL) {
	last->next = elem;
    }

    // set the previous history line to the current last
    elem->prev = last;
    // the next is not known at this point
    elem->next = NULL;

    // set the new node as the current last
    last = elem;

    log_debug("Last history commands");
    history_log_list(10);

    return 0;
}



int history_move_cursor_to_previous() {
    if (last == NULL) {
        return -1;
    }

    if (current == NULL) {
        current = last;
        return 0;
    }

    if (current->prev == NULL) {
        return -1;
    }

    current = current->prev;

    return 0;
}


int history_move_cursor_to_next() {
    if (last == NULL) {
        return -1;
    }

    if (current == NULL) {
        return -1;
    }

    if (current->next == NULL) {
        return -1;
    }

    current = current->next;

    return 0;
}


int history_reset_cursor()
{
    current = NULL;
    return 0;
}


int history_get_previous_input(char *input_string) {
    if (last == NULL) {
        return -1;
    }

    if (current == NULL) {
        current = last;
    }

    strcpy(input_string, current->command);

    log_debug("returned the previous history string: %s", current->command);

    return 0;
}

const char *history_get_previous_input_ptr() {
    if (last == NULL) {
        return NULL;
    }

    if (current == NULL) {
        current = last;
    }

    log_debug("returned the history string: %s from [%p]", current->command, current);
    return current->command;
}


const char *history_get_input_ptr() {
    if (last == NULL) {
        return NULL;
    }

    if (current == NULL) {
        return NULL;
    }

    log_debug("returned the history string: %s from [%p]", current->command, current);
    return current->command;
}

