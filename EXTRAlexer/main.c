#include <stdio.h>
#include "lexer_output.h"
#include "lexer.h"
#include "token.h"
#include "utilities.h"

int main(int argc, char *argv[]) {

    lexer_open(argv[1]);
    lexer_output();
    lexer_close();
    
  return 0;
}