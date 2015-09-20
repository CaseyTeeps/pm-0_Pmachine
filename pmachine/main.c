//
//  main.c
//  pmachine
//
//  Created by Casey Barth on 9/16/15.
//  Copyright (c) 2015 Casey Barth. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

//File Declaration
FILE* input;
FILE* output;


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

//Instruction Struct
typedef struct instructions {
    int op;
    int l;
    int m;
} instruction;
int structQty = 3;
instruction **code;
instruction *IR;

//Instruction Function Prototypes
void    OPR();
void    SIO();

//Instruction Execution Prototypes
void    loadInstructions(FILE* input);
void    mallocArrays();
void    execute();
void    writeToStack();
int     base(int l, int base);
void    freeAll(FILE* input, FILE* output);

//File Writing Prototypes
void    writeToFile(FILE* output);
void    writeInitialValues(FILE* output);
void    writeOP();

//Pointers
int     *stack;         //programatic stack
int     *dLink;         //points to the activation record that called the new activation record

//Global Vars
int     pc = 0;         //program counter
int     bp = 1;         //base pointer (point to base of current activation record)
int     sp = 0;         //stack pointer (points to the top of the stack)
int     halt = 0;       //when halt is called set bool flag.
int     arCnt = 0;      //count of activation records



int main(){
    /*     comment this before submitting       */
    input = fopen("/Users/caseybarth/Desktop/pMachine/mcode.txt", "r");
    output = fopen("/Users/caseybarth/Desktop/pMachine/stacktrace.txt", "w");
//    input = fopen("mcode.txt", "r");
//    output = fopen("stacktrace.txt", "w");
    
    loadInstructions(input);
    mallocArrays();
    writeToFile(output);
    writeInitialValues(output);
    
    while(bp != 0 && !halt){
        IR = code[pc];              //Fetch the instruction
        writeOP();                  //Output the opcodes
        pc++;                       //increment the PC
        execute();                  //Execute the instruction
    }
    freeAll(input, output);
    
    return 0;
}

//Loads instructions into the structs from the file
void loadInstructions(FILE* input){
    
    code = (instruction**)calloc(MAX_CODE_LENGTH, sizeof(instruction*));
    int sInd = 0;
    
    //Reads in each instruction, lexi, and Parameter
    int OP, L, M;
    while(fscanf(input, "%d %d %d", &OP, &L, &M)== structQty){
        code[sInd]  =   (instruction*)malloc(sizeof(instruction));
        code[sInd]  ->  op = OP;
        code[sInd]  ->  l  = L;
        code[sInd]  ->  m  = M;
        sInd++;
    }
}


//Allocates arrays for the IR, Stack, and The Dynamic Link
void mallocArrays(){
    IR = (instruction*)calloc(1, sizeof(instruction));
    stack = (int*)calloc(MAX_STACK_HEIGHT, sizeof(int));
    dLink = (int*)calloc(MAX_STACK_HEIGHT, sizeof(int));
}

//Output function
void writeToFile(FILE* output){
    int i = 0;
    fprintf(output, "Line\tOP\tL\tM\n");
    
    while(code[i] != NULL){
        fprintf(output, "%d\t%s\t%d\t%d\n", i, opCode[code[i]->op], code[i]->l, code[i]->m);
        i++;
    }
}

//Output function
void writeInitialValues(FILE* output){
    fprintf(output, "\t\t\t\tPC\t BP\t SP\t STACK\nInitial Values:\t\t\t");
    fprintf(output, "%d\t %d\t %d\t", pc, bp, sp);
    fprintf(output, "\n");
}


//Output function
void writeOP(){
    fprintf(output, "%d\t%s\t%d\t%d\t", pc, opCode[code[pc]->op], code[pc]->l, code[pc]->m);
}


//Executes the instruction given on slide 13 of class powerpoints
void execute(){
    switch(IR->op){
            //LIT
            case 1:     sp++;
                        stack[sp] = IR->m;
                        break;
            
            //OPR
            case 2:     OPR();
                        break;
            
            //LOD
            case 3:     sp++;
                        stack[sp] = stack[base(IR->l, bp) + IR->m];
                        break;
            
            //STO
            case 4:     stack[base(IR->l, bp) + IR->m] = stack[sp];
                        sp--;
                        break;
            
            //CAL
            case 5:     dLink[arCnt] = sp;
                        arCnt++;
                        stack[sp + 1] = 0;
                        stack[sp + 2] = base(IR->l, bp);
                        stack[sp + 3] = bp;
                        stack[sp + 4] = pc;
                        bp = sp + 1;
                        pc = IR->m;
                        break;
            
            //INC
            case 6:     sp = sp + IR->m;
                        break;
            
            //JMP
            case 7:     pc = IR->m;
                        break;
            
            //JPC
            case 8:     if(stack[sp] == 0){
                            pc = IR->m;
                            sp--;
                        } else {
                            sp--;
                        }
                        break;
            
            //SIO CASES
            case 9:     SIO();
            case 10:    SIO();
            case 11:    SIO();
    }
    
    fprintf(output, "%d\t %d\t %d\t", pc, bp, sp);
    writeToStack();
}



//Output values of stack per activation record.
void writeToStack(){
    int j = 0;
    for(int i = 1; i <= sp; i++){
        
        fprintf(output, "%d ", stack[i]);
        
        
        //new activation record for pipe line
        if(dLink[j] == i && i < sp){
            j++;
            fprintf(output, "| ");
        }
        
        
    }
    
    fprintf(output, "\n");
}


//Given function to find base
int base(int l, int base){
    int b1 = base;
    while (l > 0)
    {
        b1 = stack[b1 + 1];
        l--;
    }
    return b1;
}

//switch case for ALU Operations
void OPR(){
    switch(IR->m){
        //RET
        case 0:     sp = bp - 1;
                    pc = stack[sp + 4];
                    bp = stack[sp + 3];
                    break;
        
        //NEG
        case 1:     stack[sp] = -stack[sp];
                    break;
        
        //ADD
        case 2:     sp--;
                    stack[sp] = stack[sp] + stack[sp + 1];
                    break;
        
        //SUB
        case 3:     sp--;
                    stack[sp] = stack[sp] - stack[sp + 1];
                    break;
        
        //MUL
        case 4:     sp--;
                    stack[sp] = stack[sp] * stack[sp + 1];
                    break;
        
        //DIV
        case 5:     sp--;
                    stack[sp] = (stack[sp] / stack[sp + 1]);
                    break;
        
        //ODD
        case 6:     if (stack[sp] & 1){
                        stack[sp] = 1;
                    } else {
                        stack[sp] = 0;
                    }
                    break;
        
        //MOD
        case 7:     sp--;
                    stack[sp] = (stack[sp] % stack[sp + 1]);
                    break;
        
        //EQL
        case 8:     sp--;
                    if (stack[sp] == stack[sp + 1]){
                        stack[sp] = 1;
                    } else {
                        stack[sp] = 0;
                    }
                    break;
        
        //NEQ
        case 9:     sp--;
                    if (stack[sp] != stack[sp + 1]){
                        stack[sp] = 1;
                    } else {
                        stack[sp] = 0;
                    }
                    break;
        
        //LSS
        case 10:    sp--;
                    if (stack[sp] < stack[sp + 1]){
                        stack[sp] = 1;
                    } else {
                        stack[sp] = 0;
                    }
                    break;
        
        //LEQ
        case 11:    sp--;
                    if (stack[sp] <= stack[sp + 1]) {
                        stack[sp] = 1;
                    } else {
                        stack[sp] = 0;
                    }
            
                    break;
        
        //GTR
        case 12:    sp--;
                    if (stack[sp] > stack[sp + 1]) {
                        stack[sp] = 1;
                    } else {
                        stack[sp] = 0;
                    }
                    break;
       
        //GEQ
        case 13:    sp--;
                    if (stack[sp] >= stack[sp + 1]) {
                        stack[sp] = 1;
                    } else {
                        stack[sp] = 0;
                    }
                    break;
    }
}


void SIO(){

    if (IR->m == 1) {
        printf("%d", stack[sp]);
        sp--;
    }
    else if (IR->m == 2){
        sp++;
        scanf("%d", &stack[sp]);
    } else {
        halt = 1;
        pc = 0;
        sp = 0;
        bp = 0;
    }

}




void freeAll(FILE* input, FILE* output){
    
    fclose(input);
    fclose(output);
    int i = 0;
    while(code[i] != NULL){
        free(code[i]);
        i++;
    }
}
