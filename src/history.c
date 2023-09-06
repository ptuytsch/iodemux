/*
 * This file will contain the logic to read/write the history that has been entered
 *
 * The history file will be of the following format:
 *
 * ```
 * [
 *  <CMD 1>
 *  <CMD 1 ARG 1>
 * ]
 * <history 1>
 * <history 2>
 * <history 3>
 * [
 *  <CMD 2>
 * ]
 * <history 1>
 * <history 2>
 * <history 3>
 * ```
*/
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



#define FILE_READ_CHUNK_SIZE 512


struct history_map_root_node {
    FILE *hist_file;
    char *command_name;
    struct history_map_input_element *last;
};

struct history_map_input_element {
    char *command;
    struct history_map_input_element *next;
    struct history_map_input_element *prev;
};

enum reading_mode {
    idle,
    read_in_command,
    read_in_history,
    skip_rest_of_command,
};

static struct history_map_root_node hist_map;
static struct history_map_input_element *selected_item;

static void history_log_list(int n)
{
    struct history_map_input_element *walker = hist_map.last;
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
    struct history_map_input_element* node_elem = 
	malloc(sizeof(struct history_map_input_element));
    if (node_elem == NULL) {
	perror("malloc history node elem");
	return -1;
    }
    char *input_line = malloc(cmd_length);
    // memcpy(input_line, cmd, cmd_length);
    strcpy(input_line, cmd);
    node_elem->command = input_line;
    // set the next of the current last to the new node
    if (hist_map.last != NULL) {
	hist_map.last->next = node_elem;
    }
    // set the previous history line to the current last
    node_elem->prev = hist_map.last;
    // the next is not known at this point
    node_elem->next = NULL;
    // set the new node as the current last
    hist_map.last = node_elem;

    log_debug("Last history commands");
    history_log_list(10);
    return 0;
}



int history_open_file(const char *filename)
{
    log_debug("opening history file: %s", filename);
    hist_map.hist_file = fopen(filename, "rw");
    if (hist_map.hist_file == NULL) {
	log_error("Could not open %s: %s", filename, strerror(errno));
    }
    return hist_map.hist_file == NULL ? -1 : 0;
}


// Read in the history for one particular command
// for now, only the cmd will be used to parse the history file
// This can be expanded with extra options to furhter define fine
// grained control about how to work with history
int history_read_in_file(const char *cmd)
{
    if (hist_map.hist_file == NULL) {
	return -1;
    }
    log_debug("Reading in history file for command: %s", cmd);
    // char buff[FILE_READ_CHUNK_SIZE];
    char *line = NULL;
    char *line_orig = NULL;
    size_t line_length = 0;
    ssize_t n;
    enum reading_mode mode = idle;
    // struct stringdyn *dynamic_buff;
    // stringdyn_new("");

    hist_map.command_name = NULL;
    hist_map.last = NULL;
    selected_item = NULL;

    
    while ( (n = getline(&line_orig, &line_length, hist_map.hist_file)) > 0 ) {

	// first left and right trim the string
	line = line_orig;
	while(isspace(*line)) {
	    line++;
	    line_length--;
	}
	char *back = line + strlen(line) - 1;
	while(isspace(*back) || *back == '\n' || *back == '\r') {
	    *back = '\0';
	    line_length--;
	    back--;
	}

	log_debug("Reading line: '%s'", line);

	if (mode == read_in_command) {

	    // check if the string matches the command passed into the function
	    if (strcmp(cmd, line) == 0) {
		char *cmd_name = (char *)malloc(strlen(line));
		strcpy(cmd_name, line);
		hist_map.command_name = cmd_name;
		mode = skip_rest_of_command;
		log_debug("read in command: %s", cmd_name);
	    } else {
		mode = idle;
	    }

	} else if (mode == skip_rest_of_command) {
	    log_debug("SKIP");
	    if (*line == ']') {
		mode = read_in_history;
	    }

	} else if (mode == read_in_history) {
	    if (line_length > 0) {
		log_debug("read in input '%s' line for cmd '%s'", line, hist_map.command_name);
		history_append(line);
	    }
	} else {
	    if (*line == '[') {
		log_debug("START OF COMMAND BLOCK");
		mode = read_in_command;
	    }
	}
	
    }
    free(line_orig);

    return 0;
}


int history_move_cursor_to_previous() {
    if (hist_map.last == NULL) {
	return -1;
    }

    if (selected_item == NULL) {
	selected_item = hist_map.last;
	return 0;
    }
    if (selected_item->prev == NULL) {
	return -1;
    }

    selected_item = selected_item->prev;

    return 0;
}


int history_move_cursor_to_next() {
    if (hist_map.last == NULL) {
	return -1;
    }

    if (selected_item == NULL) {
	return -1;
    }

    if (selected_item->next == NULL) {
	return -1;
    }

    selected_item = selected_item->next;

    return 0;
}


int history_reset_cursor()
{
    selected_item = NULL;
    return 0;
}


int history_get_previous_input(char *input_string) {
    if (hist_map.last == NULL) {
	return -1;
    }

    if (selected_item == NULL) {
	selected_item = hist_map.last;
    }

    strcpy(input_string, selected_item->command);

    log_debug("returned the previous history string: %s", selected_item->command);

    return 0;
}

const char *history_get_previous_input_ptr() {
    if (hist_map.last == NULL) {
	return NULL;
    }

    if (selected_item == NULL) {
	selected_item = hist_map.last;
    }

    log_debug("returned the history string: %s from [%p]", selected_item->command, selected_item);
    return selected_item->command;
}


const char *history_get_input_ptr() {
    if (hist_map.last == NULL) {
	return NULL;
    }

    if (selected_item == NULL) {
	return NULL;
    }

    log_debug("returned the history string: %s from [%p]", selected_item->command, selected_item);
    return selected_item->command;
}

