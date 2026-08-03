#include <stdio.h>
#include "najd.h"

// ANSI Color Macros
#define COLOR_RESET   "\033[0m"
#define COLOR_NUMBER  "\033[1;33m" // Yellow (Numbers)
#define COLOR_IDENT   "\033[1;36m" // Cyan (Identifiers / Instructions)
#define COLOR_SPACE   "\033[0m"    // Default (Whitespace)
#define COLOR_COMMA   "\033[1;31m" // Red (Comma ,)

NSR_BUILD_READER(number, isdigit)
NSR_BUILD_READER(space, isspace)

NSR_CONDITION(comma){
    return najd_string_peek(str, 1) == ',';
}

NSR_READER(comma){
    NSR_READER_BEGIN()
        najd_string_step(str); 
    NSR_READER_END()
}

NSR_CONDITION(identifier){
    char value = najd_string_peek(str, 1);
    return isalpha(value) || value == '_';
}

NSR_READER(identifier){
    NSR_READER_BEGIN()
        char _value = najd_string_peek(str, 1);
        
        while(isalnum(_value) || _value == '_'){
            najd_string_step(str);
            _value = najd_string_peek(str, 1);
        }
    NSR_READER_END()
}

NSR_TABLE_BEGIN(zasm)
      NSR_ENTRY(1, number)
      NSR_ENTRY(2, identifier)
      NSR_ENTRY(3, space)
      NSR_ENTRY(4, comma)
NSR_TABLE_END(zasm)


void on_create(najd_string_t *str) {
}

void pre_read(najd_string_t *str) {
}

void on_result(najd_string_t *str, najd_string_reader_result_t *result) {
    if (!result || !result->value) return;

    char *color = COLOR_RESET;
    switch(result->id) {
        case 1: color = COLOR_NUMBER; break; 
        case 2: color = COLOR_IDENT;  break; 
        case 3: color = COLOR_SPACE;  break; 
        case 4: color = COLOR_COMMA;  break; 
    }

    printf("%s%s%s", color, result->value, COLOR_RESET);
}

void post_read(najd_string_t *str) {
    najd_buffer_reset(&str->buffer);
}

void on_delete(najd_string_t *str) {
    printf("\n");
}

int main(void){
    char *code = "int 10\nADD R0, R1\nMOV R2, 7";
    najd_string_t *str = najd_string_create(code, true);

    if(str){
        najd_string_handler_t handler = {
            .on_create = on_create,
            .pre_read  = pre_read,
            .on_result = on_result,
            .post_read = post_read,
            .on_delete = on_delete
        };

        najd_string_process_with_handler(str, NSR_TABLE_NAME(zasm), &handler);

        najd_string_delete(str);
    }
    
    return 0;
}