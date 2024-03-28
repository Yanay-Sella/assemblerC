#include <stdio.h>
#include "global.h"

void scanSymbols(FILE *file, Symbol *symbolTable) {
    char line[MAX_LINE_LENGTH];
    int address = 0;
    int symbolCount = 0;

    while (fgets(line, sizeof(line), file)) {
        char symbolName[MAX_LINE_LENGTH];
        int value;
        int foundSymbol = 0;

        /* Parse the line according to a .define line */
        if (strncmp(line, ".define", 7) == 0) {
            foundSymbol = 1;
            sscanf(line, ".define %s = %d", symbolName, &value);
        }

        /* saving the pointer for the ":" in the line */
        char *colonPosition = strchr(line, ':');
        /* if its a symbol defenition like "SYMBOL: ____" */
        if (colonPosition) {
            foundSymbol = 1;
            /* copying the symbol name from the line */
            strncpy(symbolName, line, colonPosition - line);
            symbolName[colonPosition - line] = '\0';

            // TODO: Parse the rest of the line to determine the value and identifier
        }

        if(foundSymbol == 1){
            /* Check if the symbol already exists in the symbol table */
            int i;
            for (i = 0; i < symbolCount; i++) {
                if (strcmp(symbolTable[i].symbolname, symbolName) == 0) {
                    /* Symbol already exists, handle as needed */
                    printf("Error: Symbol %s already defined.\n", symbolName);
                    return;
                }
            }

            /* Add the symbol to the symbol table */ 
            strcpy(symbolTable[symbolCount].symbolname, symbolName);
            symbolTable[symbolCount].value = value;
            strcpy(symbolTable[symbolCount].identifier, "mdefine");

            symbolCount++;
        }
    }
}