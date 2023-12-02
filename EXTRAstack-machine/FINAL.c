
// libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// variable assignments for basic instructions
enum BasicInstruct {
  LIT = 1,
  RTN = 2,
  CAL = 3,
  POP = 4,
  PSI = 5,
  PRM = 6,
  STO = 7,
  INC = 8,
  JMP = 9,
  JPC = 10,
  CHO = 11,
  CHI = 12,
  HLT = 13,
  NDB = 14
};

// variable assignments for arithmetic/logical instructions
enum ArithLogInstruct {
  NEG = 15,
  ADD = 16,
  SUB = 17,
  MUL = 18,
  DIV = 19,
  MOD = 20,
  EQL = 21,
  NEQ = 22,
  LSS = 23,
  LEQ = 24,
  GTR = 25,
  GEQ = 26,
  PSP = 27
};

// max values for stack and code arrays
#define MAX_STACK_HEIGHT 2048
#define MAX_CODE_LENGTH 512

// holds instruction operation and number/address
typedef struct {
  int op; /* opcode */
  int m;  /* M */
} instruction;

// registers
int BP = 0;
int SP = 0;
int PC = 0;

// controls NDB
int NDBon = 0;

// monitors addr
int addr = 0;

// IR, stack and code array declarations
instruction IR;
instruction code[MAX_CODE_LENGTH];
int stack[MAX_STACK_HEIGHT];

// Array that stores the variable assignments for the instructions
char **mnemonics;

// function prototypes
void openFileCheck(FILE *CLFile);
void readFile(FILE *CLFile);
void init_Hasharray(void);
void machine(void);
void InstructList(int count);
void printPs(void);
void printStack(void);
void popEmpty(bool halt);
void printAll(int addr);

int main(int argc, const char *argv[]) 
{
    // reads command line and opens file
    FILE *commandLineFile = fopen(argv[1], "r");

    // initializes stack
    for (int i = 0; i < MAX_STACK_HEIGHT; i++) 
    {
        stack[i] = 0;
    }

    // Initializes the mnemonics array, i.e allocates the array of strings and fills it with instructions
    init_Hasharray();
  
    // Checks if file is valid and reads input
    openFileCheck(commandLineFile);
    readFile(commandLineFile);

    return 0;
}

// Checks if file is null
void openFileCheck(FILE *CLFile) 
{
    if (CLFile == NULL) 
    {
        printf("Cannot open file!\n");
        exit(-1);
    }
}

// reads information from file
void readFile(FILE *CLFile) 
{
    char *tk;
    char string[20];
    char space[2] = " ";
    char line[2] = "\n";
    int tracker = 0;
    int count = 0;
    int buffer = 20;

    // while a line is read from file, it's split
    // and added to the respective struct int
    // depending on the tracker value
    while(fgets(string, buffer, CLFile) != NULL) 
    {
        if(strcmp(string, line)) 
        {
            // splits string by space
            tk = strtok(string, space);

            // stores as integer in struct  
            while(tk != NULL) 
            {
                if (tracker == 0)
                    code[count].op = atoi(tk);

                if (tracker == 1)
                    code[count].m = atoi(tk);

                tracker++;
                // takes in next value
                tk = strtok(NULL, space);
            }

            tracker = 0;
            count++;
        }
    }

    // prints out list of instructions
    InstructList(count);

    // stack machine call
    machine();
}

// We make an array that stores all the names of the instructions, i.e their mnemonics, into an array of strings. 
// Each mnemonic is ordered in the array in regards to its assigned variable numbering. 
// For example, LIT is the first instruction (has the variable value of 1), and it is thus stored in index 1 of the array (we skip 0 and start at 1).
void init_Hasharray(void) 
{
    mnemonics = malloc(27 * sizeof(char *));

    for(int i = 1; i <= 27; i++) 
    {
        mnemonics[i] = malloc(4 * sizeof(char));

        if (i == 1)
            strcpy(mnemonics[i], "LIT");
        if (i == 2)
            strcpy(mnemonics[i], "RTN");
        if (i == 3)
            strcpy(mnemonics[i], "CAL");
        if (i == 4)
            strcpy(mnemonics[i], "POP");
        if (i == 5)
            strcpy(mnemonics[i], "PSI");
        if (i == 6)
            strcpy(mnemonics[i], "PRM");
        if (i == 7)
            strcpy(mnemonics[i], "STO");
        if (i == 8)
            strcpy(mnemonics[i], "INC");
        if (i == 9)
            strcpy(mnemonics[i], "JMP");
        if (i == 10)
            strcpy(mnemonics[i], "JPC");
        if (i == 11)
            strcpy(mnemonics[i], "CHO");
        if (i == 12)
            strcpy(mnemonics[i], "CHI");
        if (i == 13)
            strcpy(mnemonics[i], "HLT");
        if (i == 14)
            strcpy(mnemonics[i], "NDB");
        if (i == 15)
            strcpy(mnemonics[i], "NEG");
        if (i == 16)
            strcpy(mnemonics[i], "ADD");
        if (i == 17)
            strcpy(mnemonics[i], "SUB");
        if (i == 18)
            strcpy(mnemonics[i], "MUL");
        if (i == 19)
            strcpy(mnemonics[i], "DIV");
        if (i == 20)
            strcpy(mnemonics[i], "MOD");
        if (i == 21)
            strcpy(mnemonics[i], "EQL");
        if (i == 22)
            strcpy(mnemonics[i], "NEQ");
        if (i == 23)
            strcpy(mnemonics[i], "LSS");
        if (i == 24)
            strcpy(mnemonics[i], "LEQ");
        if (i == 25)
            strcpy(mnemonics[i], "GTR");
        if (i == 26)
            strcpy(mnemonics[i], "GEQ");
        if (i == 27)
            strcpy(mnemonics[i], "PSP");
    }
} 

// The virtual machine, it runs and processes all the given (file input) instructions.
void machine(void)
{
    // Bounds check. If any conditions are exceeded, stop virtual machine, as continued operations are invalid.
    if((0 <= BP) && (BP <= SP) && (0 <= SP) && (SP < MAX_STACK_HEIGHT) && (0 <= PC) && (PC < MAX_CODE_LENGTH)) 
    {
        // Monitors the HLT instruction. If true, the virtual machine (the while loop below) will stop.
        bool halt = false;

        // All the instructions are contained here. For each iteration an instruction will be processed and performed.
        while(halt == false)
        {
            IR = code[PC];

            // For each iteration, print out relevant information: addr, PC, BP, SP, the stack, etc, as long as NDB bool is off (i.e NDBon == 0).
            if(NDBon == 0)
            {
                printAll(PC);
            }

            PC++;

            if(IR.op == LIT) 
            {
                stack[SP] = IR.m;
                SP = SP + 1;
            } 
            else if(IR.op == RTN) 
            {
                PC = stack[SP - 1];
                BP = stack[SP - 2];
                SP = SP - 2;
            } 
            else if(IR.op == CAL) 
            {
                stack[SP] = BP;
                stack[SP + 1] = PC;
                BP = SP;
                SP = SP + 2;
                PC = IR.m;
            }
            else if(IR.op == POP)
            {
                SP = SP - 1;
            } 
            else if(IR.op == PSI) 
            {
                stack[SP - 1] = stack[stack[SP - 1]];
            } 
            else if(IR.op == PRM) 
            {
                stack[SP] = stack[BP - IR.m];
                SP = SP + 1;
            } 
            else if(IR.op == STO) 
            {
                stack[stack[SP - 1] + IR.m] = stack[SP - 2];
                SP = SP - 2;
            } 
            else if(IR.op == INC) 
            {
                SP = SP + IR.m;
            } 
            else if(IR.op == JMP) 
            {
                PC = stack[SP - 1];
                SP = SP - 1;
            } 
            else if(IR.op == JPC) 
            {
                if(stack[SP - 1] != 0)
                    PC = IR.m;

                SP = SP - 1;
            }     
            else if(IR.op == CHO) 
            {
                putc(stack[SP - 1], stdout);
                SP = SP - 1;
            } 
            else if(IR.op == CHI) 
            {
                stack[SP] = getc(stdin);
                SP = SP - 1;
            } 
            else if(IR.op == HLT) 
            {
                // Set halt to true if HLT instruction is triggered.
                halt = true;

                // If NDBon is 0, print out last information before exiting the virtual machine.
                if(NDBon == 0)
                {
                    printPs();
                    printStack();
                }
            } 
            else if(IR.op == NDB) 
            {
                // Activate NDB boolean to stop tracing.
                NDBon = 1;
            } 
            else if(IR.op == NEG) 
            {
                stack[SP - 1] = -stack[SP - 1];
            } 
            else if(IR.op == ADD) 
            {
                stack[SP - 2] = stack[SP - 1] + stack[SP - 2];
                SP = SP - 1;
            } 
            else if(IR.op == SUB) 
            {
                stack[SP - 2] = stack[SP - 1] - stack[SP - 2];
                SP = SP - 1;
            } 
            else if(IR.op == MUL) 
            {
                stack[SP - 2] = stack[SP - 1] * stack[SP - 2];
                SP = SP - 1;
            } 
            else if(IR.op == DIV) 
            {
                if(stack[SP - 2] == 0)
                {
                    fprintf(stderr, "Divisor is zero in DIV instruction!\n");
                    halt = true;
                    exit(1);
                }

                stack[SP - 2] = stack[SP - 1] / stack[SP - 2];
                SP = SP - 1;

            } 
            else if(IR.op == MOD) 
            {
                if(stack[SP - 2] == 0)
                {
                    fprintf(stderr, "Modulus is zero in MOD instruction!\n");
                    halt = true;
                    exit(1);
                }

                stack[SP - 2] = stack[SP - 1] % stack[SP - 2];
                SP = SP - 1;

            }
            else if(IR.op == EQL) 
            {
                if (stack[SP - 1] == stack[SP - 2])
                    stack[SP - 2] = 1;
                else
                    stack[SP - 2] = 0;

                SP = SP - 1;
            }
            else if(IR.op == NEQ) 
            {
                if (stack[SP - 1] != stack[SP - 2])
                    stack[SP - 2] = 1;
                else 
                    stack[SP - 2] = 0;

                SP = SP - 1;
            }
            else if(IR.op == LSS) 
            {
                if (stack[SP - 1] < stack[SP - 2])
                    stack[SP - 2] = 1;
                else
                    stack[SP - 2] = 0;

                SP = SP - 1;
            }
            else if(IR.op == LEQ) 
            {
                if (stack[SP - 1] <= stack[SP - 2])
                    stack[SP - 2] = 1;
                else
                    stack[SP - 2] = 0;

                SP = SP - 1;
            }
            else if(IR.op == GTR) 
            {
                if (stack[SP - 1] > stack[SP - 2])
                    stack[SP - 2] = 1;
                else
                    stack[SP - 2] = 0;

                SP = SP - 1;
            }
            else if(IR.op == GEQ) 
            {
                if (stack[SP - 1] >= stack[SP - 2])
                    stack[SP - 2] = 1;
                else
                    stack[SP - 2] = 0;

                SP = SP - 1;
            }
            else if(IR.op == PSP) 
            {
                stack[SP] = SP;
                SP = SP + 1;
            }

            // Check to see if stack is empty at the end of each iteration.
            popEmpty(halt);
        }
    }
    else
    {
        fprintf(stderr, "Out of bounds\n");
        exit(1);
    }
}

// Prints out the given instructions before the tracing. 
void InstructList(int count) 
{
    printf("Addr \t OP \t M \t\n");

    for (int i = 0; i <= count; i++) 
    {
        IR = code[i];

        if(mnemonics[IR.op] == NULL)
        {
            break;
        }

        printf("%d \t %s \t %d\n", i, mnemonics[IR.op], IR.m);
    }

    printf("Tracing ...\n");
}

// Prints out only the PC and stack pointers.
void printPs(void)
{
    printf("PC: %d BP: %d SP: %d\n", PC, BP, SP);
}

// Prints out the stack and elements stored within.
void printStack(void)
{
    printf("stack: ");

    for(int i = BP; i < SP; i++) 
    {
      printf("S[%d]: %d ", i, stack[i]);
    }

    printf("\n");
}

// Checks if stack is empty and exits if true
void popEmpty(bool halt)
{
    if(SP < 0)
    {
        fprintf(stderr, "Trying to pop an empty stack!\n");
        halt = true;
        exit(1);
    }
}
// Prints registers, stack, and addr.
void printAll(int addr)
{
    printf("PC: %d BP: %d SP: %d\n", PC, BP, SP);

    printf("stack: ");

    // Prints out the stack and all its elements within.
    for(int i = BP; i < SP; i++) 
    {
        printf("S[%d]: %d ", i, stack[i]);
    }

    printf("\n");

    printf("==> addr: %d  \t %s \t %d\n", addr, mnemonics[IR.op], IR.m);

}