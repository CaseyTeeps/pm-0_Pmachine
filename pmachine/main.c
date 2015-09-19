//
//  main.c
//  pmachine
//
//  Created by Casey Barth on 9/16/15.
//  Copyright (c) 2015 Casey Barth. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

//defined in spec
#define MAX_STACK_HEIGHT = 2000;
#define MAX_CODE_LENGTH = 500;
#define MAX_LEXI_LEVELS = 3

//Prototypes
void fetch();
void execute();
void freeAll(FILE* input);



int     pc = 0;         //program counter
int     bp = 1;         //base pointer (point to base of current activation record)
int     sp = 0;         //stack pointer (points to the top of the stack)
int     halt = 0;       //when halt is called set bool flag.
int     arCnt = 0;      //count of activation records

//DECLARE OP CODE ARRAY OF TYPE STRING FOR OUTPUT
char*  opCode[] = {
    "",
    "LIT",
    "OPR",
    "LOD",
    "STO",
    "CAL",
    "INC",
    "JMP",
    "JPC",
    "SIO",
    "SIO",
    "SIO"
};

//STRUCT DEFINED FOR INSTRUCTION FORMAT
struct instruction {
    int op;             //OP CODE
    int l;              //LEXIGRAPHICAL LEVEL
    int m;              //PARAMETER
};

struct instruction **code;
struct instruction *IR;

//DECLARE OUTPUT POINTER
FILE* output;

int main(int argc, const char * argv[]) {
    
    //OPEN THE MACHINE CODE
    FILE* input = fopen("mcode.txt", "r");
    
    
    
    
    return 0;
}
