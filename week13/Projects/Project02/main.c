#include <stdio.h>
#include "najd.h"

NSR_BUILD_READER(number, isdigit)
NSR_BUILD_READER(space, isspace)

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
NSR_TABLE_END (zasm)

int main(void){
    char *code = "int 10";
    najd_string_t *str = najd_string_create(code, true);
    najd_string_reader_result_t* result = NULL;

    if(str){
        str->readers = NSR_TABLE_NAME(zasm);
        // on_create
        while(najd_string_nextable(str)){
            // pre_read
            result = najd_string_read(str);
            // on_result(result)
            // post_read
            printf("%d: %s -> %s @[%d:%d]\n", result->id, result->name, result->value,
                 result->line_number, result->position);
            najd_string_result_delete(result);
            najd_buffer_reset(&str->buffer);
        }
        najd_string_delete(str);
        //on_delete
    }
    
    return 0;
}