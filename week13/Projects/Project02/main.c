#include <stdio.h>
#include "najd.h"

NSR_BUILD_READER(number, isdigit)
NSR_BUILD_READER(space, isspace)

NSR_CONDITION(identifier){
    char value = najd_string_peek(str, 1);
    return isalpha(value) || value == '_';

    
}

NSR_CONDITION(comma){
    return najd_string_peek(str, 1) == ',';
}
NSR_READER(comma){
    NSR_READER_BEGIN()
        najd_string_step(str);
    NSR_READER_END()
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
      NSR_ENTRY(4, comma)
      NSR_ENTRY(3, space)
NSR_TABLE_END (zasm)

#define COLOR_RESET       "\x1b[0m"
#define COLOR_NUMBER      "\x1b[33m"  
#define COLOR_REGISTER    "\x1b[35m"  
#define COLOR_INSTRUCTION "\x1b[36m"  
#define COLOR_COMMA       "\x1b[37m"  
#define COLOR_SPACE       "\x1b[90m" 

const char *token_color(int id){
    switch(id){
        case 1: return COLOR_NUMBER;
        case 2: return COLOR_REGISTER;
        case 3: return COLOR_SPACE;
        case 4: return COLOR_INSTRUCTION;
        case 5: return COLOR_COMMA;
        default: return COLOR_RESET;
    }
}

int main(void){
    char *code =
        "ADD 9, 7\n"
        "SUB 8, 4\n"
        "ADD 5, 8\n"
        "LDI R2, 77\n"
        "ADR R2, 23\n"
        "SUR R2, 5\n"
        "MUL 3, 6\n"
        "DIV 9, 3\n"
        "MOD 7, 2\n"
        "LDI R0, 20\n"
        "LDI R1, 20\n"
        "JMP 45\n"
        "LDI R0, 70\n"
        "INC R0\n"
        "DEC R0\n"
        "CMP R0, R1\n"
        "JE 36\n"
        "STI 0, 17\n"
        "LDM R3, 0\n"
        "PUSH 22\n"
        "PUSH 77\n"
        "POP R2\n"
        "POP R3\n"
        "STP 0\n";
    najd_string_t *str = najd_string_create(code, true);
    najd_string_reader_result_t* result = NULL;

    if(str){
        str->readers = NSR_TABLE_NAME(zasm);
        // on_create
        while(najd_string_nextable(str)){
            // pre_read
            result = najd_string_read(str);
            if(!result)break;
            // on_result(result)
            // post_read
printf("%s%s%s", token_color(result->id), result->value, COLOR_RESET);

            najd_string_result_delete(result);
            najd_buffer_reset(&str->buffer);
        }
        najd_string_delete(str);
    }
    return 0;
}