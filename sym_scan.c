#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

int dataCount = 0;

/* add data to dataArray */
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

/* find symbol value from symbol table by symbol name */
int findMDefine(char* symbolName) {
    /* Iterate over the symbol table */
    for (int i = 0; i < dataCount; i++) {
        /* If the symbol name matches and its identifier is "mdefine", return its value */
        if (strcmp(symbolTable[i].name, symbolName) == 0 && strcmp(symbolTable[i].identifier, "mdefine") == 0) {
            return symbolTable[i].value;
        }
    }

    // If the symbol was not found or its identifier is not "mdefine", return an error value or handle the error appropriately
    return -1;
}

void scanSymbols(FILE *file, Symbol *symbolTable) {
    char line[MAX_LINE_LENGTH];
    int address = 0;
    int instCound = 0;

    while (fgets(line, sizeof(line), file)) {
        /* symbol properties */
        char symbolName[MAX_LINE_LENGTH];
        char symbolIdentifier[10];
        int symbolValue;
        /* found symbol flag */
        int foundSymbol = 0;

        /* Parse the line according to a .define line */
        if (strncmp(line, ".define", 7) == 0) {
            foundSymbol = 1;
            strcpy(symbolIdentifier, "mdefine");
            sscanf(line, ".define %s = %d", symbolName, &symbolValue);
        }

        /* saving the pointer for the ":" in the line if exists */
        char *colonPosition = strchr(line, ':');
        /* if its a symbol defenition like "SYMBOL: ____" */
        if (colonPosition) {
            foundSymbol = 1;

            /* copying the symbol name from the line */
            strncpy(symbolName, line, colonPosition - line);
            symbolName[colonPosition - line] = '\0';

            /* move the pointer to the start of the directive */
            char *directiveStart = colonPosition + 1;

            /* check for .data or .string directive */
            if (strncmp(directiveStart, ".data ", 6) == 0 || strncmp(directiveStart, ".string ", 8) == 0) {
                strcpy(symbolIdentifier, "data");
                symbolValue = dataCount; /* saving the data address */

                if(strncmp(directiveStart, ".data ", 6) == 0){
                    char* pointer = strtok(directiveStart + 6, ",");
                    while (pointer != NULL) {
                        int value;
                        if (sscanf(pointer, "%d", &value) == 1) {
                            // pointer is a number
                            addData(value);
                        } else {
                            // pointer is a symbol
                            int definedVar = findMDefine(pointer);
                            addData(definedVar);
                        }
                        pointer = strtok(NULL, ",");
                    }
                }
                if(strncmp(directiveStart, ".string ", 8) == 0){

                }
            }

            /* check for .extern or .entry directive */
            else if (strncmp(directiveStart, ".extern ", 8) == 0 || strncmp(directiveStart, ".entry ", 7) == 0) {
                // TODO: Handle .extern or .entry directive
            }
        }

        /* TODO: maybe do this only to mdefine, need to think */
        if(foundSymbol == 1){
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

            /* increase DC for the next symbol */
            dataCount++;
        }
    }
}