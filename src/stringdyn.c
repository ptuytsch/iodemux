#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


#define START_STRING_SIZE 64


struct inputstring{
    char *content;
    size_t length;
    size_t capacity;
};

//===========================================

int inputstring_expand_capacity(struct inputstring *self)
{
    size_t new_size = self->capacity * 2;
    void *ptr = malloc(new_size);
    if (ptr == NULL) {
        perror("MALLOC inputstring, expand");
        return -1;
    }
    memcpy(ptr, self->content, self->capacity);
    free(self->content);
    self->content = ptr;
    self->capacity = new_size;
    return 0;
}


void inputstring_print_internals(struct inputstring *self)
{
    printf("inputstring @ %p\n", self);
    printf("content: %s\n", self->content);
    printf("length: %lu\n", self->length);
    printf("capacity: %lu\n", self->capacity);
}

//===========================================

int inputstring_init(struct inputstring *self)
{
    void *ptr = malloc(START_STRING_SIZE);
    if (ptr == NULL) {
        perror("MALLOC inputstring");
        return -1;
    }
    self->content = ptr;
    self->content[0] = '\0';
    self->capacity = START_STRING_SIZE;
    self->length = 0;
    return 0;
}


int inputstring_deinit(struct inputstring *self)
{
    free(self->content);
    return 0;
}


int inputstring_concat_cstring(struct inputstring *self, const char *str)
{
    size_t str_len = strlen(str);
    while (str_len + self->length > self->capacity) {
        inputstring_expand_capacity(self);
    }
    strcat(self->content, str);
    self->length += str_len;
    return 0;
}
int inputstring_insert_cstring(struct inputstring *self, const char *str, size_t position)
{
    size_t str_len = strlen(str);
    if (position > self->length) {
        return -1;
    }
    if (str_len + self->length > self->capacity) {
        inputstring_expand_capacity(self);
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


int inputstring_append_char(struct inputstring *self, const char ch)
{
    if (1 + self->length > self->capacity) {
        inputstring_expand_capacity(self);
    }
    self->content[self->length] = ch;
    self->content[self->length + 1] = '\0';
    return 0;
}

int inputstring_insert_char(struct inputstring *self, const char ch, size_t position)
{
    if (position > self->length) {
        return -1;
    }
    if (1 + self->length > self->capacity) {
        inputstring_expand_capacity(self);
    }
    memmove(self->content + position +1,
            self->content + position,
            self->length - position + 1);  // one plus for the ending char
    self->content[position] = ch;
    self->length += 1;
    return 0;
}

int inputstring_delete_char(struct inputstring *self, size_t position)
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


void inputstring_reset(struct inputstring *self)
{
    self->length = 0;
    self->content[0] = '\0';
}


struct inputstring * inputstring_new(const char * str)
{
    void *ptr = malloc(sizeof(struct inputstring));
    if (ptr == NULL) {
        perror("MALLOC new inputstring");
        return NULL;
    }
    struct inputstring *self = (struct inputstring *) ptr;

    int ret = inputstring_init(self);
    if (ret != 0) {
        return NULL;
    }

    ret = inputstring_concat_cstring(self, str);
    if (ret != 0) {
        return NULL;
    }

    return self;
}

const char * inputstring_get_cstring(struct inputstring *self)
{
    return (const char *)self->content;
}

size_t inputstring_get_length(struct inputstring *self)
{
    return (size_t)self->length;
}
