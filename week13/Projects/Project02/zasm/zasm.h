#ifndef __ZASM_H__
#define __ZASM_H__

#include "../najd.h"

#define ZASM_ANSI_RESET "\x1b[0m"
#define ZASM_ANSI_GREY "\x1b[90m"
#define ZASM_ANSI_RED "\x1b[91m"
#define ZASM_ANSI_GREEN "\x1b[92m"
#define ZASM_ANSI_YELLOW "\x1b[93m"
#define ZASM_ANSI_BLUE "\x1b[94m"
#define ZASM_ANSI_MAGENTA "\x1b[95m"
#define ZASM_ANSI_CYAN "\x1b[96m"

enum zasm_token_id {
  ZASM_TOKEN_COMMENT,
  ZASM_TOKEN_DIRECTIVE,
  ZASM_TOKEN_NUMBER,
  ZASM_TOKEN_IDENTIFIER,
  ZASM_TOKEN_COMMA,
  ZASM_TOKEN_COLON,
  ZASM_TOKEN_LBRACKET,
  ZASM_TOKEN_RBRACKET,
  ZASM_TOKEN_RPARENTHESIS,
  ZASM_TOKEN_LPARENTHESIS,
  ZASM_TOKEN_NEWLINE,
  ZASM_TOKEN_SPACE,

  ZASM_TOKEN_MNEMONIC,
  ZASM_TOKEN_REGISTER,
  ZASM_TOKEN_LABEL
};

static const char *ZASM_MNEMONICS[] = {
    "ADD",  "SUB", "MUL", "DIV", "MOD",

    "LDI",  "LDM", "STR", "STI", "LDA", "STA", "ADR", "SUR",

    "PUSH", "POP",

    "INC",  "DEC",

    "CMP",  "JMP", "JE",  "JZ",  "JG",  "JGE", "JL",  "JLE", "JNE",

    "IN",   "OUT",

    "CALL", "RET", "STP"};

#define ZASM_MNEMONICS_SIZE sizeof(ZASM_MNEMONICS) / sizeof(ZASM_MNEMONICS[0])

#define ZASM_BUILD_READER_CHAR(rn, ch)                                         \
  NSR_CONDITION(rn) { return najd_string_peek(str, 1) == (ch); }               \
                                                                               \
  NSR_READER(rn) {                                                             \
    NSR_READER_BEGIN()                                                         \
    NSR_READER_END()                                                           \
  }

void zasm_classify(najd_string_t *str, najd_string_reader_result_t *result);
void zasm_print_token_val(najd_string_reader_result_t *result);
najd_string_t *zasm_lexer_create(char *code);

#endif