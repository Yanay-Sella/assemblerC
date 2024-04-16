#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "make_object.h"
#include "first_scan.h" /* for dataCount */

char* binaryToBase4(char* binary) {
    int length = strlen(binary);
    char* base4 = malloc(length / 2 + 1);
    int i, j = 0;

    /* If the length is odd, handle the first digit separately */
    if (length % 2 != 0) {
        base4[j++] = binary[0] == '1' ? '1' : '0';
        binary++;
    }

    /* Convert each pair of binary digits to a base 4 digit */
    for (i = 0; binary[i] != '\0'; i += 2) {
        if (binary[i] == '0' && binary[i+1] == '0') base4[j++] = '*';
        else if (binary[i] == '0' && binary[i+1] == '1') base4[j++] = '#';
        else if (binary[i] == '1' && binary[i+1] == '0') base4[j++] = '%';
        else if (binary[i] == '1' && binary[i+1] == '1') base4[j++] = '!';
    }

    base4[j] = '\0'; /* Null-terminate the string */
    return base4;
}

void writeObject(FILE* ob_file) {
    int i;
    /* Check if the file is open */
    if (ob_file == NULL) {
        printf("Error: File is not open.\n");
        return;
    }

    fprintf(ob_file, "   %d\t%d\n", instructionCount - dataCount, dataCount);
    for (i = 0; i < instructionCount; i++) {
        char* base4 = binaryToBase4(instructionTable[i]);
        if(i+100<1000) fprintf(ob_file, "0%d\t%s\n", i+100, base4);
        else fprintf(ob_file, "%d\t%s\n", i+100, base4);
        free(base4);
    }
}
