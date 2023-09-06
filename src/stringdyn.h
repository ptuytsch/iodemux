#ifndef STRINGDYN_H
#define STRINGDYN_H

#include <stdint.h>
#include <string.h>

struct stringdyn;

struct stringdyn * stringdyn_new(const char * str);

int stringdyn_init(struct stringdyn *self);
int stringdyn_deinit(struct stringdyn *self);

int stringdyn_concat_cstring(struct stringdyn *self, const char *str);
int stringdyn_insert_cstring(struct stringdyn *self, const char *str, size_t position);

int stringdyn_append_char(struct stringdyn *self, const char ch);
int stringdyn_insert_char(struct stringdyn *self, const char ch, size_t position);
int stringdyn_delete_char(struct stringdyn *self, size_t position);


const char * stringdyn_get_cstring(struct stringdyn *self);
int stringdyn_set_cstring(struct stringdyn *self, const char *str);
size_t stringdyn_get_length(struct stringdyn *self);
void stringdyn_reset(struct stringdyn *self);

void stringdyn_print_internals(struct stringdyn *self);

#endif // !STRINGDYN_H
