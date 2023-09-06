#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


#define START_STRING_SIZE 64


struct stringdyn{
    char *content;
    size_t length;
    size_t capacity;
};

//===========================================

int stringdyn_expand_capacity(struct stringdyn *self)
{
    char *new_content = realloc(self->content, self->capacity * 2);
    if (new_content == NULL) {
        return -1;
    }
    self->content = new_content;
    self->capacity *= 2;
    return 0;
    // size_t new_size = self->capacity * 2;
    // void *ptr = malloc(new_size);
    // if (ptr == NULL) {
    //     perror("MALLOC stringdyn, expand");
    //     return -1;
    // }
    // memcpy(ptr, self->content, self->capacity);
    // free(self->content);
    // self->content = ptr;
    // self->capacity = new_size;
    // return 0;
}


void stringdyn_print_internals(struct stringdyn *self)
{
    printf("stringdyn @ %p\n", self);
    printf("content: %s\n", self->content);
    printf("length: %lu\n", self->length);
    printf("capacity: %lu\n", self->capacity);
}

//===========================================

int stringdyn_init(struct stringdyn *self)
{
    void *ptr = malloc(START_STRING_SIZE);
    if (ptr == NULL) {
        perror("MALLOC stringdyn");
        return -1;
    }
    self->content = ptr;
    self->content[0] = '\0';
    self->capacity = START_STRING_SIZE;
    self->length = 0;
    return 0;
}


int stringdyn_deinit(struct stringdyn *self)
{
    free(self->content);
    return 0;
}


int stringdyn_concat_cstring(struct stringdyn *self, const char *str)
{
    size_t str_len = strlen(str);
    while (str_len + self->length > self->capacity) {
        stringdyn_expand_capacity(self);
    }
    strcat(self->content, str);
    self->length += str_len;
    return 0;
}
int stringdyn_insert_cstring(struct stringdyn *self, const char *str, size_t position)
{
    size_t str_len = strlen(str);
    if (position > self->length) {
        return -1;
    }
    if (str_len + self->length > self->capacity) {
        stringdyn_expand_capacity(self);
    }
    memmove(self->content + position + str_len,
            self->content + position,
            self->length - position + 1);  // one plus for the ending char
    memcpy(self->content + position,
           str,
           str_len);
    self->length += str_len;
    return 0;
}


int stringdyn_append_char(struct stringdyn *self, const char ch)
{
    if (1 + self->length > self->capacity) {
        stringdyn_expand_capacity(self);
    }
    self->content[self->length] = ch;
    self->content[self->length + 1] = '\0';
    return 0;
}

int stringdyn_insert_char(struct stringdyn *self, const char ch, size_t position)
{
    if (position > self->length) {
        return -1;
    }
    if (1 + self->length > self->capacity) {
        stringdyn_expand_capacity(self);
    }
    memmove(self->content + position +1,
            self->content + position,
            self->length - position + 1);  // one plus for the ending char
    self->content[position] = ch;
    self->length += 1;
    return 0;
}

int stringdyn_delete_char(struct stringdyn *self, size_t position)
{
    if (position > self->length) {
        return -1;
    }
    memmove(self->content + position - 1,
            self->content + position,
            self->length - position + 1);  // one plus for the ending char
    self->length--;
    return 0;
}


void stringdyn_reset(struct stringdyn *self)
{
    self->length = 0;
    self->content[0] = '\0';
}


struct stringdyn * stringdyn_new(const char * str)
{
    void *ptr = malloc(sizeof(struct stringdyn));
    if (ptr == NULL) {
        perror("MALLOC new stringdyn");
        return NULL;
    }
    struct stringdyn *self = (struct stringdyn *) ptr;

    int ret = stringdyn_init(self);
    if (ret != 0) {
        return NULL;
    }

    ret = stringdyn_concat_cstring(self, str);
    if (ret != 0) {
        return NULL;
    }

    return self;
}

const char * stringdyn_get_cstring(struct stringdyn *self)
{
    return (const char *)self->content;
}

int stringdyn_set_cstring(struct stringdyn *self, const char *str)
{
    size_t str_len = strlen(str);
    while (str_len > self->capacity) {
        stringdyn_expand_capacity(self);
    }
    strcpy(self->content, str);
    self->length = str_len;
    return 0;
}

size_t stringdyn_get_length(struct stringdyn *self)
{
    return (size_t)self->length;
}
