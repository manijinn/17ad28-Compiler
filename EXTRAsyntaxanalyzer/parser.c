#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include "utilities.h"
#include "file_location.h"
#include "id_attrs.h"

const char *filename;
token tok;
unsigned int scope_offset;

// Open the lexer which opens the input file and returns us our first token.
void parser_open(const char *fname)
{
    lexer_open(fname);
    filename = fname;
    tok = lexer_next();
    scope_offset = 0;
}

// Close lexer.
void parser_close(void)
{
    lexer_close();
}

// Unless the lexer is finished, we get the next token from the lexer.
void advance(void)
{
    if (!lexer_done()) 
    {
    	tok = lexer_next(); 
    }
}

// If there is no type error, call advance() for next token, otherwise report error for parsing.
void eat(token_type t) 
{
    if (tok.typ == t) 
    {
        advance();
    } 
    else 
    {
      	token_type expected[1] = {t};
      	parse_error_unexpected(expected, 1, tok);
    }
}

// Parse the input file.
AST * parseProgram(void)
{
    AST * ast = parseBlock();
    eat(periodsym); 
    eat(eofsym);
    return ast;
} 

// Get all Const declarations first, then variable declarations, then statements. This parses the program.
AST * parseBlock(void)
{
    AST_list cds = parseConstDecls();
    AST_list vds = parseVarDecls();
    AST * stmt = parseStmt();

    file_location floc;

    if(!ast_list_is_empty(cds))
    {
        // Consts are at the top of programs, so we get Consts first. Otherwise something is not right and we report error.
        if(ast_list_first(cds)->type_tag == const_decl_ast)
        {
            floc = ast_list_first(cds)->file_loc;
        }
        else
        {
            bail_with_error("Bad AST for const declarations");
        }
    }
    else if(!ast_list_is_empty(vds))
    {
        // We get var declarations, and we make sure the
        if(ast_list_first(vds)->type_tag == var_decl_ast)
        {
            floc = ast_list_first(vds)->file_loc;
        }
        else
        {
            bail_with_error("Bad AST for var declarations");
        }
    }
    else
    {
        floc = stmt->file_loc;
    }

    return ast_program(floc.filename, floc.line, floc.column, cds, vds, stmt);
}

// const-decls

// Parse constant declarations.
AST_list parseConstDecls(void)
{
    // Create nodes for ASTs. They will be empty initially.
    AST_list ret = ast_list_empty_list();
    AST_list last = ast_list_empty_list();

    // While any encountered input is a constsym, we parse them. Once we receive the AST back we add it to the end of the AST.
    while(tok.typ == constsym)
    {
        AST * cdast = parseConstDecl();
    	  add_AST_to_end(&ret, &last, ast_list_singleton(cdast));
    }

    return ret;
}

// The actual parsing procedure.
AST * parseConstDecl(void)
{
    // Store the constant token into a temporary token.
    token cktok = tok;
    
    // Advance to the next symbol.
    eat(constsym);
    
    // We read the first const and create a new const ast with it. It will serve as the head in the Const AST list.
    AST * cd = parseConstDef();
    
    // Update file location of constant.
    cd->file_loc = token2file_loc(cktok);

    AST * ret = cd;
    AST * last = cd;

    // Now we go through the rest of the line to parse the rest of the constants (if any), creating new ASTs with them and adding them to the linked list.
    while(tok.typ == commasym)
    {
        eat(commasym);
        AST * next_cd = parseConstDef();
        add_AST_to_end(&ret, &last, ast_list_singleton(next_cd));
    }
    
    // End of line, so we eat the semicolon symbol.
    eat(semisym);

    return ret;
}

// We create an AST with a constant identifier.
AST * parseConstDef(void)
{
    // Store identifier token info into ident.
    token ident = tok;
    
    // Advance through equation and eat the rest of the symbols (if any).
    eat(identsym);
    eat(eqsym);
    short int numval = tok.value;
    eat(numbersym);
    
    // Create an AST with the constant's information.
    AST * ret = ast_const_def(ident, ident.text, numval);
  
    return ret;
}

// end of const-decls

// var-decls

// Parse variable declarations.
AST_list parseVarDecls(void)
{
    AST_list ret = ast_list_empty_list();
    AST_list last = ast_list_empty_list();

    while(tok.typ == varsym)
    {
        AST_list vdasts = parseVarDecl();
        
        add_AST_to_end(&ret, &last, vdasts);
    }

    return ret;
}

// Parse a variable declaration.
AST_list parseVarDecl(void)
{
    token vktok = tok;
    eat(varsym);
    AST_list vds = parseIdents();     

    // We check if the varaible declaration consists of only one AST in the list. If so, we overwrite its file_location with vktok's information.
    if (vds->next != NULL) {

        vds->file_loc = token2file_loc(vktok);
    }

    eat(semisym);
  
    return vds;
}

// Parse identities.
AST_list parseIdents(void)
{
    token idtok = tok; 
    eat(identsym);
    
    // Create an AST for the identity and return it as a AST_List/node in the list. Store the first identity AST node into ret.
    AST_list ret = ast_list_singleton(ast_var_decl(idtok, idtok.text));
    AST_list last = ret;

    // Go through the rest of the identities and add them to the list.
    while (tok.typ == commasym)
    {
        eat(commasym);
        token idtok = tok;
        eat(identsym);
        AST *vd = ast_var_decl(idtok, idtok.text);
        add_AST_to_end(&ret, &last, ast_list_singleton(vd));
    }
  
    return ret;
}

// end of var-decls

// Add the ast to the end.
void add_AST_to_end(AST_list *head, AST_list *last, AST_list lst)
{
    // Add the AST as head if the AST list is empty for that particular type.
    // Otherwise insert to the end of the AST linked list.
    if (ast_list_is_empty(*head)) 
    {
      	*head = lst;
      	*last = ast_list_last_elem(lst);
    } 
    else 
    {
      	ast_list_splice(*last, lst);
      	*last = ast_list_last_elem(lst);
    }
}

// Parse statements.
AST * parseStmt(void)
{
    // We have 7 cases of unique statements in PL0.
    AST * ret = NULL;
    token_type expected[7] = {identsym, beginsym, ifsym, whilesym, readsym, writesym, skipsym};

  switch(tok.typ)
  {
      case identsym:
        ret = parseAssignStmt();
        break;
      case beginsym:
        ret = parseBeginStmt();
        break;
      case ifsym:
        ret = parseIfStmt();
        break;
      case whilesym:
        ret = parseWhileStmt();
        break;
      case readsym:
      	ret = parseReadStmt();
      	break;
      case writesym:
      	ret = parseWriteStmt();
      	break;
      case skipsym:
      	ret = parseSkipStmt();
      	break;
      default:
        parse_error_unexpected(expected, 7, tok);
  }

    return ret;
}

// Parse assignment expression. An identity is equal to an expression
AST *parseAssignStmt(void)
{
    token idtok = tok;
    eat(identsym);
    eat(becomessym);
    AST *exp = parseExpr();

    return ast_assign_stmt(idtok, idtok.text, exp);
}

// Parse Begin statement.
AST_list parseBeginStmt(void)
{
    token btok = tok;
    eat(beginsym);

    // After eating the begin statement, presumably there will be more unique statments after. So we call parseStmt() again, and return the statements as AST_List.
    AST_list stmts = ast_list_singleton(parseStmt());
    AST_list last = stmts;

    while(tok.typ == semisym) 
    {
      	AST *stmt = parseSemiStmt();
      	add_AST_to_end(&stmts, &last, ast_list_singleton(stmt));
    }
  
    eat(endsym);
    AST *ret = ast_begin_stmt(btok, stmts);
    return ret;
}

// Parse semi colons.
AST * parseSemiStmt(void)
{
    eat(semisym);
    return parseStmt();
}

// Parse if statements. We get the if symbol, the condition, and the subsequent statements.
AST * parseIfStmt(void)
{
    token iftok = tok;
    eat(ifsym);
    AST * cond = parseCondition();
    eat(thensym);
    AST * t1 = parseStmt();
    eat(elsesym);
    AST * t2 = parseStmt();

    return ast_if_stmt(iftok, cond, t1, t2);
}

// Parse while statements. Get the while symbol, the condition, the do symbol, and the subsequent statements.
AST * parseWhileStmt(void)
{
    token wtok = tok;
    eat(whilesym);
    AST * cond = parseCondition();
    eat(dosym);
    AST * stmt = parseStmt();

    return ast_while_stmt(wtok, cond, stmt);
}

// Parse read statements. We get the identity being read.
AST *parseReadStmt(void)
{
    token rt = tok;
    eat(readsym);
    const char *name = tok.text;
    eat(identsym);
    return ast_read_stmt(rt, name);
}

// Parse write statements. We get the expression being written.
AST *parseWriteStmt(void)
{
    token wt = tok;
    eat(writesym);
    AST * exp = parseExpr();
    return ast_write_stmt(wt, exp);
}

// Parse skip statements.
AST * parseSkipStmt(void)
{
    token st = tok;
    eat(skipsym);
    return ast_skip_stmt(st);
}

// Parse conditions
AST * parseCondition(void)
{
    AST * ret;
    switch(tok.typ)
    {
      case oddsym:
        ret = parseOddCond();
        break;
      default:  
        ret = parseBinRelCond();
        break;
    }

    return ret;
}

// Parse odd conditions
AST * parseOddCond(void)
{
    token ot = tok;
    eat(oddsym);
    AST * exp = parseExpr();
    return ast_odd_cond(ot, exp);
}

// Parse binary conditions (comparison condition).
AST * parseBinRelCond(void)
{
    token fst = tok;

    // The first element.
    AST * e1 = parseExpr();
    rel_op r = parseRelOp();
    
    // The second element.
    AST * e2 = parseExpr();

    // Create AST with expression ASTs and fst token.
    AST * ret = ast_bin_cond(tok, e1, r, e2);
    ret->file_loc = token2file_loc(fst);
  
    return ret;
}

// Parse relationship operators
rel_op parseRelOp(void)
{
    token rp = tok;
    token_type expected[6] = {eqsym, neqsym, lessym, leqsym, gtrsym, geqsym};

    switch(tok.typ)
    {
      case eqsym:
        eat(eqsym);
        return eqop;
        break;
      case neqsym:
        eat(neqsym);
        return neqop;
        break;  
      case lessym:
        eat(lessym);
        return ltop;
        break;
      case leqsym:
        eat(leqsym);
        return leqop;
        break;
      case gtrsym:
        eat(gtrsym);
        return gtop;
        break;
      case geqsym:
        eat(geqsym);
        return geqop;
        break;
      default:
        parse_error_unexpected(expected, 6, rp);
        break;
    }  
    return (rel_op) 0;
}

// Parse expression. We get the first term of the expression and any add/sub operation symbols after the term.
AST *parseExpr(void)
{
    // Have to account for an expression thats only just one term.
    token fst = tok;
    AST * trm = parseTerm();
    AST * exp = trm;

    while(tok.typ == plussym || tok.typ == minussym)
    {
        AST * right = parseAddSubTerm();
        exp = ast_bin_expr(fst, exp, right->data.op_expr.arith_op, right->data.op_expr.exp);
    }
    return exp;
}

// Parse add/subtraction of terms. The gets the next term of the expression being parsed, after an add/sub operation symbol.
AST * parseAddSubTerm(void)
{
    token opt = tok; 
    token_type expected[2] = {plussym, minussym};
  
    switch(tok.typ)
    {
      case plussym:
        eat(plussym);
        AST * exp = parseTerm();
        return ast_op_expr(opt, addop, exp);
        break;
      case minussym:
        eat(minussym);
        AST * e = parseTerm();
        return ast_op_expr(opt, subop, e);
        break;
      default:
        parse_error_unexpected(expected, 2, tok);
        break;
    }

    return (AST *) NULL;
}

// Parse term. We get the factor (identity, parenthesis expression, or number) and find any Multiplication or Division symbols after it.
AST * parseTerm(void)
{
    token fst = tok;
    AST * fac = parseFactor();
    AST * exp = fac;
    while(tok.typ == multsym || tok.typ == divsym)
    {
        AST * right = parseMultDivFactor();
        exp = ast_bin_expr(fst, exp, right->data.op_expr.arith_op, right->data.op_expr.exp);
    }
    return exp;
}

// Parse multiplication/division of terms. This parses the next term after the multi/div symbol in the expression.
AST * parseMultDivFactor(void)
{
    token opt = tok;
    token_type expected[2] = {multsym, divsym};

    switch(tok.typ)
    {
      case multsym:
        eat(multsym);
        AST * exp = parseFactor();
        return ast_op_expr(opt, multop, exp);
        break;
      case divsym:
        eat(divsym);
        AST * e = parseFactor();
        return ast_op_expr(opt, divop, e);
        break;
      default:
        parse_error_unexpected(expected, 2, tok);
        break;
    }
  
    return (AST *) NULL;
}

// Parse factors, which should be an identity, parenthesis expression, or a signed number.
AST *parseFactor(void)
{
  token idt;
  token_type expected[5] = {identsym, lparensym, plussym, minussym, numbersym};
  
    switch(tok.typ) 
    {
        case identsym:
        	idt = tok;
          eat(identsym);
          return ast_ident(idt, idt.text);
        	break;
        case lparensym:
        	return parseParenExpr();
        	break;
        case numbersym: case plussym: case minussym:
        	return parseSignedNumber();
        	break;
        default:
        	parse_error_unexpected(expected, 5, tok);
        	break;
    }
    // The following should never execute
    return (AST *) NULL;
}

// Parse parenthetical expression.
AST * parseParenExpr(void)
{
    token lpt = tok;
    eat(lparensym);
    AST * ret = parseExpr();
    eat(rparensym);
    ret->file_loc = token2file_loc(lpt);

    return ret;
}

// Parse signed number.
AST * parseSignedNumber(void)
{
    bool negate = false;
    token fst = tok;
  
    switch(tok.typ)
    {
      case plussym:
        eat(plussym);
        break;
      case minussym:
        eat(minussym);
        negate = true;
        break;
      default: 
        break;
    }

    token numt = tok;  
    eat(numbersym);
    short int val = numt.value;
    if(negate)
    {
        val = -val;
    }
    return ast_number(fst, val);
}