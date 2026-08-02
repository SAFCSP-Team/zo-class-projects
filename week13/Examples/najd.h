#ifndef __NAJD_H__
#define __NAJD_H__

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

typedef struct najd_cursor_t najd_cursor_t;

struct najd_cursor_t{
    int size;
    int position;
    int line_number;
};

najd_cursor_t *najd_cursor_create(int size);
void najd_cursor_delete(najd_cursor_t *cursor);

#define najd_cursor_reset(c) \
    (c)->position = -1;      \
    (c)->line_number = 1;    \

#define najd_cursor_get_position(c)     ((c)->position)
#define najd_cursor_get_line_number(c)  ((c)->line_number)

#define najd_cursor_is_position(c, p) \
    (((p) >= 0) && ((p) < (c)->size))

#define najd_cursor_is_line_number(c, l) \
    (((l) >= 1) && ((l) <= (c)->line_number))

bool najd_cursor_step(najd_cursor_t *cursor);
bool najd_cursor_back(najd_cursor_t *cursor);
bool najd_cursor_move(najd_cursor_t *cursor, int position);

bool najd_cursor_enter(najd_cursor_t *cursor);
bool najd_cursor_denter(najd_cursor_t *cursor);

#define NAJD_BUFFER_SIZE    4096
typedef struct najd_buffer_t najd_buffer_t;

struct najd_buffer_t{
    char base[NAJD_BUFFER_SIZE];
    najd_cursor_t cursor;
};

najd_buffer_t *najd_buffer_create(void);
void najd_buffer_delete(najd_buffer_t *buffer);

bool najd_buffer_push(najd_buffer_t *buffer, char value);
bool najd_buffer_pop(najd_buffer_t *buffer, char *value);

#define najd_buffer_reset(b) \
    najd_cursor_reset(&(b)->cursor) \
    (b)->cursor.size = NAJD_BUFFER_SIZE; \
    memset((b)->base, 0, NAJD_BUFFER_SIZE);


typedef struct najd_string_t najd_string_t;


#define NSR_READER_BEGIN()                      \
    najd_string_reader_result_t *result = NULL; \
    najd_string_step(str);                      \
    int position = str->cursor.position;        \
    int line_number = str->line_number;         \
    char *value = NULL;

#define NSR_READER_END()                         \
    value = strdup(str->buffer.base);            \
    result = najd_string_result_create(          \
        id, name, value, position, line_number); \
                                                 \
    return result;

#define NSR_CONDITION_NAME(cn)  nsr_is_##cn
#define NSR_READER_NAME(rn) nsr_reader_##rn

#define NSR_CONDITION(cn) bool NSR_CONDITION_NAME(cn)(najd_string_t *str)
#define NSR_READER(rn) najd_string_reader_result_t*  NSR_READER_NAME(rn)(najd_string_t *str, int id, char *name)

#define NSR_READER_BODY_DEFAULT(rn)      \
    while(nsr_is_##rn(str)){             \
        najd_string_step(str);           \
    }

#define NSR_BUILD_READER(rn, cn)\
    NSR_CONDITION(rn){\
        return cn(najd_string_peek(str, 1));\
    } \
\
    NSR_READER(rn){\
        NSR_READER_BEGIN()\
        NSR_READER_BODY_DEFAULT(rn)\
        NSR_READER_END()\
}

#define NSR_TABLE_NAME(tn) nsr_reader_table_##tn

#define NSR_TABLE_BEGIN(tn) \
    najd_string_reader_t NSR_TABLE_NAME(tn) [] = {

#define NSR_TABLE_END(tn) \
    NSR_ENTRY_NULL() \
};

#define NSR_ENTRY(id, n)    {id, #n, NSR_CONDITION_NAME(n), NSR_READER_NAME(n)},
#define NSR_ENTRY_NULL()    {0, NULL, NULL, NULL},

typedef struct najd_string_reader_t najd_string_reader_t;
typedef struct najd_string_reader_result_t najd_string_reader_result_t;

typedef bool (*najd_string_reader_condition_t)(najd_string_t *str);
typedef najd_string_reader_result_t* (*najd_string_reader_body_t)(najd_string_t *str, int id, char *name);

struct najd_string_reader_t{
    int id;
    char *name;
    najd_string_reader_condition_t condition;
    najd_string_reader_body_t reader;
};

struct najd_string_reader_result_t{
    int id;
    char *name;
    char *value;

    int position;
    int line_number;
};

najd_string_reader_result_t *najd_string_result_create(int id, 
                                                char *name,
                                                char *value,
                                                int position,
                                                int line_number);
void najd_string_result_delete(najd_string_reader_result_t *result);


struct najd_string_t{
    najd_buffer_t buffer;
    najd_cursor_t cursor;

    char *string;
    int length;

    int line_number;
    char character;

    bool buffering;
    bool duplicated;

    najd_string_reader_t *readers;
};

najd_string_t *najd_string_create(char *string, bool duplicate);
void najd_string_delete(najd_string_t *str);
bool najd_string_step(najd_string_t *str);
bool najd_string_back(najd_string_t *str);
bool najd_string_jump(najd_string_t *str, int steps);
bool najd_string_put(najd_string_t *str, int postion, char value);
bool najd_string_nextable(najd_string_t *str);
char najd_string_peek(najd_string_t *str, int look);

void najd_string_set_buffering(najd_string_t *str, bool enable);
void najd_string_register_readers(najd_string_t *str, najd_string_reader_t* reader);

najd_string_reader_result_t *najd_string_read(najd_string_t *str);
#endif