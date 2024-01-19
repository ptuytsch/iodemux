#ifndef HISTORY_H
#define HISTORY_H

int history_move_cursor_to_previous();
int history_move_cursor_to_next();
int history_reset_cursor();

int history_get_previous_input(char *input_string);
const char *history_get_input_ptr();
const char *history_get_previous_input_ptr();


int history_append(const char  *const cmd);

#endif // !HISTORY_H
