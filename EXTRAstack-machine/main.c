#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

#define MAX_STACK_HEIGHT 2048
#define MAX_CODE_LENGTH 512

typedef struct {
  int op; /* opcode */
  int m;  /* M */
} instruction;

int BP = 0;
int SP = 0;
int PC = 0;
int NDBon = 0;

instruction IR;
instruction code[MAX_CODE_LENGTH];
int stack[MAX_STACK_HEIGHT];
char **mnemonics;

void openFileCheck(FILE *CLFile);
void readFile(FILE *CLFile);
void init_Hasharray(void);
void machine(void);

void InstructList(int count);

void printPs(void);
void printStack(void);
void printAddr(int addr);

void popEmpty(void);

void outPrint(int addr);

int main(int argc, const char *argv[]) 
{
    FILE *commandLineFile = fopen(argv[1], "r");

    for (int i = 0; i < MAX_STACK_HEIGHT; i++) 
        stack[i] = 0;
    
    init_Hasharray();
    openFileCheck(commandLineFile);
    readFile(commandLineFile);

    return 0;
}

void openFileCheck(FILE *CLFile) 
{
    if (CLFile == NULL) 
    {
        printf("Cannot open file!\n");
        exit(-1);
    }
}

void readFile(FILE *CLFile) 
{
    char *tk;
    char string[20];
    char space[2] = " ";
    char line[2] = "\n";
    int tracker = 0;
    int count = 0;
    int buffer = 20;

    while(fgets(string, buffer, CLFile) != NULL) 
    {
        if(strcmp(string, line)) 
        {
            tk = strtok(string, space);

            while(tk != NULL) 
            {
                if (tracker == 0)
                    code[count].op = atoi(tk);

                if (tracker == 1)
                    code[count].m = atoi(tk);

                tracker++;
                tk = strtok(NULL, space);
            }

            tracker = 0;
            count++;
        }
    }

    InstructList(count);
    machine();
}

// We make an array that stores all the names of the instructions, i.e their mnemonics, into an array of strings. 
// Each index is ordered in regards to the numbering of its respectively store mnenomic. 
// For example, LIT is the first instruction (position 1), and it is thus stored in index 1 of the array. (We skipped over 0).
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

void machine(void)
{
    if((0 <= BP) && (BP <= SP) && (0 <= SP) && (SP < MAX_STACK_HEIGHT) && (0 <= PC) && (PC < MAX_CODE_LENGTH)) 
    {
        bool halt = false;


        while(halt == false && NDBon != 1)
        {

            IR = code[PC];
            
            if(NDBon == 0) {
                    //printAddr(addr);
                    outPrint(PC);
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
                SP = SP + 2 
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
                halt = true;
                printPs();
                printStack();
            } 
            else if(IR.op == NDB) 
            {
                //printAddr(PC);
                NDBon = 1;
                //printf("N");
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
                    fprintf(stderr, "Divisor is zero in DIV instruction!\n");

                stack[SP - 2] = stack[SP - 1] / stack[SP - 2];
                SP = SP - 1;

            } 
            else if(IR.op == MOD) 
            {
                if(stack[SP - 2] == 0)
                    fprintf(stderr, "Modulus is zero in MOD instruction!\n");

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

            popEmpty();

            /*if(NDBon == 0) {
                //printAddr(addr);
                justPrint(PC);
            }

            PC++;*/

        }

        /*if(NDBon == 0) {
                //printAddr(addr);
                justPrint(PC);
            }
            
            PC++;*/
    }
    else
    {
        fprintf(stderr, "Out of bounds\n");
        exit(1);
    }
}

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

void printPs(void)
{
    printf("PC: %d BP: %d SP: %d\n", PC, BP, SP);
}

void printStack(void)
{
    printf("stack: ");

    for(int i = 0; i < SP; i++) 
    {
      printf("S[%d]: %d ", i, stack[i]);
    }

    printf("\n");
}

void printAddr(int addr)
{
    printf("==> addr: %d  \t %s \t %d\n", addr, mnemonics[IR.op], IR.m);
}


void outPrint(int addr) 
{
    printf("PC: %d BP: %d SP: %d\n", PC, BP, SP);

    printf("stack: ");

    for(int i = 0; i < SP; i++) 
    {
      printf("S[%d]: %d ", i, stack[i]);
    }

    printf("\n");

    printf("==> addr: %d  \t %s \t %d\n", addr, mnemonics[IR.op], IR.m);

}

void popEmpty(void)
{
    if(SP < 0)
        fprintf(stderr, "Trying to pop an empty stack!\n");
}