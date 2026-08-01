#include <stdio.h>
#include "najd.h"

NSR_BUILD_READER(number, isdigit)
NSR_BUILD_READER(space, isspace)
NSR_BUILD_READER(opcode, isupper)

NSR_CONDITION(comment){
    return najd_string_peek(str, 1) == '#';
}

NSR_READER(comment){
    NSR_READER_BEGIN()
    while(najd_string_peek(str, 1) != '\n' && najd_string_peek(str, 1) != '\0'){
        najd_string_step(str);
    }
    NSR_READER_END()
}

NSR_CONDITION(comma){
    return najd_string_peek(str, 1) == ',';
}

NSR_READER(comma){
    NSR_READER_BEGIN()
    NSR_READER_END()
}

NSR_CONDITION(reg){
    return najd_string_peek(str, 1) == 'R' && 
           najd_string_peek(str, 2) >= '0' && 
           najd_string_peek(str, 2) <= '3';
}

NSR_READER(reg){
    NSR_READER_BEGIN()
    najd_string_step(str);
    NSR_READER_END()
}

NSR_TABLE_BEGIN(zasm)
      NSR_ENTRY(1, comment)
      NSR_ENTRY(2, reg)
      NSR_ENTRY(3, opcode)
      NSR_ENTRY(4, number)
      NSR_ENTRY(5, comma)
      NSR_ENTRY(6, space)
NSR_TABLE_END (zasm)

int main(void){
    char *code = "# This is assembly code for zasm virtual machine\n\n"
                 "ADD 9, 7  \n"
                 "SUB 8, 4\n"
                 "ADD 5, 8\n"
                 "LDI R2, 77 \n"
                 "ADR R2, 23 \n";

    const char *colors[] = {
        "\x1b[0m",  
        "\x1b[31m", 
        "\x1b[35m", 
        "\x1b[32m", 
        "\x1b[33m", 
        "\x1b[34m", 
        "\x1b[0m"   
    };

    najd_string_t *str = najd_string_create(code, true);
    najd_string_reader_result_t* result = NULL;

    if(str){
        str->readers = NSR_TABLE_NAME(zasm);
        while(najd_string_nextable(str)){
            result = najd_string_read(str);
            if(result){
                printf("%s%d: %s -> %s @[%d:%d]\x1b[0m\n", 
                     colors[result->id], result->id, result->name, result->value,
                     result->line_number, result->position);
                najd_string_result_delete(result);
            }
            najd_buffer_reset(&str->buffer);
        }
        najd_string_delete(str);
    }
    
    return 0;
}
