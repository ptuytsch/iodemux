#ifndef HISTORY_H
#define HISTORY_H
/*
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


/*
 * Function
 * desc: Opens the history file that is found on the filename location
 *
 * For now it only is the first argument of the command that is read in.
 * Furhter options for finegraned control of what command or which history
 * to select can be added
*/
int history_open_file(const char *filename);

/*
 * Function
 * desc: Reads in the opened history file for a specific command
 *
 * For now it only is the first argument of the command that is read in.
 * Furhter options for finegraned control of what command or which history
 * to select can be added
*/
int history_read_in_file(const char *cmd);


int history_move_cursor_to_previous();
int history_move_cursor_to_next();
int history_reset_cursor();

int history_get_previous_input(char *input_string);
const char *history_get_input_ptr();
const char *history_get_previous_input_ptr();


int history_append(const char  *const cmd);

#endif // !HISTORY_H
