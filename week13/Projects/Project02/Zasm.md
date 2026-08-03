# Najd Lexer (Event-Driven Callbacks)

New feature updates for the **Najd Lexer** framework that introduce event callbacks for seamless token handling and pipeline customization.

---

## What's New

- **`pre_token` Callback:** Executed automatically right *before* reading the next token.
- **`on_token` Callback:** Executed automatically right *after* a token is successfully extracted, passing the `result` directly to your handler (ideal for AST parsing or colored logging).
- **`najd_string_set_callbacks`:** Inline helper function to register your callbacks directly into `najd_string_t`.

---

## Usage Example

### 1. Define Your Token Handler
```c
// Callback triggered every time a token is produced
void my_on_token(najd_string_reader_result_t *result) {
    if (!result) return;

    if (result->id == 1) { // Number
        printf("\033[1;33m[NUMBER]\033[0m %s\n", result->value);
    } else if (result->id == 2) { // Identifier
        printf("\033[1;36m[IDENTIFIER]\033[0m %s\n", result->value);
    }
}
