// Yasminie Sahadeo, Usmaan Baig, Group 70. COP 3402 00130 T&Th 12pm-1:15Pm, hw3.

#ifndef _SCOPE_CHECK_H
#define _SCOPE_CHECK_H

#include "ast.h"
#include "id_attrs.h"
#include "file_location.h"

// Builds symbol table for the AST and checks 
// for duplicate declarations undeclared identifiers
void scope_check_program(AST * prog);

// Checks constants, variables, and statements for 
// duplicate declarations undeclared identifiers
void scope_check_block(AST * block);

// Build the symbol table and check the declarations for constants
void scope_check_constDecls(AST * cds);

// Adds to table based on passed name, id_kind, and file location
void add_ident_to_scope(const char * name, id_kind k, file_location floc);

// Checks the declaration and adds it to the symbol table 
void scope_check_constDecl(AST * cd);

// Build the symbol table and check the declarations for variables
void scope_check_varDecls(AST * vds);

// Checks the declaration and adds it to the symbol table 
void scope_check_varDecl(AST * vd);

// Checks that all idenfifiers are declared
void scope_check_stmt(AST * stmt);

// Checks that all idenfifiers are declared
void scope_check_assignStmt(AST * stmt);

// Checks that all idenfifiers are declared
void scope_check_beginStmt(AST * stmt);

// Checks that all idenfifiers are declared
void scope_check_ifStmt(AST * stmt);

// Checks that all idenfifiers are declared
void scope_check_whileStmt(AST * stmt);

// Checks that all idenfifiers are declared
void scope_check_readStmt(AST * stmt);

// Checks that all idenfifiers are declared
void scope_check_writeStmt(AST * stmt);

// Checks that all idenfifiers are declared
void scope_check_cond(AST * cond);

// Checks that all idenfifiers are declared
void scope_check_odd_cond(AST * cond);

// Checks that all idenfifiers are declared
void scope_check_bin_cond(AST * cond);

// Checks that all idenfifiers are declared
void scope_check_expr(AST * exp);

// Checks that all idenfifiers are declared
void scope_check_ident(file_location floc, const char * name);

// Checks that all idenfifiers are declared
void scope_check_bin_expr(AST * exp);

#endif
