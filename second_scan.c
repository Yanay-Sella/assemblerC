#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "global.h"

int IC = 0;

void addAdditionalWords(char* instructionLine) {
    char* operation;
    char* operand1;
    char* operand2;
    char newWord[WORD_LENGTH] = ""; /* starting with the four not-used bits */
    InstructionParts parts;
    
    IC++;
    parts = splitInstruction(instructionLine);
    operation = parts.operation;
    operand1 = parts.operand1;
    operand2 = parts.operand2;

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
            IC++;
        }

        /* index addressing: arr[k]*/    
        else if (strstr(operand1, "[") && strstr(operand1, "]")) {
            char* arrayName;
            char* tempOperand = strdup(operand1); /* create a copy of operand1 to avoid modifying the original string */
            int symbolIndex;
            int arrAddress;
            char binAddress[15];
            int i;

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
            IC++;

            free(tempOperand);

            /* extract the index */
            char* indexStr;
            int index;

            /* getting the index inside the square brackets [index] */
            indexStr = strtok(NULL, "[");
            indexStr = strtok(indexStr, "]");
            /* convert the index to an integer */
            index = atoi(indexStr);

            /* convert the index to a 12-bit binary number */
            char binIndex[15]; /* 14 bits plus null terminator */
            int i;

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
            IC++;
        }
        /* TODO: finish with those two cases */
        /* direct register addressing: r4 */
        else if (operand1[0] == 'r' && isdigit(operand1[1]) && operand1[1] >= '0' && operand1[1] <= '7') 
            strcat(newWord, "11");  
        /* direct addressing: x */
        else 
            strcat(newWord, "01"); 
    } else {
        /* operand1 is null */
        strcat(newWord, "00");
    }

    /* TODO: finish the second operand, should be similar to the first */
    /* ~~~ taking care of the target operand bits 3-2 ~~~ */
    /* immediate addressing */
    if(operand2 != NULL) {
        if (operand2[0] == '#')
            strcat(newWord, "00");
        /* index addressing */    
        else if (strstr(operand2, "[") && strstr(operand2, "]"))
            strcat(newWord, "10");  
        /* direct register addressing */
        else if (operand2[0] == 'r' && isdigit(operand2[1]) && operand2[1] >= '0' && operand2[1] <= '7') 
            strcat(newWord, "11");  
        /* direct addressing */
        else 
            strcat(newWord, "01");
    } else {
        /* operand2 is null */
        strcat(newWord, "00");
    }
    /* adding the A,R,E bits */
    strcat(newWord, "00");     
    printf("word:%s\n", newWord);
}

void scanAgainAndAddWords(FILE *file, Symbol *symbolTable) {
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        char *colonPosition;

        if (strncmp(line, ".extern", 7) == 0){
            /* do nothing... */
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
        }

        /* if its a symbol defenition like "SYMBOL: ____" */
        /* saving the pointer for the ":" in the line if exists */
        colonPosition = strchr(line, ':');
        if (colonPosition) {
            char *directiveStart;
            int length = colonPosition - line;

            /* move the pointer to the start of the directive */
            directiveStart = colonPosition + 1;
            /* Remove leading white spaces */
            while(isspace((unsigned char) *directiveStart)) {
                directiveStart++;
            }
            
            /* skipping .data .string */
            /* .data */
            if (strlen(directiveStart) >= 6 && strncmp(directiveStart, ".data ", 6) == 0) {
                /* do nothing... */
            }
            /* .string */
            else if (strlen(directiveStart) >= 8 && strncmp(directiveStart, ".string ", 8) == 0) {
                /* do nothing... */
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