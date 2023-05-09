// Assembler code for LC-2K 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

#define NUMREGS 8
#define MAXMEM 65536

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

int formatO(int);
int formatJ(int,char*,char*);
int formatR(int,char*,char*,char*);
int labeling(FILE*, char*);

int main(int argc, char *argv[]) {
    char *inFileString, *outFileString;
    FILE *inFilePtr, *inFilePtr2, *outFilePtr;

    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH];
    char arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc!= 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n", argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    inFilePtr2 = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
    }

    
	/* this is how to rewind the file ptr so that you start reading from the
		 beginning of the file */
	rewind(inFilePtr);

	/* here is an example for how to use readAndParse to read a line from
		 inFilePtr */

    int i;
    int op;

    while(1) {
	  	if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
	    	break;
	    }
	    
	    int num;
	    
	    if (!strcmp(opcode, "add")) {
			/* do whatever you need to do for opcode "add" */
			op = 0 << 22;
			num = formatR(op, arg0, arg1, arg2);
		}
		else if (!strcmp(opcode, "nor")) {
			op = 1 << 22;
			num = formatR(op, arg0, arg1, arg2);
		}
	    else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) {
	    	num = formatI(inFilePtr2, i, opcode, arg0, arg1, arg2);
	    }

	    else if (!strcmp(opcode, "jalr")) {
			op = 5 << 22;
			num = formatJ(op, arg0, arg1);   
		}
		else if (!strcmp(opcode, "halt")) {
			op = 6 << 22;
			num = formatO(op);
		}
		else if (!strcmp(opcode, "noop")) {
			op = 7 << 22;
			num = formatO(op);
		}

	    else if (!strcmp(opcode, ".fill")) {
	    	{
				if (isNumber(arg0))
				{
					num = atoi(arg0);
				}
				else
				{
					num = labeling(inFilePtr, arg0);
				}
			}
	    }
	    else {
	    	printf("unrecognized opcodes");
	    	exit(1);
	    }

	    fprintf(outFilePtr, "%d\n", num);
        i++;
	}

    /* TODO: Phase-1 label calculation */


	/* TODO: Phase-2 generate machine codes to outfile */

	/* after doing a readAndParse, you may want to do the following to test the
		 opcode */


    if (inFilePtr2) {
		fclose(inFilePtr2);
	}
    if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	return(0);
    exit(0);
}

int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2) {
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        /* reached end of file */
        return 0;
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n\r ]", label)) {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
    * Parse the rest of the line. Would be nice to have real regular expressions, but scanf will suffice.
    */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r "
                  "]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
    
    return 1;
}

int isNumber(char *string) {
    /* return 1 if string is a number */
    int i;
    return ((sscanf(string, "%d", &i)) == 1);
}

int labeling(FILE *file, char *alabel) {
    int address;
    int allo = 0;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    rewind(file);


    int i = 0;
    while (readAndParse(file, label, opcode, arg0, arg1, arg2)) {
      if (!strcmp(label, alabel)) {
        address = i;
        allo++;
      }
      i++;
    }

	if (allo==0){
		printf("use of undefined labels");
		exit(1);
	}
	
	else if (allo>1){
		printf("duplicate definition of labels");
		exit(1);
	}
    
	return address;

}
int formatR(int number, char* arg0, char* arg1, char* arg2) {

    if(!isNumber(arg0) || !isNumber(arg1)) {
		printf("non_integer register arguments");
		exit(1);
	}
    if (atoi(arg0)<0||atoi(arg0)>7||atoi(arg1)<0||atoi(arg1)>7){
        printf("registers outstide the range [0,7]");
        exit(1);
    }
	number |= atoi(arg0) << 19;
	number |= atoi(arg1) << 16;
	number |= atoi(arg2) << 0;
	return number;
}

int formatJ(int number, char* arg0, char* arg1) {

    if(!isNumber(arg0) || !isNumber(arg1)) {
		printf("non_integer register arguments");
		exit(1);
	}
    if (atoi(arg0)<0||atoi(arg0)>7||atoi(arg1)<0||atoi(arg1)>7){
        printf("registers outstide the range [0,7]");
        exit(1);
    }

	number |= atoi(arg0) << 19;
	number |= atoi(arg1) << 16;
	return number;
}
int formatO(int number) {
	return number;
}
int formatI(FILE *file, int counter, char *opcode, char *arg0, char *arg1, char *offset) {
	int chosen = 0;
	int dis;
    if(!isNumber(arg0) || !isNumber(arg1)) {
		printf("non_integer register arguments");
		exit(1);
	}
    if (atoi(arg0)<0||atoi(arg0)>7||atoi(arg1)<0||atoi(arg1)>7){
        printf("registers outstide the range [0,7]");
        exit(1);
    }
	if (!strcmp(opcode, "lw"))
		chosen = 2; 

	else if (!strcmp(opcode, "sw"))
		chosen = 3;	

	else if (!strcmp(opcode, "beq"))
		chosen = 4;	


	if(isNumber(offset)) {
		dis = atoi(offset);
        if(dis > 32767 || dis < -32768) {
			printf("offsetFields that don't fit in 16 bits");
			exit(1);
		}
	}

	else {
		dis = labeling(file, offset);
	}

	if(!strcmp(opcode, "beq") && !isNumber(offset)) {
		dis = dis - counter - 1;
	}

	if(dis < 0)
		dis += 65536;

	return (chosen << 22) + (atoi(arg0) << 19) + (atoi(arg1) << 16) + dis;
	
}
