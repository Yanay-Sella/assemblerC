#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "global.h"

int findSymbol(char* symbolName) {
    int i;
    int len;

    /* remove newline at the end of symbolName */
    len = strlen(symbolName);
    if (len > 0 && symbolName[len-1] == '\n') {
        symbolName[len-1] = '\0';
    }

    for (i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, symbolName) == 0) {
            return i;
        }
    }
    /* symbol not found */
    return -1;
}

InstructionParts splitInstruction(char* instructionLine) {
    InstructionParts parts;
    char* operands;
    int len;

    /* split the instructionLine into tokens */
    strcpy(parts.operation, strtok(instructionLine, " "));
    operands = strtok(NULL, "");

    if (operands != NULL) {
        parts.operand1 = strtok(operands, ",");
        operands = strtok(NULL, "");
        if (operands != NULL) {
            parts.operand2 = strtok(operands, ",");
        } else {
            parts.operand2 = NULL;
        }
    } else {
        parts.operand1 = NULL;
        parts.operand2 = NULL;
    }

    /* remove trailing newline and white spaces from operation */
    len = strlen(parts.operation);
    while (len > 0 && isspace((unsigned char) parts.operation[len-1])) {
        len--;
    }
    parts.operation[len] = '\0';

    return parts;
}
