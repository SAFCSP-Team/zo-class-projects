
#include "./zasm/zasm.h"


char input_str[] = "# Add two numbers\n"
                   ".code\n"
                   "main:  LDI R0, 10\n"
                   "       LDI R1, 20\n"
                   "       ADD R0, R1, R2\n"
                   "       RET\n";

int main(void) {
  najd_string_t *str = zasm_lexer_create(input_str);

  if (str) {
    while (najd_string_nextable(str)) {
      
      najd_string_reader_result_t *result = najd_string_read(str);
      zasm_classify(str, result);
      zasm_print_token_val(result);

      najd_string_result_delete(result);
      najd_buffer_reset(&str->buffer);
    }

    najd_string_delete(str);
  }

  return 0;
}