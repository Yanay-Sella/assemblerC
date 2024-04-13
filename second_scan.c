#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

void addAdditionalWords(char* instructionLine) {

}

void scanAgainAndAddWords(FILE *file, Symbol *symbolTable) {
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        char *colonPosition;

        if (strncmp(line, ".extern", 7) == 0){

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
                addAdditionalWords(line);
            }
        }
        /* independant operation */
        else {
            addAdditionalWords(line);
        }
    }
}