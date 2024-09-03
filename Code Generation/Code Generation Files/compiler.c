#include "compiler.h"

int numberOfParseIterations=0;
FILE* vmFile = NULL;
char* temporaryVariable = NULL;
int printStringWasCalled = 0;
int isConstructor = 0;
int expressionListCount = 0;
int isMethod = 0;
int isThis = 0;
int readIntIsCalled = 0;
int callNewArray = 0;
int numberOfWhileStatements = 0;

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

			// create a .vm file with the same name as filename
			// the code generation will write to this file
			char vmFileName[512];
			snprintf(vmFileName, 512, "%s/%s.vm", dir_name, filename);
			//printf("Creating file %s in %s directory \n", vmFileName, dir_name);

			vmFile = fopen(vmFileName, "w");

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

			// close the .vm file
			fclose(vmFile);
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

// write to the vm file
void writeToFile(char* word){
	fprintf(vmFile, "%s", word);
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
