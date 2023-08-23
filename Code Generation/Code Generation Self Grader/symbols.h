#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "compiler.h"

#define TABLE_SIZE 1000

typedef enum {
	fieldVariable,  // instance variable
    staticVariable, // class variable
    argument, // parameter
    var, // local variable
    subroutine, // method or function
    class,
    null // not a variable
} Kind;

typedef enum{
    rootOfTree,
    classNode,
    methodNode
} NodeType;

typedef struct Symbol {
    char name[128];
    char type[128];
    Kind kind;
    int number;
} Symbol;

typedef struct SymbolTable {
    Symbol* table[TABLE_SIZE];
    int nextAvailableLocation;
    char className[128];
} SymbolTable;

typedef struct TreeNode {
    struct TreeNode* parent;
    struct TreeNode* firstChild;
    struct TreeNode* nextSibling;
    SymbolTable* symbolTable;
    NodeType nodeType;
} TreeNode;


// Create a new symbol table
SymbolTable* createSymbolTable(char *className);
// insert a symbol into the symbol table, error if unsuccessful. As you insert, update the counter in the symbol table
int insertSymbol(SymbolTable* table, Symbol* symbol);
// lookup a symbol in the symbol table
Symbol* lookupSymbol(SymbolTable* table, char* name);
// initialise root node
TreeNode* initRootNode(char* className);
// initialise a new child node
TreeNode* initNode(TreeNode* parent, char* className, NodeType nodeType);
// clean up the whole structure
void cleanUp(TreeNode* root);
// construct a symbol
Symbol* constructSymbol(char* name, char* type, Kind kind, int number);
// print the symbol table
void printSymbolTable(SymbolTable* table);
// print the whole tree
void printTree(TreeNode* root, int level);
// get current node
TreeNode* getCurrentNode();
// set current node
void setCurrentNode(TreeNode* node);
// get root node
TreeNode* getRootNode();
// change current class name
void changeCurrentClassName(char* className);
// perform lookup on the current node and its parents until the root node
Symbol* lookupSymbolInCurrentScope(char* name);
Symbol* findClassNode(char* className);
TreeNode* findMethodNode(char* methodName, TreeNode* targetClass);
TreeNode* findClassNodeReturnNode(char* className);
int insertSymbolLocalScope(SymbolTable* table, Symbol* symbol);

int isJackLibrary(char* className);
int isJackMathLibraryMethod(char* methodName);
int isJackMemoryLibraryMethod(char* methodName);
int isJackOutputLibraryMethod(char* methodName);
int isJackScreenLibraryMethod(char* methodName);
int isJackStringLibraryMethod(char* methodName);
int isJackSysLibraryMethod(char* methodName);
int isJackKeyboardLibraryMethod(char* methodName);
int isJackArrayLibraryMethod(char* methodName);

int isJackLibraryMethod(char* className, char* methodName);
int calculateNumberOfLocalVariablesInCurrentScope(SymbolTable* table);
int calculateNumberOfFieldVariablesInCurrentScope(SymbolTable* table);
void cleanUpConstructor();


extern TreeNode* TREE_ROOT;
extern TreeNode* CURRENT_NODE;

extern char *JACKlibraries[];
extern char *JACKMathLibraryMethods[];
extern char *JACKMemoryLibraryMethods[];
extern char *JACKOutputLibraryMethods[];
extern char *JACKScreenLibraryMethods[];
extern char *JACKStringLibraryMethods[];
extern char *JACKSysLibraryMethods[];
extern char *JACKKeyboardLibraryMethods[];
extern char *JACKArrayLibraryMethods[];

#endif
