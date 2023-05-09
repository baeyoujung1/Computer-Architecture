/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;
    }
    
    /* TODO: */
    int i=0;
    int opcode, reg0, reg1, offset;
    //loop 조건문을 halt로
    while (1){

        printState(&state);
        
        int num = state.mem[state.pc];
        opcode=(num>>22)%8;
        reg0=(num>>19)%8;
        reg1=(num>>16)%8;
        offset=convertNum(num&0xFFFF);

        if (reg0<0||reg0>7||reg1<0||reg1>7){
            exit(1);
        }

        if (offset>32767||offset<-32768){
            exit(1);
        }
        
        if (opcode == 6) {
            printf("Machine halted.\ntotal of %d instructions"
            " executed.\nFinal state of machine: \n", i + 1);
            state.pc++;
            printState(&state);
            exit(1);
        }
        else if (opcode==0){
            state.reg[offset] = state.reg[reg0] + state.reg[reg1];
            
        }
        else if (opcode==1){
            state.reg[offset] = ~(state.reg[reg0] | state.reg[reg1]);
        }
        else if (opcode==2){
            int l_offset = state.reg[reg0]+offset;
            state.reg[reg1] = state.mem[l_offset];
        }
        else if (opcode==3){
            int s_offset = state.reg[reg1]+offset;
            state.mem[s_offset] = state.reg[reg1];
        }
        else if (opcode==4){
            if (state.reg[reg0] == state.reg[reg1]) {
                state.pc = state.pc + offset;
            }
}
        else if (opcode==5){
            state.reg[reg1] = state.pc+1;
            state.pc = state.reg[reg0]-1;
        }
        else if (opcode==7){
            
        }
        else {
            exit(1);
        }

        state.pc++;
        i++;
    }
    fclose(filePtr);
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}
