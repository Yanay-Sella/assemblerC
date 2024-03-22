#include <stdio.h>
#include <string.h>
#include "pre_assembler.h" // Include the corresponding header file

// Function to expand macros in the assembly code
void expand_macros(FILE *input_file, FILE *output_file) {
    char line[MAX_LINE_LENGTH];
    char label[MAX_LINE_LENGTH];
    char opcode[MAX_LINE_LENGTH];
    char operand[MAX_LINE_LENGTH];

    // Read each line from the input file
    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Check if the line is a macro definition
        if (strstr(line, "#define") != NULL) {
            // Ignore macro definitions
            continue;
        }

        // Parse the line into label, opcode, and operand
        if (sscanf(line, "%s %s %s", label, opcode, operand) == 3) {
            // Check if the opcode is a macro
            if (strcmp(opcode, "MACRO") == 0) {
                // Expand the macro by replacing occurrences in the code
                char macro_name[MAX_LINE_LENGTH];
                sscanf(operand, "%s", macro_name); // Get the macro name
                // Read and ignore the macro definition
                while (fgets(line, sizeof(line), input_file) != NULL) {
                    if (strstr(line, "ENDMACRO") != NULL)
                        break;
                }
            } else {
                // Write the line to the output file
                fprintf(output_file, "%s %s %s", label, opcode, operand);
            }
        } else {
            // Write the line to the output file (no label)
            fprintf(output_file, "%s", line);
        }
    }
}