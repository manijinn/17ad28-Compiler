#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "parser.h"
#include "unparser.h"
#include "scope_symtab.h"
#include "scope_check.h"

int main(int argc, const char * argv[]) 
{
    parser_open(argv[1]);
    AST * progast = parseProgram();
    parser_close();
    unparseProgram(stdout, progast);

    scope_initialize();
    scope_check_program(progast);

    return EXIT_SUCCESS;
}