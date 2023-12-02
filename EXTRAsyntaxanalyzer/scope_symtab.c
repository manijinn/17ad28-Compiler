#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "scope_symtab.h"
#include "utilities.h"

// structs for symbol table
typedef struct {
    const char *id;
    id_attrs *attrs;
} symtab_assoc_t;

typedef struct scope_symtab_s {
    unsigned int size;
    symtab_assoc_t *entries[MAX_SCOPE_SIZE];
} scope_symtab_t;

// Creation of symbol table
static scope_symtab_t *symtab = NULL;

// Allocates memory for a fresh scope symbol table
static scope_symtab_t * scope_create(void)
{
    scope_symtab_t *new_scope = (scope_symtab_t *) malloc(sizeof(scope_symtab_t));
    // if no space then error message issued and exited
    if (new_scope == NULL) 
    {
	    bail_with_error("No space for new scope_symtab_t!");
    }
    new_scope->size = 0;
    for (int j; j < MAX_SCOPE_SIZE; j++) 
    {
	    new_scope->entries[j] = NULL;
    }
    return new_scope;
}

// initialize the symbol table
void scope_initialize(void)
{
    symtab = scope_create();
}

// Return the current scope's next offset to use for allocation
unsigned int scope_size(void)
{
    return symtab->size;
}

// Checks if scope is full
bool scope_full(void)
{
    return scope_size() >= MAX_SCOPE_SIZE;
}

// Adds new association to the table
static void scope_add(symtab_assoc_t *assoc)
{
    symtab->entries[symtab->size] = assoc;
    symtab->size++;
}

// Inserts an association from the given name to the given id_attrs attrs
void scope_insert(const char *name, id_attrs *attrs)
{
    symtab_assoc_t *new_assoc = malloc(sizeof(symtab_assoc_t));

    // if no space then error is issued and program exited
    if (new_assoc == NULL) 
    {
	     bail_with_error("No space for association!");
    }
    new_assoc->id = name;
    new_assoc->attrs = attrs;
    scope_add(new_assoc);
}

// Checks if name is associated with an attribute
bool scope_defined(const char *name)
{
    return scope_lookup(name) != NULL;
}

// Checks to see if there's an existing association based on the name
id_attrs *scope_lookup(const char *name)
{
    int i;

    for (i = 0; i < symtab->size; i++) 
    {
        if (strcmp(symtab->entries[i]->id, name) == 0) 
        {
            return symtab->entries[i]->attrs;
        }
    }
  
    return NULL;
}