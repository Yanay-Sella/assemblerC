#define MAX_LINE_LENGTH 256
#define MAX_MACROS 100
#define MAX_SYMBOLS 4096

typedef struct {
    char symbolname[MAX_LINE_LENGTH];
    char identifier[MAX_LINE_LENGTH];
    int value;
} Symbol;

extern Symbol symbolTable[MAX_SYMBOLS];