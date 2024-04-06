#define MAX_LINE_LENGTH 256
#define MAX_MACROS 100
#define MAX_SYMBOLS 4096
#define MAX_INSTRUCTIONS 3996
#define WORD_LENGTH 14

extern int* dataArray;
extern int currentSize;

typedef struct {
    char name[MAX_LINE_LENGTH];
    char identifier[MAX_LINE_LENGTH];
    int value;
} Symbol;

extern Symbol symbolTable[MAX_SYMBOLS];

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
int instructionCount = 0;