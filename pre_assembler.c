#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pre_assembler.h" /* pre_assembler header file */
#include "global.h" /* global variables and macros if needed */

typedef struct Macro {
    char name[MAX_LINE_LENGTH];
    char **lines;
    int num_lines;
} Macro;

/* Define an array to store macros */
Macro macros[MAX_MACROS];
int num_macros = 0;

void free_macros(void) {
    int i,j;
    for (i = 0; i < num_macros; i++) {
        for (j = 0; j < macros[i].num_lines; j++) {
            free(macros[i].lines[j]);
        }
        free(macros[i].name);
    }
}

void replace_macros(char* line) {
    
}

void expand_macros(FILE *input_file, FILE *output_file) {
    char line[MAX_LINE_LENGTH];
    
    /* 
    defining macro settings:
    in_macro: a binary to determine whether the program is inside macro defenition
    current_macro: a pointer to 
    */
    int in_macro = 0;
    Macro* current_macro = NULL;

    /* read each line from the input file */
    while (fgets(line, sizeof(line), input_file) != NULL) {
        char macro_keyword[4];

        /* remove new line charaters */
        line[strcspn(line, "\n")] = '\0';

        /* check the first three characters for macro defenition */
        if (strncmp(line, "mcr", 3) == 0) {
            /* turning on macro "boolean" */
            in_macro = 1;

            /* our new macro points to the num_macro element in the array */
            current_macro = &macros[num_macros++];

            if (sscanf(line, "%3s %s", macro_keyword, current_macro->name) == 2) {
                if(strcmp(macro_keyword, "mcr") == 0){
                    /* this is a macro defenition */
                }
            }
            current_macro->num_lines = 0;
        /* check the first 6 character for macro end */
        } else if (strncmp(line, "endmcr", 6) == 0) {
            /* turning off macro "boolean" */
            in_macro = 0;
            current_macro = NULL;
        
        /* check if inside the macro */
        } else if (in_macro == 1) {
            /* add the line to the current macro that point to the array */
            /* strdup also allocates memory which should be freed */
            current_macro->lines[current_macro->num_lines++] = strdup(line);
        } else {
            /* replace any macros in the line */
            replace_macros(line);
            /* write the processed line to the output file */
            fprintf(output_file, "%s\n", line);
        }
    }
}
