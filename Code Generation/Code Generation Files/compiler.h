#ifndef COMPILER_H
#define COMPILER_H

#define TEST_COMPILER    // uncomment to run the compiler autograder

#include "parser.h"
#include "symbols.h"
#include "lexer.h"

#include <stdio.h>
#include <dirent.h>
#include <string.h>

int InitCompiler ();
ParserInfo compile (char* dir_name);
int StopCompiler();

// write to the vm file
void writeToFile(char* line);

// global variables
extern int numberOfParseIterations;
extern FILE* vmFile;
extern char* temporaryVariable;
extern int printStringWasCalled;
extern int isConstructor;
extern int expressionListCount;
extern int isMethod;
extern int isThis;
extern int readIntIsCalled;
extern int callNewArray;
extern int numberOfWhileStatements;

#endif
