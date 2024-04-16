#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "make_var_files.h"

void createEntryFile(char* fileName) {
    FILE *ent_file;
    int i;
    int hasEntries = 0;

    /* check if there are any entries in the symbolTable */
    for (i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].identifier, "entry") == 0) {
            hasEntries = 1;
            break;
        }
    }

    /* if there are no entries, return without creating a file */
    if (!hasEntries) return;

    ent_file = fopen(fileName, "w");
    if(ent_file == NULL) {
        printf("Error: Unable to open output file %s.\n" ,fileName);
        exit(1);
    }

    /* write the entries to the file */
    for (i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].identifier, "entry") == 0) {
            if(symbolTable[i].value < 1000) fprintf(ent_file, "%s\t0%d\n", symbolTable[i].name, symbolTable[i].value);
            else fprintf(ent_file, "%s\t%d\n", symbolTable[i].name, symbolTable[i].value);
        }
    }
}

void createExternFile(char* fileName) {
    FILE *ext_file;
    int i,j;
    int hasExterns = 0;

    /* check if there are any entries in the symbolTable */
    for (i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].identifier, "external") == 0) {
            hasExterns = 1;
            break;
        }
    }

    /* if there are no entries, return without creating a file */
    if (!hasExterns) return;

    ext_file = fopen(fileName, "w");
    if(ext_file == NULL) {
        printf("Error: Unable to open output file %s.\n" ,fileName);
        exit(1);
    }

    /* write the externs to the file */
    for (i = 0; i < externUsageCount; i++) {
        for (j = 0; j < MAX_USAGE; j++) {
            if (externUsageTable[i].locations[j] != 0) {
                int location = externUsageTable[i].locations[j]+100;
                if(location < 1000) fprintf(ext_file, "%s\t0%d\n", externUsageTable[i].symbolName, location);
                else fprintf(ext_file, "%s\t%d\n", externUsageTable[i].symbolName, location);
            }
        }
    }

    fclose(ext_file); 
}
