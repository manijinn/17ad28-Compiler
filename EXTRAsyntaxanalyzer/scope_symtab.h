// Yasminie Sahadeo, Usmaan Baig, Group 70. COP 3402 00130 T&Th 12pm-1:15Pm, hw3.

#ifndef _SCOPE_SYMTAB_H
#define _SCOPE_SYMTAB_H

#include <stdbool.h>
#include "token.h"
#include "ast.h"
#include "id_attrs.h"

// Maximum number of declarations that can be stored in a scope
#define MAX_SCOPE_SIZE 4096

// initialize the symbol table for the current scope
extern void scope_initialize(void);

// Return the current scope's next offset to use for allocation
extern unsigned int scope_size(void);

// Checks if scope is full
extern bool scope_full(void);

// Checks if name is associated with an attribute
extern bool scope_defined(const char *name);

// Inserts an association from the given name to the given id_attrs attrs
extern void scope_insert(const char *name, id_attrs *attrs);

// Checks to see if there's an existing association based on the name
extern id_attrs *scope_lookup(const char *name);

#endif