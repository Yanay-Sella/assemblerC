#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

int dataCount = 0;

/* find symbol value from symbol table by symbol name */
int findMDefine(char* symbolName) {
    /* Iterate over the symbol table */
    for (int i = 0; i < dataCount; i++) {
        /* If the symbol name matches and its identifier is "mdefine", return its value */
        if (strcmp(symbolTable[i].name, symbolName) == 0 && strcmp(symbolTable[i].identifier, "mdefine") == 0) {
            return symbolTable[i].value;
        }
    }
    /* if the symbol was not found or its identifier is not "mdefine" */
    printf("Error: Symbol %s not found or not an mdefine.\n", symbolName);
    exit(1);
}

/* add data to the dataArray */
void addData(int value) {
    /* check if dataArray is full and reallocate more memory if needed */
    if (dataCount >= currentSize) {
        /* double the size */
        currentSize *= 2;
        dataArray = (int*) realloc(dataArray, currentSize * sizeof(int));
    }

    /* add the value to dataArray and increment dataCount */
    dataArray[dataCount] = value;
    dataCount++;
}

int findSymbol(char* symbolName) {
    int i;
    for (i = 0; i < dataCount; i++) {
        if (strcmp(symbolTable[i].name, symbolName) == 0) {
            return i;
        }
    }
    /* symbol not found */
    return -1;
}

/* add symbol to the symbolTable */
void addSymbol(char* symbolName, int symbolValue, char* symbolIdentifier) {
    /* check for conflicts in the symbol table */
    if(findSymbol >= 0 ){
        printf("Error: Symbol %s already defined.\n", symbolName);
        return;
    }

    /* add the symbol to the symbol table at dataCount */ 
    strcpy(symbolTable[dataCount].name, symbolName);
    symbolTable[dataCount].value = symbolValue;
    strcpy(symbolTable[dataCount].identifier, symbolIdentifier);
}

void addInstructionFirstWord(char* instructionLine) {
    char* operation;
    char* operand1;
    char* operand2;
    int i; 
    int found = 0;
    char resultWord[WORD_LENGTH] = "0000"; /* starting with the four not-used bits */
    int additionalWords = 0;

    /* split the instructionLine into tokens */
    operation = strtok(instructionLine, ":");
    operand1 = strtok(NULL, ",");
    operand2 = strtok(NULL, ",");

    /* iterate over the operations array */
    for (i = 0; i < sizeof(operations) / sizeof(operations[0]); i++) {
        if (strcmp(operations[i].name, operation) == 0) {
            strcat(resultWord, operations[i].opcode);
            found = 1;
            break;
        }
    }

    /* calculate additional words based on operands */
    if (operand1[0] == 'r' && operand2[0] == 'r') {
        /* two register operands can be combined into one word */
        additionalWords = 1;
    } else {
        /* each operand contributes one additional word */
        additionalWords = (operand1 != NULL) + (operand2 != NULL);

        /* array operand contributes an -extra- word */
        if (strstr(operand1, "[") && strstr(operand1, "]")) additionalWords++;
        if (strstr(operand2, "[") && strstr(operand2, "]")) additionalWords++;
    }

    /* ~~~ taking care of the source operand bits 5-4 ~~~ */
    /* immediate addressing */
    if (operand1[0] == '#')
        strcat(resultWord, "00");
    /* index addressing */    
    else if (strstr(operand1, "[") && strstr(operand1, "]"))
        strcat(resultWord, "10");  
    /* direct register addressing */
    else if (operand1[0] == 'r' && isdigit(operand1[1]) && operand1[1] >= '0' && operand1[1] <= '7') 
        strcat(resultWord, "11");  
    /* direct addressing */
    else 
        strcat(resultWord, "01"); 

    /* ~~~ taking care of the target operand bits 3-2 ~~~ */
    /* immediate addressing */
    if (operand2[0] == '#')
        strcat(resultWord, "00");
    /* index addressing */    
    else if (strstr(operand2, "[") && strstr(operand2, "]"))
        strcat(resultWord, "10");  
    /* direct register addressing */
    else if (operand2[0] == 'r' && isdigit(operand2[1]) && operand2[1] >= '0' && operand2[1] <= '7') 
        strcat(resultWord, "11");  
    /* direct addressing */
    else 
        strcat(resultWord, "01"); 


    /* adding the A,R,E bits */
    strcat(resultWord, "00");     

    /* handling errors */
    if (!found) {
        printf("Error: Unknown operation %s.\n", operation);
    } else {
        /* check if there's enough space in the instructionTalbe */
        if (instructionCount + additionalWords >= MAX_INSTRUCTIONS) {
            printf("Error: Too many instructions.\n");
            return;
        }
        /* add the resultWord to the instructionTable */
        strcpy(instructionTable[instructionCount], resultWord);
        instructionCount += additionalWords; /* increment instructionCount by the number of additional words */
    }
}

void scanSymbols(FILE *file, Symbol *symbolTable) {
    char line[MAX_LINE_LENGTH];
    int address = 0;

    while (fgets(line, sizeof(line), file)) {
        /* symbol properties */
        char symbolName[MAX_LINE_LENGTH];
        char symbolIdentifier[10];
        int symbolValue;

        /* Parse the line according to a .define line */
        if (strncmp(line, ".define", 7) == 0) {
            strcpy(symbolIdentifier, "mdefine");
            sscanf(line, ".define %s = %d", symbolName, &symbolValue);
            /* increase DC for the next symbol */
            dataCount++;
            addSymbols(symbolName, symbolValue, symbolIdentifier);
            continue;
        }

        /* Parse the line according to a .extern line */
        else if (strncmp(line, ".extern", 7) == 0) {
            strcpy(symbolIdentifier, "external");
            sscanf(line, ".extern %s", symbolName);
            /* increase DC for the next symbol */
            dataCount++;
            addSymbol(symbolName, NULL, symbolIdentifier);  // Extern symbols don't have a value
            continue;
        }

        /* Parse the line according to a .entry line */
        else if (strncmp(line, ".entry", 6) == 0) {
            strcpy(symbolIdentifier, "entry");
            sscanf(line, ".entry %s", symbolName);
            /* increase DC for the next symbol */
            dataCount++;
            addSymbol(symbolName, NULL, symbolIdentifier);  // Entry symbols don't have a value
            continue;
        }

        /* ~~~ intructions ~~~ */

        /* if its a symbol defenition like "SYMBOL: ____" */
        /* saving the pointer for the ":" in the line if exists */
        char *colonPosition = strchr(line, ':');
        if (colonPosition) {

            /* copying the symbol name from the line */
            strncpy(symbolName, line, colonPosition - line);
            symbolName[colonPosition - line] = '\0';

            /* move the pointer to the start of the directive */
            char *directiveStart = colonPosition + 1;

            /* check for .data or .string directive */
            if (strncmp(directiveStart, ".data ", 6) == 0 || strncmp(directiveStart, ".string ", 8) == 0) {
                strcpy(symbolIdentifier, "data");
                symbolValue = dataCount; /* saving the data starting address */

                /* adding the actual data */

                /* Array */
                if(strncmp(directiveStart, ".data ", 6) == 0){
                    /* skip to the first number of the array */
                    char* pointer = strtok(directiveStart + 6, ",");
                    while (pointer != NULL) {
                        int value;
                        if (sscanf(pointer, "%d", &value) == 1) {
                            /* pointer on a number */
                            addData(value);
                        } else {
                            /* pointer on a symbol */
                            int definedVar = findMDefine(pointer);
                            addData(definedVar);
                            /* TODO: add the instruction to the instructionTable */
                        }
                        pointer = strtok(NULL, ",");
                    }
                }

                /* String */
                else if(strncmp(directiveStart, ".string ", 8) == 0){
                    /* skip to the first character after the " */
                    char* pointer = strchr(directiveStart, '\"');
                    if (pointer == NULL) {
                        printf("Error: missing starting quote in .string directive\n");
                        exit(1);
                    }
                    pointer++;  /* Skip past the opening quote */

                    /* Iterate over each character in the string */
                    while (*pointer != '\"') {
                        /* add a character to the dataArray */
                        addData((int)*pointer);
                        
                        /* TODO: add the instruction to the instructionTable */

                        /* Move to the next character */
                        pointer++;
                        if (*pointer == '\0') {
                            printf("Error: missing closing quote in .string directive\n");
                            exit(1);
                        }
                    }
                }

                /* Code */
                else {
                    strcpy(symbolIdentifier, "code");
                    symbolValue = instructionCount + 100;
                    /* increase DC for the next symbol */
                    dataCount++;
                    addSymbol(symbolName, symbolValue, symbolIdentifier);

                    /* Extract the part of the line after the ":" */
                    char *instructionStart = colonPosition + 1;

                    /* Skip any leading spaces or tabs */
                    instructionStart += strspn(instructionStart, " \t");

                    /* Pass the instruction to the addInstructionFirstWord function */
                    addInstructionFirstWord(instructionStart);
                }
            }
        }



        instructionCount ++;
    }
}