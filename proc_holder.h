#ifndef _PROC_HOLDER_H
#define _PROC_HOLDER_H
#include "machine_types.h"
#include "code.h"

extern void proc_holder_initialize();
extern address proc_holder_register(code_seq code);
extern code_seq proc_holder_code_for_all();
extern address proc_addr();

#endif