#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pre_assembler.h"
#include "first_scan.h"
#include "global.h"

int instructionCount = 0;
Symbol symbolTable[MAX_SYMBOLS];
int* dataArray;
int dataArraySize; 

int main(int argc, char *argv[]) {
    FILE *input_file;
    FILE *output_file;
    char *input_filename;
    char *output_filename;
    char *copy_input_filename = strdup(argv[1]);

    /* allocating memory for data array and initializng dataArraySize to its size */
    dataArray = (int*) malloc(80 * sizeof(int));
    dataArraySize = 80;

    if (argc != 2) {
        printf("Usage: %s input_file.as\n", argv[0]);
        return 1;
    }

    /* creating the string for the .am output file */

    input_filename = strtok(copy_input_filename, ".");
    output_filename = malloc(strlen(input_filename) + strlen(".am") + 1);
    strcpy(output_filename, input_filename);
    strcat(output_filename, ".am");
    printf("output_filename: %s \n", output_filename);
    printf("input_filename: %s \n", input_filename);

    input_file = fopen(argv[1], "r");
    output_file = fopen(output_filename, "w");

    /* checking wether the files opened succefully */
    if (input_file == NULL) {
        printf("Error: Unable to open input file %s.\n", argv[1]);
        free(copy_input_filename);
        free(output_filename);
        return 1;
    }
    if (output_file == NULL) {
        printf("Error: Unable to open output file %s.\n", output_filename);
        free(copy_input_filename);
        free(output_filename);
        return 1;
    }
    free(copy_input_filename);
    

    /* Expand macros in the input file and write the result to the output file */
    expand_macros(input_file, output_file);

    free(dataArray);

    /* close files */
    fclose(input_file);
    fclose(output_file);

    /* Reopen the output file in read mode */
    output_file = fopen(output_filename, "r");
    if (output_file == NULL) {
        printf("Error: Unable to open output file %s.\n", output_filename);
        return 1;
    }
    
    free(output_filename);
    scanSymbolsAllocateWords(output_file, symbolTable);

    return 0;
}
