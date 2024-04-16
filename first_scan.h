#include <stdio.h>
#include "global.h"

void scanSymbolsAllocateWords(FILE *file, Symbol *symbolTable);
int findMDefine(char* symbolName);
void printAll(void);
extern int dataCount;
