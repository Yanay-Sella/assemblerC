#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pre_assembler.h"
#include "first_scan.h"
#include "second_scan.h"
#include "make_object.h"
#include "make_var_files.h"
#include "global.h"


int instructionCount = 0;
int externUsageCount = 0;
int isError = 0;

Symbol symbolTable[MAX_SYMBOLS];
int* dataArray;
int dataArraySize; 

int main(int argc, char *argv[]) {
    FILE *input_file;
    FILE *am_file;
    FILE *ob_file;
    char *input_filename;
    char *amFileName;
    char *obFileName;
    char *entFileName;
    char *extFileName;

    char *copy_input_filename = malloc(strlen(argv[1]) + 1);
    if (copy_input_filename == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        exit(1);
    }
    memcpy(copy_input_filename, argv[1], strlen(argv[1]) + 1);

    /* allocating memory for data array and initializng dataArraySize to its size */
    dataArray = (int*) malloc(80 * sizeof(int));
    dataArraySize = 80;

    if (argc != 2) {
        printf("Usage: %s input_file.as\n", argv[0]);
        exit(1);
    }

    /* creating the string for the .am output file */

    input_filename = strtok(copy_input_filename, ".");
    amFileName = malloc(strlen(input_filename) + strlen(".am") + 1);
    obFileName = malloc(strlen(input_filename) + strlen(".ob") +1);
    entFileName = malloc(strlen(input_filename) + strlen(".ent") +1);
    extFileName = malloc(strlen(input_filename) + strlen(".ent") +1);
    strcpy(amFileName, input_filename);
    strcpy(obFileName, input_filename);
    strcpy(entFileName, input_filename);
    strcpy(extFileName, input_filename);
    strcat(amFileName, ".am");
    strcat(obFileName, ".ob");
    strcat(entFileName, ".ent");
    strcat(extFileName, ".ext");

    input_file = fopen(argv[1], "r");
    am_file = fopen(amFileName, "w");

    /* checking wether the files opened succefully */
    if (input_file == NULL) {
        printf("Error: Unable to open input file %s.\n", argv[1]);
        free(copy_input_filename);
        free(amFileName);
        exit(1);
    }
    if (am_file == NULL) {
        printf("Error: Unable to open output file %s.\n", amFileName);
        free(copy_input_filename);
        free(amFileName);
        exit(1);
    }
    free(copy_input_filename);
    

    /* Expand macros in the input file and write the result to the am_file */
    expand_macros(input_file, am_file);

    free(dataArray);

    /* close files */
    fclose(input_file);
    fclose(am_file);

    /* Reopen the .am file in read mode */
    am_file = fopen(amFileName, "r");
    if (am_file == NULL) {
        printf("Error: Unable to open output file %s.\n", amFileName);
        exit(1);
    }
    
    free(amFileName);
    scanSymbolsAllocateWords(am_file, symbolTable);

    /* Reset the file pointer to the start of the file */
    rewind(am_file);
    
    scanAgainAndAddWords(am_file , symbolTable);

    ob_file = fopen(obFileName, "w");
    if(ob_file == NULL) {
        printf("Error: Unable to open output file %s.\n" ,obFileName);
        exit(1);
    }

    writeObject(ob_file);
    /* it will create the file only if there are entries */
    if(isError) {
        printf("Errors found in file %s", input_filename);
        return 1;
    }
    createEntryFile(entFileName);
    createExternFile(extFileName);
    free(copy_input_filename);

    return 0;
}
