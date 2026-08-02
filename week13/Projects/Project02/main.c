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

NSR_CONDITION(comma){
    char value = najd_string_peek(str, 1);
    return value == ',';
}

NSR_READER(comma){
    NSR_READER_BEGIN()
        char _value = najd_string_peek(str, 1);
        
        while(_value == ','){
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
NSR_TABLE_END (zasm)

int main(void){
    char *code = "ADD R1, 10 \nSUB 23, 10 \n";
    najd_string_t *str = najd_string_create(code, true);
    najd_string_reader_result_t* result = NULL;

    if(str){
        str->readers = NSR_TABLE_NAME(zasm);
        // on_create
        while(najd_string_nextable(str)){
            // pre_read
            result = najd_string_read(str);
            // on_result(result)
            switch(result->id){
                case 1: /* number */
                    printf("\033[37m%s\033[0m", result->value);
                    break;
                case 2: /* identifier */
                    {
                        int is_mnemonic = 0;
                        size_t count = sizeof(najd_zasm_mnemonics) / sizeof(najd_zasm_mnemonics[0]);
                        for(size_t i = 0; i < count; i++){
                            if(strcmp(result->value, najd_zasm_mnemonics[i]) == 0){
                                is_mnemonic = 1;
                                break;
                            }
                        }
                        if(is_mnemonic)
                            printf("\033[31m%s\033[0m", result->value);
                        else
                            printf("\033[38;5;208m%s\033[0m", result->value);
                    }
                    break;
                case 3: /* space */
                    printf("%s", result->value);
                    break;
                case 4: /* comma */
                    printf("\033[34m%s\033[0m", result->value);
                    break;
                default:
                    printf("%s", result->value);
                    break;
            }
            // post_read
            najd_string_result_delete(result);
            najd_buffer_reset(&str->buffer);
        }
        najd_string_delete(str);
        //on_delete
    }
    
    return 0;
}