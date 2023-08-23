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

// global variables
extern int numberOfParseIterations;

#endif
