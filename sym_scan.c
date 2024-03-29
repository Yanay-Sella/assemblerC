#include <stdio.h>
#include "global.h"

void scanSymbols(FILE *file, Symbol *symbolTable) {
    char line[MAX_LINE_LENGTH];
    int address = 0;
    int dataCount = 0;
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
            strncpy(symbolIdentifier, "mdefine");
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
                symbolValue = dataCount;
            }

            /* check for .extern or .entry directive */
            else if (strncmp(directiveStart, ".extern ", 8) == 0 || strncmp(directiveStart, ".entry ", 7) == 0) {
                // TODO: Handle .extern or .entry directive
            }
        }

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