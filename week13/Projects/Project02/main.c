#include <stdio.h>
#include "najd.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_NUMBER  "\033[31m"
#define COLOR_MNEMONIC "\033[1;36m"
#define COLOR_DIM     "\033[2m"  

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
NSR_TABLE_END(zasm)


void on_create(void){
    printf(COLOR_DIM "; zasm listing\n" COLOR_RESET);
}

void pre_read(najd_string_t *str){
    (void)str; 
}

void on_result(najd_string_reader_result_t *result){
    switch(result->id){
        case 1: 
            printf(COLOR_NUMBER "%s" COLOR_RESET, result->value);
            break;
        case 2:
            printf(COLOR_MNEMONIC "%s" COLOR_RESET, result->value);
            break;
        case 3: 
            printf("%s", result->value);
            break;
        default:
            printf("%s", result->value);
            break;
    }
}

void post_read(najd_string_reader_result_t *result){
    (void)result;
}

void on_delete(void){
    printf("\n" COLOR_DIM "; end of listing" COLOR_RESET "\n");
}

int main(void){
    char *code = "int r0 10";
    najd_string_t *str = najd_string_create(code, true);
    najd_string_reader_result_t* result = NULL;

    if(str){
        str->readers = NSR_TABLE_NAME(zasm);

        on_create();
        while(najd_string_nextable(str)){
            pre_read(str);
            result = najd_string_read(str);
            on_result(result);
            post_read(result);
            najd_string_result_delete(result);
            najd_buffer_reset(&str->buffer);
        }
        najd_string_delete(str);
        on_delete();
    }

    return 0;
}