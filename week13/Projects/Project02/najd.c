#include "najd.h"

najd_cursor_t *najd_cursor_create(int size){
    if(size <= 0) return NULL;

    najd_cursor_t * cursor = (najd_cursor_t*)malloc(sizeof(najd_cursor_t));
    if(!cursor) return NULL;

    cursor->size = size;
    cursor->line_number = 1;
    cursor->position = -1;

    return cursor;
}

void najd_cursor_delete(najd_cursor_t *cursor){
    if(cursor) free(cursor);
}

bool najd_cursor_step(najd_cursor_t *cursor){
    if(cursor){
        if(najd_cursor_is_position(cursor, cursor->position + 1)){
            cursor->position++;
            return true;
        }
    }

    return false;
}

bool najd_cursor_back(najd_cursor_t *cursor){
    if(cursor){
        if(cursor->position >= 0){
            cursor->position--;
            return true;
        }
    }

    return false;
}

bool najd_cursor_move(najd_cursor_t *cursor, int position){
    if(cursor){
        bool result = najd_cursor_is_position(cursor, position);
        if(result){
            cursor->position = position;
            return true;
        }
    }
    
    return false;
}

bool najd_cursor_enter(najd_cursor_t *cursor){
    if(cursor){
        cursor->line_number++;
        return true;
    }

    return false;
}

bool najd_cursor_denter(najd_cursor_t *cursor){
    if(cursor){
        assert(cursor->line_number >= 1);

        if((cursor->line_number - 1) >= 1){
            cursor->line_number--;
            return true;
        }
    }
    
    return false;
}

najd_buffer_t *najd_buffer_create(void){
    najd_buffer_t *buffer = (najd_buffer_t*)malloc(sizeof(najd_buffer_t));
    if(!buffer) return NULL;

    buffer->cursor.size = NAJD_BUFFER_SIZE;
    najd_buffer_reset(buffer)
    return buffer;
}

void najd_buffer_delete(najd_buffer_t *buffer){
    if(buffer) free(buffer);
}

bool najd_buffer_push(najd_buffer_t *buffer, char value){
    bool result = (buffer) && (najd_cursor_step(&buffer->cursor));
    if(result){
        buffer->base[najd_cursor_get_position(&buffer->cursor)] = value;
    }
    return result;
}

bool najd_buffer_pop(najd_buffer_t *buffer, char *value){
    if((!buffer) || (!value)) return false;
    
    int position = najd_cursor_get_position(&buffer->cursor);
    bool result = najd_cursor_is_position(&buffer->cursor, position);
    if(result){
        *value = buffer->base[position];
        buffer->base[position] = 0;
        result = najd_cursor_back(&buffer->cursor);
    }
    return result; 
}

najd_string_t *najd_string_create(char *string, bool duplicate){

    if(!string) return NULL;

    najd_string_t *str = (najd_string_t*)malloc(sizeof(najd_string_t));
    if(!str) return NULL;
    
    str->length = strlen(string);
    str->line_number = 1;
    str->character = '\0';
    str->duplicated = duplicate;

    if(duplicate){
        str->string = strdup(string);
    }else{
        str->string = string;
    }

    str->cursor.size = str->length;
    najd_cursor_reset(&str->cursor)

    najd_buffer_reset(&str->buffer)

    str->readers = NULL;
    return str;
}

void najd_string_delete(najd_string_t *str){
    if(str){
        if(str->duplicated){
            free(str->string);
        }
        free(str);
    }
}

bool najd_string_step(najd_string_t *str){
    if(!str || !str->string) return false;
    
    if(!najd_cursor_step(&str->cursor)){
        return false;
    }

    str->character = 
        str->string[najd_cursor_get_position(&str->cursor)];

    if(str->buffering){
        if(!najd_buffer_push(&str->buffer, str->character)){
            assert(najd_cursor_back(&str->cursor));
             str->character = 
                str->string[najd_cursor_get_position(&str->cursor)];
            return false;
        }
    }

    if(str->character == '\n'){
        str->line_number++;
    }

    return true;
}

bool najd_string_back(najd_string_t *str){
    if(!str || !str->string) return false;
    
    if(!najd_cursor_back(&str->cursor)){
        return false;
    }

    if(str->character == '\n'){
        str->line_number--;
    }

    if(str->cursor.position >= 0){
        str->character = 
            str->string[najd_cursor_get_position(&str->cursor)];
    }else{
        str->character = '\0';
    }

    if(str->buffering){
        char tmp;
        if(!najd_buffer_pop(&str->buffer, &tmp)){
            assert(najd_string_step(str));
            return false;
        }
    }

    return true;
}

bool najd_string_jump(najd_string_t *str, int steps){
    if(!str || steps == 0) return false;

    int position = najd_cursor_get_position(&(str->cursor));
    position += steps;
    bool result = najd_cursor_is_position(&(str->cursor), position);
    if(!result) return false;

    if(steps > 0){
        for(int i = 0; i < steps; i++){
            assert(najd_string_step(str));
        }
    }else{
        for(int i = 0; i < (steps * (-1)); i++){
            assert(najd_string_back(str));
        }
    }

    return true;
}

bool najd_string_put(najd_string_t *str, int position, char value){
    if(!str || !str->string) return false;

    if(najd_cursor_is_position((&str->cursor), position)){
        str->string[position] = value;
        return true;
    }

    return false;
}

bool najd_string_nextable(najd_string_t *str){
    return (str) && (str->cursor.position < (str->length - 1));
}

char najd_string_peek(najd_string_t *str, int look){
    if(!str) return '\0';

    int position = najd_cursor_get_position(&(str->cursor));
    position += look;

    bool result = najd_cursor_is_position(&(str->cursor), position);
    return result? str->string[position] : '\0';
}

void najd_string_set_buffering(najd_string_t *str, bool enable){
    if(str){
        str->buffering = enable;
    }
}

void najd_string_register_readers(najd_string_t *str, najd_string_reader_t* readers){
    if(str){
        str->readers = readers;
    }
}

najd_string_reader_result_t *
najd_string_result_create(int id, char *name,char *value,
                          int position,
                          int line_number){
    najd_string_reader_result_t *result = (najd_string_reader_result_t*)malloc(
        sizeof(najd_string_reader_result_t));
    
    result->id = id;
    result->name = name;
    result->value = value;
    result->position = position;
    result->line_number = line_number;

    return result;
}

void najd_string_result_delete(najd_string_reader_result_t *result){
    if(result){
        if(result->value) free(result->value);
        free(result);
    }
}

najd_string_reader_result_t *najd_string_read(najd_string_t *str){
    najd_string_reader_result_t *result = NULL;
    najd_string_reader_t *current = NULL;

    if(str && str->readers){
        current = str->readers;
        najd_string_set_buffering(str, true);
        while(current != NULL && current->reader != NULL){
            if(current->condition(str)){
                result = current->reader(str, current->id, current->name);
                break;
            }
            current++;
        }
    }

    return result;
}

void najd_string_process_with_handler(najd_string_t *str, najd_string_reader_t *readers, najd_string_handler_t *handler) {
    if (!str || !readers) return;

    str->readers = readers;

    if (handler && handler->on_create) {
        handler->on_create(str);
    }

    while (najd_string_nextable(str)) {
        if (handler && handler->pre_read) {
            handler->pre_read(str);
        }

        najd_string_reader_result_t *result = najd_string_read(str);

        if (handler && handler->on_result) {
            handler->on_result(str, result);
        }

        if (handler && handler->post_read) {
            handler->post_read(str);
        }

        if (result) {
            najd_string_result_delete(result);
        }
    }

    if (handler && handler->on_delete) {
        handler->on_delete(str);
    }
}