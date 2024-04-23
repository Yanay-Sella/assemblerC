#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "global.h"
#include "second_scan.h"
#include "first_scan.h" /* for the data count */

int IC = 0;

void addAdditionalWords(char* instructionLine) {
    char* operand1;
    char* operand2;
    InstructionParts parts;
    
    IC++;
    parts = splitInstruction(instructionLine);
    operand1 = parts.operand1;
    operand2 = parts.operand2;

    if(operand2 == NULL && operand1 != NULL) {
        operand2 = operand1;
        operand1 = NULL;
    }
    
    /* remove leading spaces from both operands */
    while(operand1 != NULL && isspace((unsigned char)*operand1)) operand1++;
    while(operand2 != NULL && isspace((unsigned char)*operand2)) operand2++;

    /* Check if both operands are registers */
    if (operand1 != NULL && operand1[0] == 'r' && operand2 != NULL && operand2[0] == 'r') {
        /* both operands are registers, add one word for both */
        int reg1 = operand1[1];
        int reg2 = operand2[1];
        char binRegisters[15] = "00000000000000";
        char binReg1Num[4];
        char binReg2Num[4];
        int i;

        /* convert the registers numbers to a 3 bits binary */
        for (i = 2; i >= 0; i--) {
            binReg1Num[i] = (reg1 & 1) ? '1' : '0';
            reg1 >>= 1;
        }
        for (i = 2; i >= 0; i--) {
            binReg2Num[i] = (reg2 & 1) ? '1' : '0';
            reg2 >>= 1;
        }

        binReg1Num[3] = '\0';
        binReg2Num[3] = '\0';

        binRegisters[13-7] = binReg1Num[0];
        binRegisters[13-6] = binReg1Num[1];
        binRegisters[13-5] = binReg1Num[2];
        binRegisters[13-4] = binReg2Num[0];
        binRegisters[13-3] = binReg2Num[1];
        binRegisters[13-2] = binReg2Num[2];

        binRegisters[14] = '\0'; /* null terminate the string */
        /* add to instructionTable */
        strcpy(instructionTable[IC], binRegisters);
        IC++;
        return; /* no additional checks needed */ 
    }

    /* ~~~~~~~~~~~~~ FIRST OPERAND ~~~~~~~~~~~~~ */
    if(operand1 != NULL) {
        /* immediate addressing #k */  
        if (operand1[0] == '#') {
            /* Extract the value after the # */
            char* valueStr = operand1 + 1;
            int value;
            char binary[15];
            int i;

            if (sscanf(valueStr, "%d", &value) != 1) {
                /* valueStr is not a number, get it from the symbol table */
                value = findMDefine(valueStr);
            }

            for (i = 11; i >= 0; i--) {
                binary[i] = (value & 1) ? '1' : '0';
                value >>= 1;
            }

            /* add ARE bits */
            binary[12] = '0';
            binary[13] = '0';
            binary[14] = '\0'; /* null terminate the string */

            /* add to instructionTable */
            strcpy(instructionTable[IC], binary);
        }

        /* index addressing: arr[k]*/    
        else if (strstr(operand1, "[") && strstr(operand1, "]")) {
            char* arrayName;
            int symbolIndex;
            int arrAddress;
            char binAddress[15];
            int i;
            char* indexStr;
            char binIndex[15]; /* 14 bits plus null terminator */
            int value;
            char* tempOperand;
            char* tempOperand2;
            
            tempOperand = malloc(strlen(operand1) + 1); /* allocate memory for the copy */
            if (tempOperand == NULL) {
                printf("ERROR: memory allocation failed\n");
                exit(1);
            }
            tempOperand2 = malloc(strlen(operand1) + 1); /* allocate memory for the copy */
            if (tempOperand2 == NULL) {
                printf("ERROR: memory allocation failed\n");
                exit(1);
            }
            strcpy(tempOperand, operand1); /* copy the string */
            strcpy(tempOperand2, operand1); /* copy the string */

            /* extract the array name */
            arrayName = strtok(tempOperand, "[");
            symbolIndex = findSymbol(arrayName);
            arrAddress = symbolTable[symbolIndex].value;

            /* convert the address to a 12 bits binary */
            for (i = 11; i >= 0; i--) {
                binAddress[i] = (arrAddress & 1) ? '1' : '0';
                arrAddress >>= 1;
            }

            /* adding the A,R,E bits for each case possilbe */
            if(strcmp(symbolTable[symbolIndex].identifier, "external") == 0){

                /* add the current instruction number to the locations array for this symbol */
                for (i = 0; i < externUsageCount; i++) {
                    if (strcmp(externUsageTable[i].symbolName, arrayName) == 0) {
                        /* found the symbol in the table, add the instruction number */
                        externUsageTable[i].locations[IC] = IC;
                        break;
                    }
                }
                if (i == externUsageCount) {
                    /* didnt find the symbol in the table, add a new entry */
                    strcpy(externUsageTable[i].symbolName, arrayName);
                    externUsageTable[i].locations[0] = IC;
                    externUsageCount++;
                }

                binAddress[12] = '0';
                binAddress[13] = '1';
            } else {
                binAddress[12] = '1';
                binAddress[13] = '0';
            }
            binAddress[14] = '\0'; /* null terminate the string */
            strcpy(instructionTable[IC], binAddress);
            
            /* two words are added so adding the first here */
            IC++;
            
            free(tempOperand);

            /* getting the index inside the square brackets [index] */
            indexStr = strtok(tempOperand2, "[");
            indexStr = strtok(NULL, "]");
            
            if (sscanf(indexStr, "%d", &value) != 1) {
                /* indexStr is not a number, get it from the symbol table */
                value = findMDefine(indexStr);
            }
            
            free(tempOperand2);

            /* convert to binary... */
            for (i = 11; i >= 0; i--) {
                binIndex[i] = (value & 1) ? '1' : '0';
                value >>= 1;
            }
            /* append '00' to the end of the binary number */
            binIndex[12] = '0';
            binIndex[13] = '0';
            binIndex[14] = '\0'; /* null terminate the string */

            /* adding the index word to the instructionTable */
            strcpy(instructionTable[IC], binIndex);
        }

        /* direct register addressing: r4 */
        else if (operand1[0] == 'r' && isdigit(operand1[1]) && operand1[1] >= '0' && operand1[1] <= '7') {
            int regNum = operand1[1];
            char binRegister[15] = "00000000000000";
            char binRegNum[4];
            int i;

            /* convert the register number to a 3 bits binary */
            for (i = 2; i >= 0; i--) {
                binRegNum[i] = (regNum & 1) ? '1' : '0';
                regNum >>= 1;
            }
            binRegNum[3] = '\0';
            binRegister[13-7] = binRegNum[0];
            binRegister[13-6] = binRegNum[1];
            binRegister[13-5] = binRegNum[2];

            binRegister[14] = '\0'; /* null terminate the string */

            /* add the binary word to the instructionTable */
            strcpy(instructionTable[IC], binRegister);
        }
        /* direct addressing: x */
        else {
            int symbolIndex;
            int symbolAddress;
            char binAddress[15];
            int i;

            symbolIndex = findSymbol(operand1);
            symbolAddress = symbolTable[symbolIndex].value;
            

            /* convert the address to a 12 bits binary */
            for (i = 11; i >= 0; i--) {
                binAddress[i] = (symbolAddress & 1) ? '1' : '0';
                symbolAddress >>= 1;
            }

            /* adding the A,R,E bits for each case possilbe */
            if(strcmp(symbolTable[symbolIndex].identifier, "external") == 0){

                /* add the current instruction number to the locations array for this symbol */
                for (i = 0; i < externUsageCount; i++) {
                    if (strcmp(externUsageTable[i].symbolName, operand1) == 0) {
                        /* found the symbol in the table, add the instruction number */
                        externUsageTable[i].locations[IC] = IC;
                        break;
                    }
                }
                if (i == externUsageCount) {
                    /* didnt find the symbol in the table, add a new entry */
                    strcpy(externUsageTable[i].symbolName, operand1);
                    externUsageTable[i].locations[0] = IC;
                    externUsageCount++;
                }

                binAddress[12] = '0';
                binAddress[13] = '1';
            } else {
                binAddress[12] = '1';
                binAddress[13] = '0';
            }
            binAddress[14] = '\0'; /* null terminate the string */
            strcpy(instructionTable[IC], binAddress);
        } 
        IC++;
    }

    /* ~~~~~~~~~~~~~ SECOND OPERAND ~~~~~~~~~~~~~ */
    if(operand2 != NULL) {
        /* immediate addressing #k */  
        if (operand2[0] == '#') {
            /* Extract the value after the # */
            char* valueStr = operand2 + 1;
            int value;
            char binary[15];
            int i;

            if (sscanf(valueStr, "%d", &value) != 1) {
                /* valueStr is not a number, get it from the symbol table */
                value = findMDefine(valueStr);
            }

            for (i = 11; i >= 0; i--) {
                binary[i] = (value & 1) ? '1' : '0';
                value >>= 1;
            }
            
            /* add ARE bits */
            binary[12] = '0';
            binary[13] = '0';
            binary[14] = '\0'; /* null terminate the string */

            /* add to instructionTable */
            strcpy(instructionTable[IC], binary);
        }

        /* index addressing: arr[k]*/    
        else if (strstr(operand2, "[") && strstr(operand2, "]")) {
            char* arrayName;
            char* tempOperand;
            char* tempOperand2;
            int symbolIndex;
            int arrAddress;
            char binAddress[15];
            int i;
            char* indexStr;
            char binIndex[15]; /* 14 bits plus null terminator */
            int value;

            tempOperand = malloc(strlen(operand1) + 1); /* allocate memory for the copy */
            if (tempOperand == NULL) {
                printf("ERROR: memory allocation failed\n");
                exit(1);
            }
            tempOperand2 = malloc(strlen(operand1) + 1); /* allocate memory for the copy */
            if (tempOperand2 == NULL) {
                printf("ERROR: memory allocation failed\n");
                exit(1);
            }
            strcpy(tempOperand, operand1); /* copy the string */
            strcpy(tempOperand2, operand1); /* copy the string */

            /* extract the array name */
            arrayName = strtok(tempOperand, "[");
            symbolIndex = findSymbol(arrayName);
            arrAddress = symbolTable[symbolIndex].value;

            /* convert the address to a 12 bits binary */
            for (i = 11; i >= 0; i--) {
                binAddress[i] = (arrAddress & 1) ? '1' : '0';
                arrAddress >>= 1;
            }

            /* adding the A,R,E bits for each case possilbe */
            if(strcmp(symbolTable[symbolIndex].identifier, "external") == 0){

                /* add the current instruction number to the locations array for this symbol */
                for (i = 0; i < externUsageCount; i++) {
                    if (strcmp(externUsageTable[i].symbolName, arrayName) == 0) {
                        /* found the symbol in the table, add the instruction number */
                        externUsageTable[i].locations[IC] = IC;
                        break;
                    }
                }
                if (i == externUsageCount) {
                    /* didnt find the symbol in the table, add a new entry */
                    strcpy(externUsageTable[i].symbolName, arrayName);
                    externUsageTable[i].locations[0] = IC;
                    externUsageCount++;
                }

                binAddress[12] = '0';
                binAddress[13] = '1';
            } else {
                binAddress[12] = '1';
                binAddress[13] = '0';
            }
            binAddress[14] = '\0'; /* null terminate the string */
            strcpy(instructionTable[IC], binAddress);
            
            /* two words are added so adding the first here */
            IC++;
            
            free(tempOperand);

            /* getting the index inside the square brackets [index] */
            indexStr = strtok(tempOperand2, "[");
            indexStr = strtok(NULL, "]");
            
            if (sscanf(indexStr, "%d", &value) != 1) {
                /* indexStr is not a number, get it from the symbol table */
                value = findMDefine(indexStr);
            }
            
            free(tempOperand2);

            /* convert to binary... */
            for (i = 11; i >= 0; i--) {
                binIndex[i] = (value & 1) ? '1' : '0';
                value >>= 1;
            }
            /* append '00' to the end of the binary number */
            binIndex[12] = '0';
            binIndex[13] = '0';
            binIndex[14] = '\0'; /* null terminate the string */

            /* adding the index word to the instructionTable */
            strcpy(instructionTable[IC], binIndex);
        }

        /* direct register addressing: r4 */
        else if (operand2[0] == 'r' && isdigit(operand2[1]) && operand2[1] >= '0' && operand2[1] <= '7') {
            int regNum = operand2[1];
            char binRegister[15] = "00000000000000";
            char binRegNum[4];
            int i;

            /* convert the register number to a 3 bits binary */
            for (i = 2; i >= 0; i--) {
                binRegNum[i] = (regNum & 1) ? '1' : '0';
                regNum >>= 1;
            }
            binRegNum[3] = '\0';
            binRegister[13-4] = binRegNum[0];
            binRegister[13-3] = binRegNum[1];
            binRegister[13-2] = binRegNum[2];

            binRegister[14] = '\0'; /* null terminate the string */

            /* add the binary word to the instructionTable */
            strcpy(instructionTable[IC], binRegister);
        }
        /* direct addressing: x */
        else {
            int symbolIndex;
            int symbolAddress;
            char binAddress[15];
            int i;

            symbolIndex = findSymbol(operand2);
            symbolAddress = symbolTable[symbolIndex].value;

            /* convert the address to a 12 bits binary */
            for (i = 11; i >= 0; i--) {
                binAddress[i] = (symbolAddress & 1) ? '1' : '0';
                symbolAddress >>= 1;
            }

            /* adding the A,R,E bits for each case possilbe */
            if(strcmp(symbolTable[symbolIndex].identifier, "external") == 0){

                /* add the current instruction number to the locations array for this symbol */
                for (i = 0; i < externUsageCount; i++) {
                    if (strcmp(externUsageTable[i].symbolName, operand2) == 0) {
                        /* found the symbol in the table, add the instruction number */
                        externUsageTable[i].locations[IC] = IC;
                        break;
                    }
                }
                if (i == externUsageCount) {
                    /* didnt find the symbol in the table, add a new entry */
                    strcpy(externUsageTable[i].symbolName, operand2);
                    externUsageTable[i].locations[0] = IC;
                    externUsageCount++;
                }

                binAddress[12] = '0';
                binAddress[13] = '1';
            } else {
                binAddress[12] = '1';
                binAddress[13] = '0';
            }
            binAddress[14] = '\0'; /* null terminate the string */
            strcpy(instructionTable[IC], binAddress);
        } 
        IC++;
    }
}

void scanAgainAndAddWords(FILE *file, Symbol *symbolTable) {
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        char *colonPosition;

        if (line[0] == ';' || isWhiteLine(line)) {
            continue;
        }

        if (strncmp(line, ".extern", 7) == 0){
            continue;
        }
        else if (strncmp(line, ".entry", 6) == 0) {
            char symbolName[MAX_LINE_LENGTH];
            int symbolIndex;
            sscanf(line, ".entry %s", symbolName);
            symbolIndex = findSymbol(symbolName);
            /* changing the symbol identifier to entry */
            if (symbolIndex != -1) {
                strcpy(symbolTable[symbolIndex].identifier, "entry");
            }
            continue;
        } else if (strncmp(line, ".define", 7) == 0) {
            continue;
        }

        /* if its a symbol defenition like "SYMBOL: ____" */
        /* saving the pointer for the ":" in the line if exists */
        colonPosition = strchr(line, ':');
        if (colonPosition) {
            char *directiveStart;

            /* move the pointer to the start of the directive */
            directiveStart = colonPosition + 1;
            /* Remove leading white spaces */
            while(isspace((unsigned char) *directiveStart)) {
                directiveStart++;
            }
            
            /* skipping .data .string */
            /* .data */
            if (strlen(directiveStart) >= 6 && strncmp(directiveStart, ".data ", 6) == 0) {
                /* do nothing */
            }
            /* .string */
            else if (strlen(directiveStart) >= 8 && strncmp(directiveStart, ".string ", 8) == 0) {
                /* do nothing */
            }
            /* code */
            else {
                char *instructionStart;
                /* Extract the part of the line after the ":" */
                instructionStart = colonPosition + 1;
                /* Skip any leading spaces or tabs */
                instructionStart += strspn(instructionStart, " \t");
                addAdditionalWords(instructionStart);
            }
        }
        /* independant operation */
        else {
            addAdditionalWords(line);
        }
    }
}
