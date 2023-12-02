// Yasminie Sahadeo, Usmaan Baig, Group 70. COP 3402 00130 T&Th 12pm-1:15Pm, hw3.

// This header file defines the externally-visible entry points to the parser
#ifndef _PARSER_H
#define _PARSER_H
#include "ast.h"

// initialize the parser to work on the given file
extern void parser_open(const char *filename);

// finish using the parser
extern void parser_close(void);

void advance(void);

void eat(token_type t);

AST * parseProgram(void);

AST * parseBlock(void);

AST_list parseConstDecls(void);

AST * parseConstDecl(void);

AST * parseConstDef(void);

AST_list parseVarDecls(void);

AST * parseVarDecl(void);

AST_list parseIdents(void);

void add_AST_to_end(AST_list *head, AST_list *last, AST_list lst);

AST * parseStmt(void);

AST *parseAssignStmt(void);

AST_list parseBeginStmt(void);

AST * parseSemiStmt(void);

AST * parseIfStmt(void);

AST * parseWhileStmt(void);

AST * parseCondition(void);

AST * parseReadStmt(void);

AST *parseWriteStmt(void);

AST *parseSkipStmt(void);

AST * parseCondition(void);

AST * parseOddCond(void);

AST * parseBinRelCond(void);

rel_op parseRelOp(void);

extern AST *parseExpr(void);

AST * parseAddSubTerm(void);

AST * parseTerm(void);

AST * parseMultDivFactor(void);

AST *parseFactor(void);

AST * parseParenExpr(void);

AST * parseSignedNumber(void);

// <program> ::= <var-decls> <stmt>
//extern AST *parseProgram();

// <stmt> ::= <ident> = <expr> ; | ...
//extern AST *parseStmt();

// Return an AST with the operators (if any) associating to the left
// <expr> ::= <lterm> <rel-op-lterm>
// <rel-op-lterm> ::= <empty> | <rel=op-expr>

#endif