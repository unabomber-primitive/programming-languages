/* main.c */

#include "ast.h"
#include "ring.h"
#include "tokenizer.h"

void ast_print(struct AST ast) { print_ast(stdout, &ast); }
void token_print(struct token token) { printf("%s(%" PRId64 ")", TOKENS_STR[token.type], token.value); }

DECLARE_RING(ast, struct AST)
DEFINE_RING(ast, struct AST)
DEFINE_RING_PRINT(ast, ast_print)
DEFINE_RING(token, struct token)
DEFINE_RING_PRINT(token, token_print)

#define RETURN_ERROR(code, msg) return printf(msg), code

struct AST *build_ast(char *str)
{
  struct ring_token *tokens = NULL;
  if ((tokens = tokenize(str)) == NULL)
    RETURN_ERROR(NULL, "Tokenization error.\n");

  ring_token_print(tokens);

  ring_token_free(&tokens);

  return NULL;
}


int main()
{
  char *str = "1 + 2 * (2 - -3) + 8";

  struct AST *ast = build_ast(str);

  if (ast == NULL)
    printf("AST build error.\n");
  else
  {
    print_ast(stdout, ast);
    printf("\n\n%s = %" PRId64 "\n", str, calc_ast(ast));
  }

  return 0;
}
