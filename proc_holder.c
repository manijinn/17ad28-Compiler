#include "proc_holder.h"
#include "machine_types.h"

static address next_addr = 1;

static code_seq proc_code;

void proc_holder_initialize()
{
	proc_code = code_seq_empty();
}

address proc_holder_register(code_seq pcode)
{
	unsigned int plen = code_seq_size(pcode);
	proc_code = code_seq_concat(proc_code, pcode);
	address start_addr = next_addr;
	next_addr += plen;
	return start_addr;
}

code_seq proc_holder_code_for_all()
{
	if(!code_seq_is_empty(proc_code)) {
	code_seq_fix_labels(proc_code);
	unsigned int total_length = code_seq_size(proc_code);
	address start_of_main = 1 + total_length;
	code_seq initial_jump = code_seq_singleton(code_jmp(start_of_main));
	return code_seq_concat(initial_jump, proc_code);
	} else {
    // printf("\ntest proc code %d\n", proc_code->instr.op);
    // printf("test");
	return code_seq_empty();
	}
}

address proc_addr()
{
	return proc_code->lab->addr;
}