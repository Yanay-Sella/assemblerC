#ifndef GLOBAL_H

#define GLOBAL_H

#define MAX_LINE_LENGTH 80
#define MAX_MACROS 100
#define MAX_SYMBOLS 4096
#define MAX_INSTRUCTIONS 3996
#define WORD_LENGTH 15
#define MAX_USAGE 4096

/* data integer array for .string and .data */
extern int* dataArray;
extern int dataArraySize;

typedef struct {
    char name[MAX_LINE_LENGTH];
    char identifier[MAX_LINE_LENGTH];
    int value;
} Symbol;

extern Symbol symbolTable[MAX_SYMBOLS];
extern int symbolCount;

typedef struct {
    const char* name;
    const char* opcode;
} Operation;

static const Operation operations[] = {
    {"mov", "0000"},
    {"cmp", "0001"},
    {"add", "0010"},
    {"sub", "0011"},
    {"not", "0100"},
    {"clr", "0101"},
    {"lea", "0110"},
    {"inc", "0111"},
    {"dec", "1000"},
    {"jmp", "1001"},
    {"bne", "1010"},
    {"red", "1011"},
    {"prn", "1100"},
    {"jsr", "1101"},
    {"rts", "1110"},
    {"hlt", "1111"},
};

char instructionTable[MAX_INSTRUCTIONS][WORD_LENGTH];
extern int instructionCount;

typedef struct {
    char operation[MAX_LINE_LENGTH];
    char* operand1;
    char* operand2;
} InstructionParts;

typedef struct {
    char symbolName[MAX_LINE_LENGTH]; /* symbol name, assuming max length of 30 */
    int locations[MAX_USAGE]; /* array of instruction numbers where the symbol is used */
} ExternUsage;
ExternUsage externUsageTable[MAX_SYMBOLS];
extern int externUsageCount;

extern int isError;

/* ~~~ common functions ~~~ */
int findSymbol(char* symbolName);
InstructionParts splitInstruction(char* instructionLine);
int isWhiteLine(char *str);

#endif
