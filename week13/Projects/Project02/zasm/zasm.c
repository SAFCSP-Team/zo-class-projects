#include "./zasm.h"
#include <ctype.h>

NSR_CONDITION(comment){
    return (najd_string_peek(str, 1) == '#');
}

NSR_READER(comment){
    NSR_READER_BEGIN()
    char c = najd_string_peek(str, 1);
    while((c != '\n') && (c != '\0')){
        najd_string_step(str);
        c = najd_string_peek(str, 1);
    }
    NSR_READER_END()
}
 
 

NSR_CONDITION(directive){
    return najd_string_peek(str, 1) == '.';
}
 
NSR_READER(directive){
    NSR_READER_BEGIN()
 
    char c = najd_string_peek(str, 1);
    while(isalnum(c) || (c == '_')){
        najd_string_step(str);
        c = najd_string_peek(str, 1);
    }
 
    NSR_READER_END()
}
 
 


NSR_BUILD_READER(number, isdigit)
 
 
NSR_CONDITION(identifier){
    char c = najd_string_peek(str, 1);
    return isalpha(c) || (c == '_');
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



ZASM_BUILD_READER_CHAR(comma,    ',')
ZASM_BUILD_READER_CHAR(colon,    ':')
ZASM_BUILD_READER_CHAR(lbracket, '[')
ZASM_BUILD_READER_CHAR(rbracket, ']')
ZASM_BUILD_READER_CHAR(lparenthesis, '(')
ZASM_BUILD_READER_CHAR(rparenthesis, ')')
 

NSR_CONDITION(newline){
    return najd_string_peek(str, 1) == '\n';
}
 
NSR_READER(newline){
    NSR_READER_BEGIN()
    NSR_READER_BODY_DEFAULT(newline)
    NSR_READER_END()
}
 
 
NSR_CONDITION(space){
    char c = najd_string_peek(str, 1);
    return c == '\t' || c == ' ';
}
 
NSR_READER(space){
    NSR_READER_BEGIN()
    NSR_READER_BODY_DEFAULT(space)
    NSR_READER_END()
}
 



NSR_TABLE_BEGIN(zasm)
    NSR_ENTRY(ZASM_TOKEN_COMMENT,    comment)
    NSR_ENTRY(ZASM_TOKEN_DIRECTIVE,  directive)
    NSR_ENTRY(ZASM_TOKEN_NUMBER,     number)
    NSR_ENTRY(ZASM_TOKEN_IDENTIFIER, identifier)
    NSR_ENTRY(ZASM_TOKEN_COMMA,      comma)
    NSR_ENTRY(ZASM_TOKEN_COLON,      colon)
    NSR_ENTRY(ZASM_TOKEN_LBRACKET,   lbracket)
    NSR_ENTRY(ZASM_TOKEN_RBRACKET,   rbracket)
    NSR_ENTRY(ZASM_TOKEN_NEWLINE,    newline)
    NSR_ENTRY(ZASM_TOKEN_SPACE,      space)
NSR_TABLE_END(zasm)






void zasm_classify(najd_string_t *str, najd_string_reader_result_t *result) {
  if (!result || (result->id != ZASM_TOKEN_IDENTIFIER))
    return;

  if (najd_string_peek(str, 1) == ':') {
    result->id = ZASM_TOKEN_LABEL;
    result->name = "label";
    return;
  }

  for (int i = 0; i < ZASM_MNEMONICS_SIZE; i++) {
        
    if (strcmp(ZASM_MNEMONICS[i], result->value) == 0) {
      result->id = ZASM_TOKEN_MNEMONIC;
      result->name = "mnemonic";
      return;
    }
  }

  if (tolower(result->value[0]) == 'r') {
    int i = 1;
    while (isdigit(result->value[i]))
      i++;

    if (i > 1) {
      result->id = ZASM_TOKEN_REGISTER;
      result->name = "register";
      return;
    }
  }
}

void zasm_print_token_val(najd_string_reader_result_t *result) {
  const char *color = "";

  if (!result)
    return;

  switch (result->id) {
  case ZASM_TOKEN_COMMENT:
    color = ZASM_ANSI_GREY;
    break;
  case ZASM_TOKEN_DIRECTIVE:
    color = ZASM_ANSI_GREEN;
    break;
  case ZASM_TOKEN_NUMBER:
    color = ZASM_ANSI_RED;
    break;
  case ZASM_TOKEN_LABEL:
    color = ZASM_ANSI_YELLOW;
    break;
  case ZASM_TOKEN_MNEMONIC:
    color = ZASM_ANSI_MAGENTA;
    break;
  case ZASM_TOKEN_REGISTER:
    color = ZASM_ANSI_CYAN;
    break;
  case ZASM_TOKEN_COMMA:
    color = ZASM_ANSI_BLUE;
    break;
    
  default:
    break;
  }

  printf("%s%s%s", color, result->value, ZASM_ANSI_RESET);
}

najd_string_t *zasm_lexer_create(char *code) {
  najd_string_t *str = najd_string_create(code, true);
  if (!str)
    return NULL;

  najd_string_register_readers(str, NSR_TABLE_NAME(zasm));
  return str;
}