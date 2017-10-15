/*
Tony Lee & Munkhuush Ganbold
Mini Course Project
12/12/16
Question 3

This program takes in a simple assembly code program, translates it to
machine code, loads machine code into memory, then executes the machine
code.
*/

#include<stdlib.h>
#include<stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define INPUT_FILE_NAME "1.asm"
#define MEM_ARR_LENGTH 10

enum {
    ADD, //0
    LW, //1
    OTHER //2
};

typedef struct Memory{
    char var_name[32];
    int value;
} Memory;

int binToDecimal(char *);
char* reg_translator(char *);
void getFromInputFile(void);
void convertToMach_I(char *, int);
void convertToMach_R(char *, int);
void PC(void);                          //Keeps track of current myProgramCounter
void I_MEM(char*, int);                 //Determines current instruction
void ALU(char*, char*);                 //Executes current instruction
void WRITE_REGISTER_FILE(int, int);     //Writes to specified register file
void WRITE_DATA_MEM(int, int);          //Writes to specified register file (for model)
int READ_REGISTER_FILE(int);            //Reads from specified register num
int READ_DATA_MEM(int);                 //Reads from specified register num (for model)
void D_MEM(void);                       //Stores ALU results

int myRegisterFile[32];        //32-bit Register File Array
char myInstruction[32];     //Current instruction set by I_MEM
int myProgramCounter;       //Address of current instruction set by PC
Memory loadedMemory[MEM_ARR_LENGTH];
int memoryCounter;

void main(){
    memoryCounter = 0;
    myProgramCounter = 0;   //initialize PC
    // myRegisterFile = {0};
    getFromInputFile();
}

/*
This function should have a bunch of if statements to determine
what the specified instruction does.  
    *If storing data, use: 
            WRITE_DATA_MEM(int setLocation, int newValue)
    *If reading data, use :
            READ_DATA_MEM(int readLocation)
*/
void ALU(char *theInstruction, char *theOperation) {
    char sp[7], rs[6], rt[6], rd[6], zero[6], base[6], offset[17];;
    int num1, num2, sum;
    
    //ADD
    if (strcmp(theOperation, "100000") == 0) {
        sscanf(theInstruction, "%6s%5s%5s%5s%5s", sp, rs, rt, rd, zero);
        // printf("sp: %s rs: %s rt: %s rd: %s zero: %s\n", sp, rs, rt, rd, zero);
        num1 = READ_DATA_MEM(binToDecimal(rs));
        num2 = READ_DATA_MEM(binToDecimal(rt));
        // printf("num1: %d num2: %d\n", num1, num2);
        sum = num1 + num2;
        WRITE_DATA_MEM(binToDecimal(rd), sum);
    } 
    //LW
    else if (strcmp(theOperation, "100011") == 0) {
        sscanf(theInstruction, "%5s%5s%16s", base, rt, offset);
        // printf("base: %s rt: %s offset: %s\n", base, rt, offset);
        num1 = loadedMemory[(binToDecimal(offset) / 4)].value;
        // printf("num1: %d\n", num1);
        WRITE_DATA_MEM(binToDecimal(rt), num1);
    } 
}

void WRITE_REGISTER_FILE(int setLocation, int newValue) {
    myRegisterFile[setLocation] = newValue;
}

int READ_REGISTER_FILE(int readLocation) {
    return myRegisterFile[readLocation];
}

void I_MEM(char *binInstructions, int type) {
    char operation[6];
    // printf("the binary: %s\n", binInstructions);
    if (type == ADD) {
        int len = strlen(binInstructions);
        char *temp = binInstructions;
        char *temp2;
        strncpy(temp2, binInstructions, 27);
        temp2[26] = '\0';
        temp += 26;
        
        strncpy(operation, temp, 6);
        // printf("The operation: %s\n", operation);
        // printf("The instructions: %s\n", temp2);
        ALU(temp2, operation);
    } else if (type == LW) {
        strncpy(operation, binInstructions, 6);
        binInstructions += 6;
        // printf("The operation: %s\n", operation);
        // printf("The instructions: %s\n", binInstructions);
        ALU(binInstructions, operation);
    }   
}

//Redundant code, but necessary for mips computer model.
void WRITE_DATA_MEM(int setLocation, int newValue) {
    WRITE_REGISTER_FILE(setLocation, newValue);
}

//Redundant code, but necessary for mips computer model.
int READ_DATA_MEM(int readLocation) {
    READ_REGISTER_FILE(readLocation);
}

void PC() {
    printf("PC:%d Registers: $t0=%d, $t1=%d, $s0=%d\n", myProgramCounter, myRegisterFile[8], myRegisterFile[9], myRegisterFile[16]);
    myProgramCounter++;
}

// Identify the register and return its binary 
char* reg_translator(char *reg_word) {
    if (strcmp(reg_word, "$T0") == 0) { // Reg $t0
            return ("01000");
    } else if (strcmp(reg_word, "$T1") == 0) { // Reg $t1
        return ("01001");
    } else if (strcmp(reg_word, "$S0") == 0) { // Reg $s0
        return ("10000");
    } 
}

int binToDecimal(char *reg_word)
{
    char* start = &reg_word[0];
    int total = 0;
    while (*start)
    {
     total *= 2;
     if (*start++ == '1') total += 1;
    }
    return total;
}

//I type instruction
void convertToMach_I(char *line, int type) {
    char line_code[34];
    char opcode[6], Rx_store[5], var[32];
    int immediate, offset;
    bool is_valid = true;

    if (type == LW) {
        sscanf(line, "%[^' '] %[^','], %[^' ']", opcode, Rx_store, var);
       
        // printf("opcode: %s rx_store: %s  var: %s\n", opcode, Rx_store, var);
        if (reg_translator(Rx_store) == NULL) {
            is_valid = false;
        }

        if (is_valid) {
            //LW code
            strcpy(line_code, "100011");
            //$at
            strcat(line_code, "00001");
            strcat(line_code, reg_translator(Rx_store));
            //offset
            int i;
            for (i = 0; i < MEM_ARR_LENGTH; i++) {
                if (strcmp(loadedMemory[i].var_name, var) == 0) {
                    if (i == 0) {
                        strcat(line_code, "0000000000000000");
                    } else if (i == 1) {
                        strcat(line_code, "0000000000000100");
                    }
                }
            }
            I_MEM(line_code, LW);
        } else {
            // invalid_report("LW - Invalid regsister index on line ", index);
        }

    } else {
        //error
    }
}

//R type instruction
void convertToMach_R(char *line, int type) {
    char line_code[34];
    char opcode[6], Rx_store[6], Ry_a[6], Rz_b[6];
    bool is_valid = true;

    sscanf(line, "%[^' '] %[^','], %[^','], %[^' ']", opcode, Rx_store, Ry_a, Rz_b);
    
    // printf("opcode: %s  rx_store: %s  ry_a: %s  rz_b: %s\n", opcode, Rx_store, Ry_a, Rz_b);
    if (reg_translator(Rx_store) == NULL || reg_translator(Ry_a) == NULL
            || reg_translator(Rz_b) == NULL) {
        printf("invalid\n");
        is_valid = false;
    }

    if (is_valid) {
        //add special:
        strcpy(line_code, "000000");
        
        if (type == ADD) {
            strcat(line_code, reg_translator(Ry_a));
            strcat(line_code, reg_translator(Rz_b));
            strcat(line_code, reg_translator(Rx_store));
            //0
            strcat(line_code, "00000");
            //ADD code
            strcat(line_code, "100000");
        } else {
            // GUI_popup_error("ASSEMBLER R_type_tranlator: Something happened!");
        }
        
        I_MEM(line_code, ADD);

    } else {
        // invalid_report("ADD/NAND - Invalid regsister index on line ", index);
    }
}

void translateType(char *line) {
    char operation[6], line_code[34];
    // Convert to uppercase
    int i = 0;
    while (line[i]) {
        line[i] = (toupper(line[i]));
        i++;
    }
    sscanf(line, "%s ", operation);
    bool is_valid = false;
    
    if (strcmp(operation, "ADD") == 0 || strcmp(operation, ".DATA") == 0 || strcmp(operation, ".TEXT") == 0
            || strcmp(operation, "LW") == 0 || strcmp(operation, "MAIN:") == 0 
            || strcmp(operation, ".END") == 0 || strcmp(operation, "LI") == 0
            || strcmp(operation, "SYSCALL") == 0 || strstr(line, ".WORD") != NULL) {

        is_valid = true;
    }
    
    if (is_valid) {
        //add
        if (strcmp(operation, "ADD") == 0) {
            // printf("add\n");
            convertToMach_R(line, ADD);
            printf("add\n");
        }
        //.data
        else if (strcmp(operation, ".DATA") == 0) {
            //do nothing
            printf(".data\n");
        }
        //.text
        else if (strcmp(operation, ".TEXT") == 0) {
            //do nothing
            printf(".text\n");
        }
        //main:
        else if (strcmp(operation, "MAIN:") == 0) {
            //do nothing
            printf("main:\n");
        }
        //end:
        else if (strcmp(operation, "END:") == 0) {
            //do nothing
            printf("end:\n");
        }
        //lw 
        else if (strcmp(operation, "LW") == 0) {
            printf("lw\n");
            convertToMach_I(line, LW);
        }
        //li
        else if (strcmp(operation, "LI") == 0) {
            printf("li\n");
            strcpy(line_code, "00110100000000100000000000001010");
            I_MEM(line_code, OTHER);
            
        }
        //syscall
        else if (strcmp(operation, "SYSCALL") == 0) {
            printf("syscall\n");
            strcpy(line_code, "00000000000000000000000000001100");
            I_MEM(line_code, OTHER);
        }
        //word
        else if(strstr(line, ".WORD")) {
            printf(".word\n");
            char var_name[32];
            int value;
            sscanf(line, "%[^': ']: .WORD %[^' '] \n", var_name, &value);
            // printf("var_name: %s  value: %d\n", var_name, value - 48);
            strcpy(loadedMemory[memoryCounter].var_name, var_name);
            loadedMemory[memoryCounter].value = value - 48;
            printf("Memory Struct %d: var_name: %s value: %d\n", memoryCounter, loadedMemory[memoryCounter].var_name, 
                        loadedMemory[memoryCounter].value);
            memoryCounter++;
        }
    }
}

void getFromInputFile() {
    FILE* mySourceFile;
    char data[4][20];
    char line[255];

    mySourceFile = fopen(INPUT_FILE_NAME, "r");

    //read one line at a time from file
    while (fgets(line, 255, (FILE*)mySourceFile)!= 0){
        PC();                                   //increment counter
        fgets(line, 1, (FILE*)mySourceFile);    //get line
        translateType(line);                              //execute instruction
    }
    fclose(mySourceFile);
}

