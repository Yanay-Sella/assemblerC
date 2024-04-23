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

/* a function to free all macros from the macro array */
void free_macros(void) {
    int i,j;
    for (i = 0; i < num_macros; i++) {
        for (j = 0; j < macros[i].num_lines; j++) {
            free(macros[i].lines[j]); /* free the lines themsleves */
        }
        free(macros[i].lines); /* free the lines of the array */
    }
}

/* this function removes any leading and trailing whitespace in a string */
void trim_whitespace(char *str) {
    char *start = str;
    char *end = start + strlen(start) - 1;

    while (*start == ' ' || *start == '\t') {
        start++;
    }
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n')) {
        *end = '\0';
        end--;
    }
    memmove(str, start, end - start + 2); /* +2 to include the null terminator */
}

void replace_macros(char *line, FILE *output_file) {
    int i, j;
    for (i = 0; i < num_macros; i++) {
        if (strncmp(line, macros[i].name, strlen(macros[i].name)) == 0) {
            /* this line is a call to a macro, so write the macro's definition to the output file */
            for (j = 0; j < macros[i].num_lines; j++) {
                fprintf(output_file, "%s\n", macros[i].lines[j]);
            }
            return;
        }
    }
    /* this line is not a call to a macro, so write we it to the output file */
    /* and also check for validity */
    if (fprintf(output_file, "%s\n", line) < 0) {
        printf("ERROR: write to output file failed\n");
        exit(1);
    }
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
        trim_whitespace(line);
        /* remove new line charaters */
        line[strcspn(line, "\n")] = '\0';

        /* check the first three characters for macro defenition */
        if (strncmp(line, "mcr", 3) == 0) {
            /* turning on macro "boolean" */
            in_macro = 1;

            /* our new macro points to the num_macro element in the array */
            current_macro = &macros[num_macros++];

            sscanf(line, "%3s %s", macro_keyword, current_macro->name);
            
            current_macro->num_lines = 0;
        /* check the first 6 character for macro end */
        } else if (strncmp(line, "endmcr", 6) == 0) {
            /* turning off macro "boolean" */
            in_macro = 0;
            current_macro = NULL;
        
        /* check if inside the macro */
        } else if (in_macro == 1) {
            /* reallocate memory in the -array- for a new line */
            char **new_lines = realloc(current_macro->lines, (current_macro->num_lines + 1) * sizeof(char *));
            if (new_lines == NULL) {
                printf("ERROR: memory allocation failed\n");
                exit(1);
            }
            current_macro->lines = new_lines;
            current_macro->lines[current_macro->num_lines] = malloc(strlen(line) + 1);
            if (current_macro->lines[current_macro->num_lines] == NULL) {
                printf("ERROR: memory allocation failed\n");
                exit(1);
            }
            strcpy(current_macro->lines[current_macro->num_lines], line);
            current_macro->num_lines++;
        } else {
            /* replace the macros or write original line */
            replace_macros(line, output_file);
        }
    }
    free_macros();
}
