#ifndef STRINGDYN_H
#define STRINGDYN_H

#include <stdint.h>
#include <string.h>

struct inputstring;

struct inputstring * inputstring_new(const char * str);

int inputstring_init(struct inputstring *self);
int inputstring_deinit(struct inputstring *self);

int inputstring_concat_cstring(struct inputstring *self, const char *str);
int inputstring_insert_cstring(struct inputstring *self, const char *str, size_t position);

int inputstring_append_char(struct inputstring *self, const char ch);
int inputstring_insert_char(struct inputstring *self, const char ch, size_t position);
int inputstring_delete_char(struct inputstring *self, size_t position);


const char * inputstring_get_cstring(struct inputstring *self);
size_t inputstring_get_length(struct inputstring *self);
void inputstring_reset(struct inputstring *self);

void inputstring_print_internals(struct inputstring *self);

#endif // !STRINGDYN_H
