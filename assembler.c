#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pre_assembler.h"

#define MAX_LINE_LENGTH 256

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input_file.as output_file.am\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");

    if (input_file == NULL || output_file == NULL) {
        printf("Error: Unable to open files.\n");
        return 1;
    }

    // Expand macros in the input file and write the result to the output file
    expand_macros(input_file, output_file);

    // Close the files
    fclose(input_file);
    fclose(output_file);

    printf("Macro expansion complete.\n");

    return 0;
}
