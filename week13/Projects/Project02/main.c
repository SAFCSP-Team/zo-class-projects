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

NSR_CONDITION(comment){
    return najd_string_peek(str, 1) == ';';
}

NSR_READER(comment){
    NSR_READER_BEGIN()
    char _value = najd_string_peek(str, 1);
    while(_value != '\n' && _value != '\0'){
        najd_string_step(str);
        _value = najd_string_peek(str, 1);
    }
    NSR_READER_END()
}

NSR_CONDITION(string){
    return najd_string_peek(str, 1) == '"';
}

NSR_READER(string){
    NSR_READER_BEGIN()
    char _value = najd_string_peek(str, 1);
    while(_value != '"' && _value != '\0'){
        najd_string_step(str);
        _value = najd_string_peek(str, 1);
    }
    najd_string_step(str);
    NSR_READER_END()
}

NSR_CONDITION(punct){
    char value = najd_string_peek(str, 1);
    return value == ',' || value == ':' || value == '+' ||
           value == '-' || value == '[' || value == ']';
}

NSR_READER(punct){
    NSR_READER_BEGIN()
    NSR_READER_END()
}

NSR_CONDITION(reg){
    char c1 = najd_string_peek(str, 1);
    char c2 = najd_string_peek(str, 2);
    return (c1 == 'R' || c1 == 'r') && isdigit((unsigned char)c2);
}

NSR_READER(reg){
    NSR_READER_BEGIN()
    char _value = najd_string_peek(str, 1);
    while(isdigit((unsigned char)_value)){
        najd_string_step(str);
        _value = najd_string_peek(str, 1);
    }
    NSR_READER_END()
}

NSR_CONDITION(other){
    return true;
}

NSR_READER(other){
    NSR_READER_BEGIN()
    NSR_READER_END()
}

NSR_TABLE_BEGIN(zasm)
NSR_ENTRY(1, number)
NSR_ENTRY(8, reg)
NSR_ENTRY(2, identifier)
NSR_ENTRY(3, space)
NSR_ENTRY(4, comment)
NSR_ENTRY(5, string)
NSR_ENTRY(6, punct)
NSR_ENTRY(7, other)
NSR_TABLE_END(zasm)

#define COLOR_RESET      "\x1b[0m"
#define COLOR_NUMBER     "\x1b[34m"
#define COLOR_REGISTER   "\x1b[35m"
#define COLOR_MNEMONIC   "\x1b[1;34m"
#define COLOR_LABEL      "\x1b[1;32m"
#define COLOR_IDENT      "\x1b[33m"
#define COLOR_COMMENT    "\x1b[90m"
#define COLOR_STRING     "\x1b[32m"
#define COLOR_PUNCT      "\x1b[37m"

static const char *mnemonics[] = {
    "ADD", "SUB", "JMP", "INC", "DEC", "LOD", "AND", "MUL",
    NULL
};

static bool is_mnemonic(const char *value){
    for(int i = 0; mnemonics[i] != NULL; i++){
        if(strcasecmp(value, mnemonics[i]) == 0) return true;
    }
    return false;
}

static const char *identifier_color(najd_string_reader_result_t **tokens, int count, int i){
    if(is_mnemonic(tokens[i]->value)) return COLOR_MNEMONIC;

    for(int j = i + 1; j < count; j++){
        if(tokens[j]->id == 3) continue;
        if(tokens[j]->id == 6 && strcmp(tokens[j]->value, ":") == 0) return COLOR_LABEL;
        break;
    }
    return COLOR_IDENT;
}

static const char *token_color(najd_string_reader_result_t **tokens, int count, int i){
    switch(tokens[i]->id){
        case 1: return COLOR_NUMBER;
        case 2: return identifier_color(tokens, count, i);
        case 3: return "";
        case 4: return COLOR_COMMENT;
        case 5: return COLOR_STRING;
        case 6: return COLOR_PUNCT;
        case 8: return COLOR_REGISTER;
        default: return COLOR_RESET;
    }
}

int main(void){
    char *code =
        "start:\n"
        "    LOD R1, 5        : R1 = 5\n"
        "    LOD R2, 3        : R2 = 3\n"
        "    ADD R1, R2       : R1 = 5 + 3 = 8\n"
        "    SUB R1, R2       : R1 = 8 - 3 = 5\n"
        "    MUL R1, R2       : R1 = 5 * 3 = 15\n"
        "    AND R1, R2       : R1 = 15 & 3 = 3\n"
        "    INC R1           : R1 = 3 + 1 = 4\n"
        "    DEC R2           : R2 = 3 - 1 = 2\n"
        "    JMP start\n";

    najd_string_t *str = najd_string_create(code, true);
    if(!str) return 1;

    str->readers = NSR_TABLE_NAME(zasm);

    int capacity = 64, count = 0;
    najd_string_reader_result_t **tokens =
        malloc(sizeof(najd_string_reader_result_t*) * capacity);

    while(najd_string_nextable(str)){
        najd_string_reader_result_t *result = najd_string_read(str);
        if(!result) break;

        if(count == capacity){
            capacity *= 2;
            tokens = realloc(tokens, sizeof(najd_string_reader_result_t*) * capacity);
        }
        tokens[count++] = result;
        najd_buffer_reset(&str->buffer);
    }

    for(int i = 0; i < count; i++){
        printf("%s%s%s", token_color(tokens, count, i), tokens[i]->value, COLOR_RESET);
        najd_string_result_delete(tokens[i]);
    }
    printf("\n");

    free(tokens);
    najd_string_delete(str);
    return 0;
}