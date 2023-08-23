/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name:
Student ID:
Email:
Date Work Commenced:
*************************************************************************/

#include "compiler.h"

int numberOfParseIterations=0;

int InitCompiler ()
{
	// initialise the symbol table
	// initialise the parser
	// initialise the lexer

	// nothing needs to be done here

	return 1;
}

ParserInfo compile (char* dir_name)
{	

	DIR *dir;
    struct dirent *ent;
    char path[512];
    const char *ext = ".jack";

	ParserInfo p;

    // Open the directory
    dir = opendir(dir_name);
	if (dir == NULL) {
		printf("Error opening directory\n");
		return p;
	}

	// create the symbol table
	initRootNode("root");

    // Iterate through all files in the directory
	numberOfParseIterations=0;
    while ((ent = readdir(dir)) != NULL) {
        // Check if the file has a .jack extension
        if (strstr(ent->d_name, ext) != NULL) {
            // Construct the full path of the file
            snprintf(path, 512, "%s/%s", dir_name, ent->d_name);
            // Process the file here
            //printf("Processing file %s\n", path);
			// initialise the lexer
			
			InitLexer(path);

			// initialise the symbol table
			// get the filename from the path
			char *filename = strrchr(path, '/');
			// remove the .jack extension
			filename[strlen(filename) - 5] = '\0';
			// remove the / from the filename
			filename++;
			// parse the file once
			p = Parse();
			//printTree(TREE_ROOT, 3);
			//printf("__________________________________________________________\n");

			if (p.er != none) {
				return p;
			}

			// increment the number of parse iterations
			// reset the lexer by closing and reopening the file
			// ResetLexer();
			// parse the file again
			// increment the number of parse iterations
		}
	}
    closedir(dir);

	dir = opendir(dir_name);
	if (dir == NULL) {
		//printf("Error opening directory\n");
		return p;
	}


	//if (CURRENT_NODE!= TREE_ROOT){
		//printf("12. Error: Current node is not root\n");
	//}
	numberOfParseIterations++;

	//printf("Entering Iteration 2__________________________________________________________\n");
	//printTree(TREE_ROOT, 3);
	//printf("__________________________________________________________\n");

	while ((ent = readdir(dir)) != NULL) {
        // Check if the file has a .jack extension
        if (strstr(ent->d_name, ext) != NULL) {
            // Construct the full path of the file
            snprintf(path, 512, "%s/%s", dir_name, ent->d_name);
            // Process the file here
            //printf("Processing file %s\n", path);
			// initialise the lexer
			InitLexer(path);
			// initialise the symbol table
			// get the filename from the path
			char *filename = strrchr(path, '/');
			// remove the .jack extension
			filename[strlen(filename) - 5] = '\0';
			// remove the / from the filename
			filename++;

			// parse the file once
			p = Parse();
			//printTree(TREE_ROOT, 3);
			//printf("__________________________________________________________\n");
			
			if (p.er != none) {
				return p;
			}
			// increment the number of parse iterations
			// reset the lexer by closing and reopening the file
			//ResetLexer();
			// parse the file again
			// increment the number of parse iterations
		}
	}

	return p;
}

int StopCompiler ()
{
	// stop symbol table
	cleanUpConstructor();
	// stop parser which also stops lexer
	StopParser();
	return 1;
}


#ifndef TEST_COMPILER
int main ()
{
	InitCompiler ();
	ParserInfo p = compile ("Pong");
	PrintError (p);
	StopCompiler ();
	return 1;
}
#endif
