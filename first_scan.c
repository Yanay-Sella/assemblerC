#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "first_scan.h"
#include "global.h"

int dataCount = 0;

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
    for (i = 0; i < dataCount; i++) {
        /* If the symbol name matches and its identifier is "mdefine", return its value */
        if (strcmp(symbolTable[i].name, symbolName) == 0 && strcmp(symbolTable[i].identifier, "mdefine") == 0) {
            return symbolTable[i].value;
        }
    }
    /* if the symbol was not found or its identifier is not "mdefine" */
    printf("Error: Symbol %s not found or not an mdefine.\n", symbolName);
    exit(1);
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
    printf("ADDED DATA:%d | DC:%d\n", value, dataCount);
}

int findSymbol(char* symbolName) {
    int i;
    for (i = 0; i < dataCount; i++) {
        if (strcmp(symbolTable[i].name, symbolName) == 0) {
            return i;
        }
    }
    /* symbol not found */
    return -1;
}

/* add symbol to the symbolTable */
void addSymbol(char* symbolName, int symbolValue, char* symbolIdentifier) {
    
    int symbolIndex = findSymbol(symbolName);
    /* check for conflicts in the symbol table 
    entry symbols can be added with onflicts, and vice versa */
    if(symbolIndex >= 0 && strcmp(symbolTable[symbolIndex].identifier, "entry") != 0 && strcmp(symbolIdentifier, "entry") != 0){
        printf("Error: Symbol %s already defined.\n", symbolName);
        return;
    }

    /* add the symbol to the symbol table at dataCount */ 
    strcpy(symbolTable[dataCount].name, symbolName);
    symbolTable[dataCount].value = symbolValue;
    strcpy(symbolTable[dataCount].identifier, symbolIdentifier);

    printf("SYMBOL ADDED: sname-%s svalue-%d siden-%s DC-%d\n", symbolName, symbolValue, symbolIdentifier, dataCount);
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
    char* operands;
    char* operand1;
    char* operand2;
    char* saveptr;

    int i; 
    int found = 0;
    char resultWord[WORD_LENGTH] = "0000"; /* starting with the four not-used bits */
    int additionalWords = 0;
    int len;

    /* split the instructionLine into tokens */
    operation = strtok(instructionLine, " ");
    operands = strtok(NULL, "");

    if (operands != NULL) {
        saveptr = operands;
        operand1 = strsep(&saveptr, ",");
        operand2 = strsep(&saveptr, ",");  
    } else {
        operand1 = NULL;
        operand2 = NULL;
    }
    

    /* remove trailing newline and white spaces from operation */
    len = strlen(operation);
    while (len > 0 && isspace((unsigned char) operation[len-1])) {
        len--;
    }
    operation[len] = '\0';

    printf("opname:%s | op1:%s | op2:%s\n", operation, operand1, operand2);
    
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
    /* immediate addressing */
    if(operand1 != NULL) {
        if (operand1[0] == '#')
            strcat(resultWord, "00");
        /* index addressing */    
        else if (strstr(operand1, "[") && strstr(operand1, "]"))
            strcat(resultWord, "10");  
        /* direct register addressing */
        else if (operand1[0] == 'r' && isdigit(operand1[1]) && operand1[1] >= '0' && operand1[1] <= '7') 
            strcat(resultWord, "11");  
        /* direct addressing */
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
    printf("additional words: %d\n", additionalWords);
    printf("word:%s\n", resultWord);

    /* handling errors */
    /* TODO: check for other erros like brackets, correct register */
    if (!found) {
        printf("Error: Unknown operation-%s.\n", operation);
        exit(1);
    } else {
        /* check if there's enough space in the instructionTalbe */
        if (instructionCount + additionalWords >= MAX_INSTRUCTIONS) {
            printf("Error: Too many instructions.\n");
            return;
        }
        /* add the resultWord to the instructionTable */
        strcpy(instructionTable[instructionCount], resultWord);
        instructionCount += additionalWords + 1; /* increment instructionCount by the number of additional words */
    }
}


/* for debugging */
void printAll(void) {
    int i;
    printf("dataCount: %d\n instructionCount: %d\n", dataCount, instructionCount);
    printf("~~~~~~~ Symbols: ~~~~~~~\n");
    for (i = 0; i < dataCount; i++) {
        printf("Name: %s, Value: %d, Identifier: %s\n", symbolTable[i].name, symbolTable[i].value, symbolTable[i].identifier);
    }

    printf("\n~~~~~~~ Instruction Table: ~~~~~~~\n");
    for (i = 0; i < instructionCount; i++) {
        printf("Instruction %d: %s\n", i, instructionTable[i]);
    }

    printf("\n~~~~~~~ Data Array: ~~~~~~~\n");
    for (i = 0; i < dataCount; i++) {
        printf("Data: %d\n", dataArray[i]);
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

        printf("\n--------------------------\n");


        printf("%s\tIC=%d\n", line, instructionCount);
        if (line[0] == ';') {
            continue;
        }

        /* Parse the line according to a .define line */
        if (strncmp(line, ".define", 7) == 0) {
            strcpy(symbolIdentifier, "mdefine");
            sscanf(line, ".define %s = %d", symbolName, &symbolValue);
            /* increase DC for the next symbol */
            addSymbol(symbolName, symbolValue, symbolIdentifier);
            dataCount++;
            continue;
        }

        /* Parse the line according to a .extern line */
        else if (strncmp(line, ".extern", 7) == 0) {
            strcpy(symbolIdentifier, "external");
            sscanf(line, ".extern %s", symbolName);
            /* increase DC for the next symbol */
            dataCount++;
            addSymbol(symbolName, -1, symbolIdentifier); /* Extern symbols don't have a value, entring -1 */
            continue;
        }

        /* Parse the line according to a .entry line */
        else if (strncmp(line, ".entry", 6) == 0) {
            strcpy(symbolIdentifier, "entry");
            sscanf(line, ".entry %s", symbolName);
            /* increase DC for the next symbol */
            dataCount++;
            addSymbol(symbolName, -1, symbolIdentifier); /* Entry symbols don't have a value, entring -1 */
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

            printf("SYMBOL FOUND!\n");
            /* copying the symbol name from the line */
            strncpy(symbolName, line, length);

            symbolName[colonPosition - line] = '\0';

            /* move the pointer to the start of the directive */
            directiveStart = colonPosition + 1;
            /* Remove leading white spaces */
            while(isspace((unsigned char) *directiveStart)) {
                directiveStart++;
            }
            printf("SymbolName:%s | ", symbolName);
            printf("directiveStart:%s\n", directiveStart);
            /* check for directives or operations */
            
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

                strcpy(symbolIdentifier, "data");
                symbolValue = dataCount; /* saving the data starting address */
                addSymbol(symbolName, symbolValue, symbolIdentifier);
                
                /* skip to the first character after the " */
                if (pointer == NULL) {
                    printf("Error: missing starting quote in .string directive\n");
                    exit(1);
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
                        exit(1);
                    }
                }
            }

            
            /* Code after symbol, ":" */
            else {
                char *instructionStart;
                printf("CODE symbol\n");

                strcpy(symbolIdentifier, "code");
                symbolValue = instructionCount + 100;
                /* increase DC for the next symbol */
                dataCount++;
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
    for (i = 0; i < dataCount; i++) {
        /* Check if the identifier of the symbol is "data" */
        if (strcmp(symbolTable[i].identifier, "data") == 0) {
            /* Add instructionCount + 100 to the value of the symbol */
            symbolTable[i].value += instructionCount + 100;
        }
    }
    printAll();
}
