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

    printf("op1:%s  op2:%s\n", operand1, operand2);

    /* ~~~ taking care of the source operand bits 5-4 ~~~ */
    if(operand1 != NULL) {
        /* immediate addressing #k */  
        if (operand1[0] == '#') {
            /* TODO: check for mdefines variables if its not a nunber */
            int num = atoi(operand1 + 1); /* get the number after the # */
            char binary[15];
            int i;

            for (i = 11; i >= 0; i--) {
                binary[i] = (num & 1) ? '1' : '0';
                num >>= 1;
            }
            /* add ARE bits */
            binary[12] = '0';
            binary[13] = '0';
            binary[14] = '\0'; /* null terminate the string */

            /* add to instructionTable */
            strcpy(instructionTable[IC], binary);
            printf("immediate operand, added word:%s\n", binary);
        }

        /* index addressing: arr[k]*/    
        else if (strstr(operand1, "[") && strstr(operand1, "]")) {
            char* arrayName;
            char* tempOperand = strdup(operand1); /* create a copy of operand1 to avoid modifying the original string */
            int symbolIndex;
            int arrAddress;
            char binAddress[15];
            char binIndex[15]; /* 14 bits plus null terminator */
            int i;
            char* indexStr;
            int index;

            /* extract the array name */
            arrayName = strtok(tempOperand, "[");
            symbolIndex = findSymbol(arrayName);
            arrAddress = symbolTable[symbolIndex].value - instructionCount - 100;

            /* convert the address to a 12 bits binary */
            for (i = 11; i >= 0; i--) {
                binAddress[i] = (arrAddress & 1) ? '1' : '0';
                arrAddress >>= 1;
            }

            /* adding the A,R,E bits for each case possilbe */
            if(strcmp(symbolTable[symbolIndex].identifier, "external")){
                binAddress[12] = '0';
                binAddress[13] = '1';
            } else {
                binAddress[12] = '1';
                binAddress[13] = '0';
            }
            binAddress[14] = '\0'; /* null terminate the string */
            strcpy(instructionTable[IC], binAddress);
            printf("array index operand, added word 1/2:%s\n", binAddress);
            

            /* two words are added so adding the first here */
            IC++;

            free(tempOperand);

            /* getting the index inside the square brackets [index] */
            indexStr = strtok(NULL, "[");
            indexStr = strtok(indexStr, "]");
            /* convert the index to an integer */
            index = atoi(indexStr);

            /* convert to binary... */
            for (i = 11; i >= 0; i--) {
                binIndex[i] = (index & 1) ? '1' : '0';
                index >>= 1;
            }
            /* append '00' to the end of the binary number */
            binIndex[12] = '0';
            binIndex[13] = '0';
            binIndex[14] = '\0'; /* null terminate the string */

            /* adding the index word to the instructionTable */
            strcpy(instructionTable[IC], binIndex);
            printf("array index operand, added word 2/2:%s\n", binIndex);
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
            printf("direct register operand, added word:%s\n", binRegister);
        }
        /* direct addressing: x */
        else {
            int symbolIndex;
            int symbolAddress;
            char binAddress[15];
            int i;

            symbolIndex = findSymbol(operand1);
            symbolAddress = symbolTable[symbolIndex].value - instructionCount - 100;
            

            /* convert the address to a 12 bits binary */
            for (i = 11; i >= 0; i--) {
                binAddress[i] = (symbolAddress & 1) ? '1' : '0';
                symbolAddress >>= 1;
            }

            /* adding the A,R,E bits for each case possilbe */
            if(strcmp(symbolTable[symbolIndex].identifier, "external")){
                binAddress[12] = '0';
                binAddress[13] = '1';
            } else {
                binAddress[12] = '1';
                binAddress[13] = '0';
            }
            binAddress[14] = '\0'; /* null terminate the string */
            strcpy(instructionTable[IC], binAddress);
            printf("direct operand, added word:%s\n", binAddress);
        } 
        IC++;
    } else {
        /* operand1 is null */
        /* do nothing... */
    }

    /* ~~~~~~ second operand ~~~~~~ */
    if(operand2 != NULL) {
        /* immediate addressing #k */  
        if (operand2[0] == '#') {
            /* TODO: check for mdefines variables if its not a nunber */
            int num = atoi(operand2 + 1); /* get the number after the # */
            char binary[15];
            int i;

            for (i = 11; i >= 0; i--) {
                binary[i] = (num & 1) ? '1' : '0';
                num >>= 1;
            }
            /* add ARE bits */
            binary[12] = '0';
            binary[13] = '0';
            binary[14] = '\0'; /* null terminate the string */

            /* add to instructionTable */
            strcpy(instructionTable[IC], binary);
            printf("immediate operand, added word:%s\n", binary);
        }

        /* index addressing: arr[k]*/    
        else if (strstr(operand2, "[") && strstr(operand2, "]")) {
            char* arrayName;
            char* tempOperand = strdup(operand2); /* create a copy of operand2 to avoid modifying the original string */
            int symbolIndex;
            int arrAddress;
            char binAddress[15];
            int i;
            char* indexStr;
            int index;
            char binIndex[15]; /* 14 bits plus null terminator */

            /* extract the array name */
            arrayName = strtok(tempOperand, "[");
            symbolIndex = findSymbol(arrayName);
            arrAddress = symbolTable[symbolIndex].value - instructionCount - 100;

            /* convert the address to a 12 bits binary */
            for (i = 11; i >= 0; i--) {
                binAddress[i] = (arrAddress & 1) ? '1' : '0';
                arrAddress >>= 1;
            }

            /* adding the A,R,E bits for each case possilbe */
            if(strcmp(symbolTable[symbolIndex].identifier, "external")){
                binAddress[12] = '0';
                binAddress[13] = '1';
            } else {
                binAddress[12] = '1';
                binAddress[13] = '0';
            }
            binAddress[14] = '\0'; /* null terminate the string */
            strcpy(instructionTable[IC], binAddress);
            printf("array index operand, added word 1/2:%s\n", binAddress);
            
            /* two words are added so adding the first here */
            IC++;

            free(tempOperand);

            /* extract the index */

            /* getting the index inside the square brackets [index] */
            indexStr = strtok(NULL, "[");
            indexStr = strtok(indexStr, "]");
            /* convert the index to an integer */
            index = atoi(indexStr);

            /* convert to binary... */
            for (i = 11; i >= 0; i--) {
                binIndex[i] = (index & 1) ? '1' : '0';
                index >>= 1;
            }
            /* append '00' to the end of the binary number */
            binIndex[12] = '0';
            binIndex[13] = '0';
            binIndex[14] = '\0'; /* null terminate the string */

            /* adding the index word to the instructionTable */
            strcpy(instructionTable[IC], binIndex);
            printf("array index operand, added word 2/2:%s\n", binIndex);
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
            printf("direct register operand, added word:%s\n", binRegister);
        }
        /* direct addressing: x */
        else {
            int symbolIndex;
            int symbolAddress;
            char binAddress[15];
            int i;

            symbolIndex = findSymbol(operand2);
            symbolAddress = symbolTable[symbolIndex].value - instructionCount - 100;
            

            /* convert the address to a 12 bits binary */
            for (i = 11; i >= 0; i--) {
                binAddress[i] = (symbolAddress & 1) ? '1' : '0';
                symbolAddress >>= 1;
            }

            /* adding the A,R,E bits for each case possilbe */
            if(strcmp(symbolTable[symbolIndex].identifier, "external")){
                binAddress[12] = '0';
                binAddress[13] = '1';
            } else {
                binAddress[12] = '1';
                binAddress[13] = '0';
            }
            binAddress[14] = '\0'; /* null terminate the string */
            strcpy(instructionTable[IC], binAddress);
            printf("direct operand, added word:%s\n", binAddress);
        } 
        IC++;
    } else {
        /* operand2 is null */
        /* do nothing... */
    }
}

void scanAgainAndAddWords(FILE *file, Symbol *symbolTable) {
    char line[MAX_LINE_LENGTH];

    printf("~~~~~~~~ SCOND SCAN! ~~~~~~~~\n");

    while (fgets(line, sizeof(line), file)) {
        char *colonPosition;
        printf("\n------------------------\n");
        printf("%s\n", line);

        if (line[0] == ';') {
            continue;
        }

        if (strncmp(line, ".extern", 7) == 0){
            printf("extern line, ignoring\n");
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
                printf("Changing symbol %s identifier to entry\n", symbolName);
            }
            continue;
        } else if (strncmp(line, ".define", 7) == 0) {
            printf("define line, ignoring\n");
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
                printf("data directive, ignoring\n");
            }
            /* .string */
            else if (strlen(directiveStart) >= 8 && strncmp(directiveStart, ".string ", 8) == 0) {
                printf("string directive, ignoring\n");
            }
            /* code */
            else {
                char *instructionStart;
                /* Extract the part of the line after the ":" */
                instructionStart = colonPosition + 1;
                /* Skip any leading spaces or tabs */
                instructionStart += strspn(instructionStart, " \t");
                addAdditionalWords(instructionStart);
                return;
            }
        }
        /* independant operation */
        else {
            printf("independant operation, adding words...\n");
            addAdditionalWords(line);
            return;
        }
    }
    printAll();
}
