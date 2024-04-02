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

/* add symbol to the symbolTable */
void addSymbol(char* symbolName, int symbolValue, char* symbolIdentifier) {
    /* check for conflicts in the symbol table */
    int i;
    for (i = 0; i < dataCount; i++) {
        if (strcmp(symbolTable[i].name, symbolName) == 0) {
        printf("Error: Symbol %s already defined.\n", symbolName);
        return;
        }
    }

    /* add the symbol to the symbol table at dataCount */ 
    strcpy(symbolTable[dataCount].name, symbolName);
    symbolTable[dataCount].value = symbolValue;
    strcpy(symbolTable[dataCount].identifier, symbolIdentifier);
}

void scanSymbols(FILE *file, Symbol *symbolTable) {
    char line[MAX_LINE_LENGTH];
    int address = 0;
    int instCount = 0;

    while (fgets(line, sizeof(line), file)) {
        /* symbol properties */
        char symbolName[MAX_LINE_LENGTH];
        char symbolIdentifier[10];
        int symbolValue;
        /* found symbol flag */

        /* Parse the line according to a .define line */
        if (strncmp(line, ".define", 7) == 0) {
            strcpy(symbolIdentifier, "mdefine");
            sscanf(line, ".define %s = %d", symbolName, &symbolValue);
            /* increase DC for the next symbol */
            dataCount++;
            addSymbols(symbolName, symbolValue, symbolIdentifier);
        }

        /* Parse the line according to a .extern line */
        else if (strncmp(line, ".extern", 7) == 0) {
            strcpy(symbolIdentifier, "external");
            sscanf(line, ".extern %s", symbolName);
            /* increase DC for the next symbol */
            dataCount++;
            addSymbol(symbolName, NULL, symbolIdentifier);  // Extern symbols don't have a value
        }

        /* Parse the line according to a .entry line */
        else if (strncmp(line, ".entry", 6) == 0) {
            strcpy(symbolIdentifier, "entry");
            sscanf(line, ".entry %s", symbolName);
            /* increase DC for the next symbol */
            dataCount++;
            addSymbol(symbolName, NULL, symbolIdentifier);  // Entry symbols don't have a value
        }

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
                            // pointer on a number
                            addData(value);
                        } else {
                            // pointer on a symbol
                            int definedVar = findMDefine(pointer);
                            addData(definedVar);
                        }
                        pointer = strtok(NULL, ",");
                    }
                }

                /* String */
                if(strncmp(directiveStart, ".string ", 8) == 0){
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
                    symbolValue = instCount + 100;
                    /* increase DC for the next symbol */
                    dataCount++;
                    addSymbol(symbolName, symbolValue, symbolIdentifier);
                }
            }
        }
        instCount ++;
    }
}