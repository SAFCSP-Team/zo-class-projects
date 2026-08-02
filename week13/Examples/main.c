#include <stdio.h>
#include "najd.h"

#ifdef _WIN32
#include <windows.h>
void enable_ansi_colors(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
}
#endif

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[0;91m"
#define COLOR_GREEN   "\033[0;92m"
#define COLOR_YELLOW  "\033[0;93m"
#define COLOR_BLUE    "\033[0;94m"

NSR_BUILD_READER(number, isdigit)
NSR_BUILD_READER(space, isspace)

NSR_CONDITION(comma){
    return najd_string_peek(str, 1) == ',';
}

NSR_READER(comma){
    NSR_READER_BEGIN()
    NSR_READER_END()
}

NSR_CONDITION(colon){
    return najd_string_peek(str, 1) == ':';
}

NSR_READER(colon){
    NSR_READER_BEGIN()
    NSR_READER_END()
}

NSR_CONDITION(bracket){
    char c = najd_string_peek(str, 1);
    return c == '[' || c == ']';
}

NSR_READER(bracket){
    NSR_READER_BEGIN()
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

NSR_CONDITION(register_name){
    char c1 = najd_string_peek(str, 1);
    char c2 = najd_string_peek(str, 2);
    return (c1 == 'R' || c1 == 'r') && isdigit(c2);
}

NSR_READER(register_name){
    NSR_READER_BEGIN()
        najd_string_step(str);
    NSR_READER_END()
}

NSR_CONDITION(opcode){
    char c1 = najd_string_peek(str, 1);
    char c2 = najd_string_peek(str, 2);
    char c3 = najd_string_peek(str, 3);
    char c4 = najd_string_peek(str, 4);

    if (isspace(c4) || c4 == '\0') {
        if ((c1=='A'||c1=='a') && (c2=='D'||c2=='d') && (c3=='D'||c3=='d')) return true;
        if ((c1=='S'||c1=='s') && (c2=='U'||c2=='u') && (c3=='B'||c3=='b')) return true;
        if ((c1=='D'||c1=='d') && (c2=='I'||c2=='i') && (c3=='V'||c3=='v')) return true;
        if ((c1=='M'||c1=='m') && (c2=='U'||c2=='u') && (c3=='L'||c3=='l')) return true;
        if ((c1=='M'||c1=='m') && (c2=='O'||c2=='o') && (c3=='V'||c3=='v')) return true;
        if ((c1=='J'||c1=='j') && (c2=='M'||c2=='m') && (c3=='P'||c3=='p')) return true;
    }
    return false;
}

NSR_READER(opcode){
    NSR_READER_BEGIN()
        najd_string_step(str);
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
      NSR_ENTRY(4, comma)
      NSR_ENTRY(5, register_name)
      NSR_ENTRY(6, opcode)
      NSR_ENTRY(7, comment)
      NSR_ENTRY(9, colon)
      NSR_ENTRY(10, bracket)
      NSR_ENTRY(2, identifier)
      NSR_ENTRY(3, space)
NSR_TABLE_END (zasm)

void on_create(najd_string_t *str) {
    printf(COLOR_RED "[INIT] Lexer started. Total input size: %d bytes." COLOR_RESET "\n", str->length);
}

void pre_read(najd_string_t *str) {
}

void on_result(najd_string_reader_result_t *result) {
    if (result->id != 3) {
        const char *color = COLOR_RESET;

        switch (result->id) {
            case 1:  color = COLOR_GREEN;  break; /* numbers */
            case 7:  color = COLOR_GREEN;  break; /* comments */
            case 2:  color = COLOR_YELLOW; break; /* identifiers / labels */
            case 6:  color = COLOR_YELLOW; break; /* opcodes */
            case 4:  color = COLOR_BLUE;   break; /* comma */
            case 5:  color = COLOR_BLUE;   break; /* registers */
            case 9:  color = COLOR_RED;    break; /* colon */
            case 10: color = COLOR_RED;    break; /* brackets [ and ] */
            default: color = COLOR_RESET;  break;
        }

        printf("%s%d: %s -> %s @[%d:%d]" COLOR_RESET "\n", 
               color, result->id, result->name, result->value,
               result->line_number, result->position);
    }
}

void post_read(najd_string_t *str, najd_string_reader_result_t *result) {
}

void on_delete(void) {
    printf(COLOR_RED "[CLEANUP] Lexer destroyed." COLOR_RESET "\n");
}

int main(void){
#ifdef _WIN32
    enable_ansi_colors();
#endif

    char *code = 
        "Add two numbers\n"
        ".code\n"
        "main:  LDI R0, 10\n"
        "       LDI R1, 20\n"
        "       ADD R0, R1, R2\n"
        "       SUB R0, R1, R2\n"
        "       RET\n";

    najd_string_t *str = najd_string_create(code, true);
    najd_string_reader_result_t* result = NULL;

    if(str){
        najd_string_register_readers(str, NSR_TABLE_NAME(zasm));
        
        on_create(str);

        while(najd_string_nextable(str)){
            pre_read(str);

            result = najd_string_read(str);

            if (result != NULL) {
                on_result(result);
                post_read(str, result);
                najd_string_result_delete(result);
            } else {
                najd_string_step(str);
            }

            najd_buffer_reset(&str->buffer);
        }

        najd_string_delete(str);
        
        on_delete();
    }
    
    return 0;
}