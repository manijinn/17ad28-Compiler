/* $Id: gen_code_stubs.c,v 1.7 2023/03/27 16:39:12 leavens Exp $ */
#include "gen_code.h"
#include "code.h"
#include "proc_holder.h"
#include "utilities.h"
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct proc_s proc;
// code sequences
typedef proc proc_seq;

// machine code instructions (that can be in linked lists)
typedef struct proc_s {
  label *lab;
  char name[50];
} proc;

code_seq test;
code_seq *head;

proc pc[MAX];
int i = 0;



// Initialize the code generator
void gen_code_initialize() {
  // why does this make the program break
  proc_holder_initialize();
}

// Generate code for the given AST
code_seq gen_code_program(AST *prog) {

  // printf("reached gen code prog\n\n\n");
  // Mana's code
  code_seq proc_code = code_seq_singleton(code_inc(LINKS_SIZE));

  //gen_code_procDecls(prog);
  // printf("about to enter gen code block\n\n\n");
  proc_code = code_seq_concat(proc_code, gen_code_block(prog));
  proc_code = code_seq_add_to_end(proc_code, code_hlt());

  // address proc_address = proc_holder_register(proc_code);

  // printf("\n%d\n", proc_code->instr.op);

  code_seq ret = proc_holder_code_for_all();
  ret = code_seq_concat(ret, proc_code);

  return ret;
}

// generate code for blk
code_seq gen_code_block(AST *blk) {

  code_seq ret = NULL; 

   gen_code_procDecls(blk->data.program.pds);
  ret = code_seq_concat(ret, gen_code_constDecls(blk->data.program.cds));
  ret = code_seq_concat(ret, gen_code_varDecls(blk->data.program.vds));


  ret = code_seq_concat(ret, gen_code_stmt(blk->data.program.stmt));



  return ret;
}

// generate code for the declarations in cds
code_seq gen_code_constDecls(AST_list cds) {

  code_seq ret = code_seq_empty();

  while (!ast_list_is_empty(cds)) {
    ret = code_seq_concat(ret, gen_code_constDecl(ast_list_first(cds)));
    cds = ast_list_rest(cds);
  }

  return ret;
}

// generate code for the const declaration cd
code_seq gen_code_constDecl(AST *cd) {


  code_seq ret = code_lit(cd->data.const_decl.num_val);
  return ret;
}

// generate code for the declarations in vds
code_seq gen_code_varDecls(AST_list vds) {


  code_seq ret = code_seq_empty();

  while (!ast_list_is_empty(vds)) {
    ret = code_seq_concat(ret, gen_code_varDecl(ast_list_first(vds)));
    vds = ast_list_rest(vds);
  }

  return ret;
}

// generate code for the var declaration vd
code_seq gen_code_varDecl(AST *vd) {

  return code_seq_singleton(code_inc(1));

}

// generate code for the declarations in pds
void gen_code_procDecls(AST_list pds) {


  while (!ast_list_is_empty(pds)) {
    gen_code_procDecl(ast_list_first(pds));
    pds = ast_list_rest(pds);
  }
}

// generate code for the procedure declaration pd
void gen_code_procDecl(AST *pd) {

  if (i == 0) {
    label *l = label_create();
    label_set(l, 1);
    pc[i].lab = l;
    strcpy(pc[i].name, pd->data.proc_decl.name);
  }

  code_seq ret = gen_code_block(pd->data.proc_decl.block);

  int val = ast_list_size(pd->data.proc_decl.block->data.program.cds);
  val += ast_list_size(pd->data.proc_decl.block->data.program.vds);

  if (val > 0) {
    ret = code_seq_add_to_end(ret, code_inc(-val));
  }

  ret = code_seq_add_to_end(ret, code_rtn());

  test = ret;

  code_seq_fix_labels(ret);

  ret->lab = label_create();

  int ad = proc_holder_register(ret);

  label_set(ret->lab, ad);

  test->lab = ret->lab;
  pc[i].lab = test->lab;
  strcpy(pc[i].name, pd->data.proc_decl.name);

  i++;

  test = test->next;

}

// generate code for the statement
code_seq gen_code_stmt(AST *stmt) {

  switch (stmt->type_tag) {
  case call_ast:
    return gen_code_callStmt(stmt);
    break;
  case begin_ast:
    return gen_code_beginStmt(stmt);
    break;
  case if_ast:
    return gen_code_ifStmt(stmt);
    break;
  case while_ast:
    return gen_code_whileStmt(stmt);
    break;
  case read_ast:
    return gen_code_readStmt(stmt);
    break;
  case write_ast:
    return gen_code_writeStmt(stmt);
    break;
  case skip_ast:
    return gen_code_skipStmt(stmt);
    break;
  case assign_ast:
    return gen_code_assignStmt(stmt);
    break;
  default:
    bail_with_error("gen_code_stmt not implemented yet!");
    return code_seq_empty();
    break;
  }
}

// generate code for the statement
code_seq gen_code_assignStmt(AST *stmt) {

  unsigned int outLevels =
      stmt->data.assign_stmt.ident->data.ident.idu->levelsOutward;

  code_seq ret = code_compute_fp(outLevels);
  ret = code_seq_concat(ret, gen_code_expr(stmt->data.assign_stmt.exp));

  unsigned int offset =
      stmt->data.assign_stmt.ident->data.ident.idu->attrs->loc_offset;
  ret = code_seq_add_to_end(ret, code_sto(offset));

  return ret;
}

// generate code for the statement
code_seq gen_code_callStmt(AST *stmt) {

  int j;
   for (j = 0; j < 100; j++) {

    if (strcmp(stmt->data.call_stmt.ident->data.ident.name, pc[j].name) == 0) {
      break;
    }
  }

  code_seq ret = code_cal(pc[j].lab);


  return ret;

}

// generate code for the statement
code_seq gen_code_beginStmt(AST *stmt) {

  AST_list stmts = stmt->data.begin_stmt.stmts;
  code_seq ret = code_seq_empty();

  while (!ast_list_is_empty(stmts)) {
    ret = code_seq_concat(ret, gen_code_stmt(ast_list_first(stmts)));
    stmts = ast_list_rest(stmts);
  }

  return ret;
}

// generate code for the statement
code_seq gen_code_ifStmt(AST *stmt) {

  code_seq ret = gen_code_cond(stmt->data.if_stmt.cond);
  ret = code_seq_add_to_end(ret, code_jpc(2));

  code_seq thenc = gen_code_stmt(stmt->data.if_stmt.thenstmt);
  int then_size = code_seq_size(thenc);

  code_seq elsec = gen_code_stmt(stmt->data.if_stmt.elsestmt);
  int else_size = code_seq_size(elsec);

  ret = code_seq_add_to_end(ret, code_jmp(then_size + 2));
  ret = code_seq_concat(ret, thenc);
  ret = code_seq_add_to_end(ret, code_jmp(else_size + 1));
  ret = code_seq_concat(ret, elsec);
  return ret;
}

// generate code for the statement
code_seq gen_code_whileStmt(AST *stmt) {

  code_seq ret = gen_code_cond(stmt->data.while_stmt.cond);
  int cond_size = code_seq_size(ret);
  ret = code_seq_add_to_end(ret, code_jpc(2));

  code_seq whilec = gen_code_stmt(stmt->data.while_stmt.stmt);
  int while_size = code_seq_size(whilec);

  ret = code_seq_add_to_end(ret, code_jmp(while_size + 2));
  ret = code_seq_concat(ret, whilec);

  ret = code_seq_add_to_end(ret, code_jmp(-(while_size + cond_size + 2)));

  return ret;

}

// generate code for the statement
code_seq gen_code_readStmt(AST *stmt) {
  code_seq ret = gen_code_expr(stmt->data.read_stmt.ident);
  ret = code_seq_add_to_end(ret, code_chi());
  return ret;
}

// generate code for the statement
code_seq gen_code_writeStmt(AST *stmt) {

  code_seq ret = gen_code_expr(stmt->data.write_stmt.exp);
  ret = code_seq_add_to_end(ret, code_cho());

  return ret;
}

// generate code for the statement
code_seq gen_code_skipStmt(AST *stmt) {

  code_seq ret = code_nop();
  return ret;
}

// generate code for the condition
code_seq gen_code_cond(AST *cond) {

  code_seq ret;

  switch (cond->type_tag) {
  case odd_cond_ast:
    return ret = gen_code_odd_cond(cond);
    break;

  case bin_cond_ast:
    return ret = gen_code_bin_cond(cond);
    break;

  default:
    bail_with_error("gen_code_cond not implemented yet!");
    return code_seq_empty();
  }
}

// generate code for the condition
code_seq gen_code_odd_cond(AST *cond) {

  code_seq ret = gen_code_expr(cond->data.odd_cond.exp);
  ret = code_seq_add_to_end(ret, code_lit(2));
  ret = code_seq_add_to_end(ret, code_mod());
  return ret;
}

// generate code for the condition
code_seq gen_code_bin_cond(AST *cond) {

  code_seq ret = gen_code_expr(cond->data.bin_cond.leftexp);

  ret = code_seq_concat(ret, gen_code_expr(cond->data.bin_cond.rightexp));

  switch (cond->data.bin_cond.relop) {

  case gtop:
    ret = code_seq_concat(ret, code_gtr());
    break;

  case geqop:
    ret = code_seq_concat(ret, code_geq());
    break;

  case leqop:
    ret = code_seq_concat(ret, code_leq());
    break;

  case ltop:
    ret = code_seq_concat(ret, code_lss());
    break;

  case neqop:
    ret = code_seq_concat(ret, code_neq());
    break;

  case eqop:
    ret = code_seq_concat(ret, code_eql());
    break;

  default:
    ret = code_seq_empty();
  }

  return ret;
}

// generate code for the expresion
code_seq gen_code_expr(AST *exp) {


  switch (exp->type_tag) {

  case number_ast:
    return gen_code_number_expr(exp);
    // break;
  case ident_ast:
    return gen_code_ident_expr(exp);

  case bin_expr_ast:
    return gen_code_bin_expr(exp);
  default:
    bail_with_error("gen_code_expr other cases not done yet!");
    return code_seq_empty();
  }
}

// generate code for the expression (exp)
code_seq gen_code_bin_expr(AST *exp) {

  code_seq ret = gen_code_expr(exp->data.bin_expr.leftexp);
  ret = code_seq_concat(ret, gen_code_expr(exp->data.bin_expr.rightexp));

  switch (exp->data.bin_expr.arith_op) {
  case addop:
    ret = code_seq_concat(ret, code_add());
    break;
  case subop:
    ret = code_seq_concat(ret, code_sub());
    break;
  case divop:
    ret = code_seq_concat(ret, code_div());
    break;
  case multop:
    ret = code_seq_concat(ret, code_mul());
    break;
  default:
    bail_with_error("gen_code_bin_expr other cases not done yet!");
    return code_seq_empty();
  }

  return ret;
}

// generate code for the ident expression (ident)
code_seq gen_code_ident_expr(AST *ident) {

  unsigned int offset = ident->data.ident.idu->attrs->loc_offset;

  code_seq ret = code_seq_singleton(code_pbp());

  int x = ident->data.ident.idu->levelsOutward;
  for (int j = 0; j < x; j++) {
    ret = code_seq_concat(ret, code_psi());
  }
  ret = code_seq_concat(ret, code_lod(offset));


  return ret;
}

// generate code for the number expression (num)
code_seq gen_code_number_expr(AST *num) {

  return code_seq_singleton(code_lit(num->data.number.value));
}
