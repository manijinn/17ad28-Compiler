#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scope_check.h"
#include "id_attrs.h"
#include "file_location.h"
#include "ast.h"
#include "utilities.h"
#include "scope_symtab.h"
#include "scope_check.h"

// Builds symbol table for the AST and checks 
// for duplicate declarations undeclared identifiers
void scope_check_program(AST * prog)
{
    scope_check_block(prog);
}

// Checks constants, variables, and statements for 
// duplicate declarations undeclared identifiers
void scope_check_block(AST * block)
{
    
    scope_check_constDecls(block->data.program.cds);
    scope_check_varDecls(block->data.program.vds);
    scope_check_stmt(block->data.program.stmt);
    
}

// Build the symbol table and check the declarations for constants
void scope_check_constDecls(AST * cds)
{
    while(cds != NULL)
    {
        scope_check_constDecl(cds);
        cds = cds->next;
    }
}

// Adds to table based on passed name, id_kind, and file location
void add_ident_to_scope(const char * name, id_kind k, file_location floc)
{
    id_attrs * attrs = scope_lookup(name);
    if(attrs != NULL)
    {
        // If a value already exists an error is printed
        general_error(floc, "%s \"%s\" is already declared as a %s", kind2str(k), name, kind2str(attrs->kind));
    }
    else
    {
        scope_insert(name, create_id_attrs(floc, k, scope_size()));
    }
}

// Checks the declaration and adds it to the symbol table 
void scope_check_constDecl(AST * cd)
{
    add_ident_to_scope(cd->data.const_decl.name, constant, cd->file_loc);
}

// Build the symbol table and check the declarations for variables
void scope_check_varDecls(AST * vds)
{
    while(vds != NULL)
    {
        scope_check_varDecl(vds);
        vds = vds->next;
    }
}

// Checks the declaration and adds it to the symbol table 
void scope_check_varDecl(AST * vd)
{
    add_ident_to_scope(vd->data.var_decl.name, variable, vd->file_loc);
}

// Checks unique statements/keywords for correct syntax.
void scope_check_stmt(AST * stmt)
{
    // If not matched then error printed 
    switch(stmt->type_tag)
    {
      case assign_ast: 
        scope_check_assignStmt(stmt);
        break;
      case begin_ast:
        scope_check_beginStmt(stmt);
        break;
      case if_ast:
        scope_check_ifStmt(stmt);
        break;
      case while_ast:
        scope_check_whileStmt(stmt);
        break;
      case read_ast:
        scope_check_readStmt(stmt);
        break;
      case write_ast:
        scope_check_writeStmt(stmt);
        break;
      case skip_ast:
        break;
      default:  
        bail_with_error("Call to scope_check_stmt with an AST that is not a statement!");
        break;
        
    }
}

// Checks assignStmt.
void scope_check_assignStmt(AST * stmt)
{
    scope_check_ident(stmt->file_loc, stmt->data.assign_stmt.name);
    scope_check_expr(stmt->data.assign_stmt.exp);
}

// Checks beginStmt.
void scope_check_beginStmt(AST * stmt)
{
    AST * stmts = stmt->data.begin_stmt.stmts;
    while(stmts != NULL)
    {
        scope_check_stmt(stmts);
        stmts = stmts->next;
    }
}

// Checks ifStmt. Checks if condition and then/else statements.
void scope_check_ifStmt(AST * stmt)
{
    scope_check_cond(stmt->data.if_stmt.cond);
    scope_check_stmt(stmt->data.if_stmt.thenstmt);
    scope_check_stmt(stmt->data.if_stmt.elsestmt);
}

// Checks whileStmt. Checks while conditiuon and statements.
void scope_check_whileStmt(AST * stmt)
{
    scope_check_cond(stmt->data.while_stmt.cond);
    scope_check_stmt(stmt->data.while_stmt.stmt);
}

// Checks readStmt. Checks subsequent identifier for syntax correctness.
void scope_check_readStmt(AST * stmt)
{
    scope_check_ident(stmt->file_loc, stmt->data.read_stmt.name);
}

// Checks writeStmt. Checks subsequent expression.
void scope_check_writeStmt(AST * stmt)
{
    scope_check_expr(stmt->data.write_stmt.exp);
}

// Checks conditionals. There are two cases, a singular condition or a condition that relies upon comparison between two expressions.
void scope_check_cond(AST * cond)
{
    // If not matched then error printed 
    switch(cond->type_tag)
    {
      case odd_cond_ast:
        scope_check_odd_cond(cond);
        break;
      case bin_cond_ast:
        scope_check_bin_cond(cond);
        break;
      default:
        bail_with_error("Unexpected type_tag (%d) in scope_check_cond (for line %d, column %d)!", cond->type_tag, cond->file_loc.line, cond->file_loc.column);
        break;
    }
}

// Checks odd conditonal statements for correctness. 
void scope_check_odd_cond(AST * cond)
{
    scope_check_expr(cond->data.odd_cond.exp);
}

// Checks that binary conditions (comparison condtional that compares two different expressions) for correctness.
void scope_check_bin_cond(AST * cond)
{
    scope_check_expr(cond->data.bin_cond.leftexp);
    scope_check_expr(cond->data.bin_cond.rightexp);
}

// Checks to see if expressions are syntactically correct. The function will look at an expression's identifiers, binary parenthetical expressions, and numbers.
void scope_check_expr(AST * exp)
{
    // If not matched then error printed 
    switch(exp->type_tag)
    {
      case ident_ast:  
        scope_check_ident(exp->file_loc, exp->data.ident.name);
        break;
      case bin_expr_ast:
        scope_check_bin_expr(exp);
        break;
      case number_ast:
        break;
      default:
        bail_with_error("Unexpected type_tag (%d) in scope_check_expr (for line %d, column %d)!", exp->type_tag, exp->file_loc.line, exp->file_loc.column);
        break;
    }
}

// Checks that all idenfifiers are declared/correct.
void scope_check_ident(file_location floc, const char * name)
{
    // If not defined then error message is printed
    if(!scope_defined(name))
    {
        general_error(floc, "identifer \"%s\" is not declared!", name);
    }
}


// Checks to see that binary parenthetical expressions (in other word, expressions with two additional expressions) are correct.
void scope_check_bin_expr(AST * exp)
{
    scope_check_expr(exp->data.bin_expr.leftexp);
    scope_check_expr(exp->data.bin_expr.rightexp);
}