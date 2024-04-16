#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "first_scan.h"
#include "global.h"

int dataCount = 0;
int symbolCount = 0;

/* find symbol value from symbol table by symbol name */
int findMDefine(char* symbolName) {
    int i;
    int len;

    /* Remove leading white spaces */
    while(isspace((unsigned char) *symbolName)) {
        symbolName++;
    }

    /* Remove trailing newline and white spaces */
    len = strlen(symbolName);
    while (len > 0 && isspace((unsigned char) symbolName[len-1])) {
        symbolName[len-1] = '\0';
        len--;
    }
    
    /* Iterate over the symbol table */
    for (i = 0; i < symbolCount; i++) {
        /* If the symbol name matches and its identifier is "mdefine", return its value */
        if (strcmp(symbolTable[i].name, symbolName) == 0 && strcmp(symbolTable[i].identifier, "mdefine") == 0) {
            return symbolTable[i].value;
        }
    }
    /* if the symbol was not found or its identifier is not "mdefine" */
    printf("Error: Symbol %s not found or not an mdefine.\n", symbolName);
    isError = 1;
    return -1;
}

/* add data to the dataArray */
void addData(int value) {
    /* check if dataArray is full and reallocate more memory if needed */
    if (dataCount >= dataArraySize) {
        /* double the size */
        dataArraySize *= 2;
        dataArray = (int*) realloc(dataArray, dataArraySize * sizeof(int));
    }

    /* add the value to dataArray and increment dataCount 
    keeping the value at the dataCount cell to be an address to the start of the array/string*/
    dataArray[dataCount] = value;
    dataCount++;
}

/* add symbol to the symbolTable */
void addSymbol(char* symbolName, int symbolValue, char* symbolIdentifier) {
    
    int symbolIndex = findSymbol(symbolName);
    /* check for conflicts in the symbol table 
    entry symbols can be added with onflicts, and vice versa */
    if(symbolIndex >= 0 && strcmp(symbolTable[symbolIndex].identifier, "entry") != 0 && strcmp(symbolIdentifier, "entry") != 0){
        printf("Error: Symbol %s already defined.\n", symbolName);
        isError = 1;
        return;
    }

    /* add the symbol to the symbol table at symbolCount */ 
    strcpy(symbolTable[symbolCount].name, symbolName);
    symbolTable[symbolCount].value = symbolValue;
    strcpy(symbolTable[symbolCount].identifier, symbolIdentifier);
    symbolCount++;
}

/* the function recieves the number and a string reference for the function to insert the result */
void toBinary(int num, char *binary) {
    int i;
    for (i = 13; i >= 0; i--) {
        binary[13 - i] = ((num >> i) & 1) ? '1' : '0';
    }
    binary[14] = '\0';  /* Null-terminate the string */
}

void addInstructionFirstWord(char* instructionLine) {
    char* operation;
    char* operand1;
    char* operand2;
    int i; 
    int found = 0;
    char resultWord[WORD_LENGTH] = "0000"; /* starting with the four not-used bits */
    int additionalWords = 0;
    InstructionParts parts;

    /* split the instruction into parts */
    parts = splitInstruction(instructionLine);
    operation = parts.operation;
    operand1 = parts.operand1;
    operand2 = parts.operand2;

    /* switching roles */
    if(operand2 == NULL && operand1 != NULL) {
        operand2 = operand1;
        operand1 = NULL;
    }

    /* remove leading spaces from both operands */
    while(operand1 != NULL && isspace((unsigned char)*operand1)) operand1++;
    while(operand2 != NULL && isspace((unsigned char)*operand2)) operand2++;

    /* iterate over the operations array checking operation validity */
    for (i = 0; i < sizeof(operations) / sizeof(operations[0]); i++) {
        if (strcmp(operations[i].name, operation) == 0) {
            strcat(resultWord, operations[i].opcode);
            found = 1;
            break;
        }
    }

    /* calculate additional words based on operands */
    if (operand1 && operand1[0] == 'r' && operand2 && operand2[0] == 'r') {
        /* two register operands can be combined into one word */
        additionalWords = 1;
    } else {
        /* each operand contributes one additional word */
        additionalWords = (operand1 != NULL) + (operand2 != NULL);

        /* array operand contributes an -extra- word */
        if (operand1 && strstr(operand1, "[") && strstr(operand1, "]")) additionalWords++;
        if (operand2 && strstr(operand2, "[") && strstr(operand2, "]")) additionalWords++;
    }

    /* ~~~ taking care of the source operand bits 5-4 ~~~ */
    if(operand1 != NULL) {
        /* immediate addressing #k */  
        if (operand1[0] == '#')
            strcat(resultWord, "00");
        /* index addressing: arr[k]*/    
        else if (strstr(operand1, "[") && strstr(operand1, "]"))
            strcat(resultWord, "10");  
        /* direct register addressing: r4 */
        else if (operand1[0] == 'r' && isdigit(operand1[1]) && operand1[1] >= '0' && operand1[1] <= '7') 
            strcat(resultWord, "11");  
        /* direct addressing: x */
        else 
            strcat(resultWord, "01");       
    } else {
        /* operand1 is null */
        strcat(resultWord, "00");
    }
    /* ~~~ taking care of the target operand bits 3-2 ~~~ */
    /* immediate addressing */
    if(operand2 != NULL) {
        if (operand2[0] == '#')
            strcat(resultWord, "00");
        /* index addressing */    
        else if (strstr(operand2, "[") && strstr(operand2, "]"))
            strcat(resultWord, "10");  
        /* direct register addressing */
        else if (operand2[0] == 'r' && isdigit(operand2[1]) && operand2[1] >= '0' && operand2[1] <= '7') 
            strcat(resultWord, "11");  
        /* direct addressing */
        else 
            strcat(resultWord, "01");
    } else {
        /* operand2 is null */
        strcat(resultWord, "00");
    }
    /* adding the A,R,E bits */
    strcat(resultWord, "00");

    /* handling errors */
    if (!found) {
        printf("Error: Unknown operation-%s.\n", operation);
        isError = 1;
    } else {
        /* check if there's enough space in the instructionTalbe */
        if (instructionCount + additionalWords >= MAX_INSTRUCTIONS) {
            printf("Error: Too many instructions.\n");
            isError = 1;
            return;
        }
        /* add the resultWord to the instructionTable */
        strcpy(instructionTable[instructionCount], resultWord);
        instructionCount += additionalWords + 1; /* increment instructionCount by the number of additional words */
    }
}

void scanSymbolsAllocateWords(FILE *file, Symbol *symbolTable) {
    char line[MAX_LINE_LENGTH];
    int i;

    while (fgets(line, sizeof(line), file)) {
        /* symbol properties */
        char symbolName[MAX_LINE_LENGTH];
        char symbolIdentifier[10];
        int symbolValue;
        char *colonPosition;

        if (line[0] == ';' || isWhiteLine(line)) {
            continue;
        }

        /* Parse the line according to a .define line */
        if (strncmp(line, ".define", 7) == 0) {
            strcpy(symbolIdentifier, "mdefine");
            sscanf(line, ".define %s = %d", symbolName, &symbolValue);
            addSymbol(symbolName, symbolValue, symbolIdentifier);
            continue;
        }

        /* Parse the line according to a .extern line */
        else if (strncmp(line, ".extern", 7) == 0) {
            strcpy(symbolIdentifier, "external");
            sscanf(line, ".extern %s", symbolName);
            addSymbol(symbolName, -1, symbolIdentifier); /* Extern symbols don't have a value, entring -1 */
            continue;
        }

        /* Parse the line according to a .entry line */
        else if (strncmp(line, ".entry", 6) == 0) {
            /*
            strcpy(symbolIdentifier, "entry");
            sscanf(line, ".entry %s", symbolName);
            addSymbol(symbolName, -1, symbolIdentifier);
            */
            continue;
        }

        /* ~~~~~ Intructions ~~~~~ */

        /* if its a symbol defenition like "SYMBOL: ____" */
        /* saving the pointer for the ":" in the line if exists */
        colonPosition = strchr(line, ':');
        if (colonPosition) {
            char *directiveStart;
            /* symbol length */
            int length = colonPosition - line;

            /* copying the symbol name from the line */
            strncpy(symbolName, line, length);

            symbolName[colonPosition - line] = '\0';

            /* move the pointer to the start of the directive */
            directiveStart = colonPosition + 1;
            /* Remove leading white spaces */
            while(isspace((unsigned char) *directiveStart)) {
                directiveStart++;
            }
            
            /* Array */
            if (strlen(directiveStart) >= 6 && strncmp(directiveStart, ".data ", 6) == 0) {
                char* pointer = strtok(directiveStart + 6, ",");
                
                strcpy(symbolIdentifier, "data");
                symbolValue = dataCount; /* saving the data starting address */
                /* Adding the symbol */
                addSymbol(symbolName, symbolValue, symbolIdentifier);                

                /* skip to the first number of the array */
                while (pointer != NULL) {
                    int value;
                    char binary[15];

                    /* assigning the pointer character to value and
                    if its not a number, override its value to get a symbol */
                    if (sscanf(pointer, "%d", &value) != 1) {
                        value = findMDefine(pointer);
                    }
                    addData(value);

                    /* getting a 14 bits binary representation of the value 
                    and pushing it to the instuctionArray */
                    toBinary(value, binary);
                    strcpy(instructionTable[instructionCount++], binary);
                            
                    pointer = strtok(NULL, ",");
                }

            }
                
            /* String */
            else if (strlen(directiveStart) >= 8 && strncmp(directiveStart, ".string ", 8) == 0) {
                char* pointer = strchr(directiveStart, '\"');
                char zero[15] = "00000000000000";

                strcpy(symbolIdentifier, "data");
                symbolValue = dataCount; /* saving the data starting address */
                /* Adding the symbol */
                addSymbol(symbolName, symbolValue, symbolIdentifier);
                
                /* skip to the first character after the " */
                if (pointer == NULL) {
                    printf("Error: missing starting quote in .string directive\n");
                    isError = 1;
                }
                pointer++;  /* Skip past the opening quote */

                /* Iterate over each character in the string */
                while (*pointer != '\"') {
                    char binary[15];

                    addData((int)*pointer);

                    /* getting a 14 bits binary representation of the character's ascii
                    and pushing it to the instuctionArray */
                    toBinary((int)*pointer, binary);
                    strcpy(instructionTable[instructionCount++], binary);

                    /* Move to the next character */
                    pointer++;
                    if (*pointer == '\0') {
                        printf("Error: missing closing quote in .string directive\n");
                        isError = 1;
                    }
                }
                addData(0);
                strcpy(instructionTable[instructionCount++], zero);
            }

            
            /* Code after symbol, ":" */
            else {
                char *instructionStart;

                strcpy(symbolIdentifier, "code");
                symbolValue = instructionCount + 100;
                /* increase DC for the next symbol */
                /* dataCount++; */
                addSymbol(symbolName, symbolValue, symbolIdentifier);

                /* Extract the part of the line after the ":" */
                instructionStart = colonPosition + 1;

                /* Skip any leading spaces or tabs */
                instructionStart += strspn(instructionStart, " \t");

                /* Pass the instruction to the addInstructionFirstWord function */
                addInstructionFirstWord(instructionStart);
            }
        }
        /* independant operation */
        else {
            addInstructionFirstWord(line);
        }
    }  
    /* Iterate over the symbolTable */
    for (i = 0; i < symbolCount; i++) {
        /* Check if the identifier of the symbol is "data" */
        if (strcmp(symbolTable[i].identifier, "data") == 0) {
            /* Add instructionCount + 100 to the value of the symbol */
            symbolTable[i].value += instructionCount + 100 - dataCount;
        }
    }
}
