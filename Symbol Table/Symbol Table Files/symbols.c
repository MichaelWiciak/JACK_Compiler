
/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Symbol Tables Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name:
Student ID: 201413717
Email: ed203mw@leeds.ac.uk
Date Work Commenced: 01 April 2023
*************************************************************************/

#include "symbols.h"

TreeNode* TREE_ROOT;
TreeNode* CURRENT_NODE;

char *JACKlibraries[] = {"Array", "Keyboard", "Math", "Memory", "Output", "Screen", "String", "Sys"};
char *JACKMathLibraryMethods[] = {"abs", "multiply", "divide", "min", "max", "sqrt"};
char *JACKMemoryLibraryMethods[] = {"peek", "poke", "alloc", "deAlloc"};
char *JACKOutputLibraryMethods[] = {"moveCursor", "printChar", "printString", "printInt", "println", "backSpace"};
char *JACKScreenLibraryMethods[] = {"clearScreen", "setColor", "drawPixel", "drawLine", "drawRectangle", "drawCircle"};
char *JACKStringLibraryMethods[] = {"new", "dispose", "length", "charAt", "setCharAt", "appendChar", "setInt", "eraseLastChar", "intValue", "backSpace", "doubleQuote", "newLine"};
char *JACKSysLibraryMethods[] = {"halt", "error", "wait"};
char *JACKKeyboardLibraryMethods[] = {"keyPressed", "readChar", "readLine", "readInt"};
char *JACKArrayLibraryMethods[] = {"new", "dispose"};

// Create a new symbol table
SymbolTable* createSymbolTable(char *className) {
    SymbolTable* table = (SymbolTable*) malloc(sizeof(SymbolTable));
    table->nextAvailableLocation = 0;
    strcpy(table->className, className);
    return table;
}

// Insert a symbol into the symbol table
int insertSymbol(SymbolTable* table, Symbol* symbol) {
    if (table->nextAvailableLocation >= TABLE_SIZE) {
        return -1;
    }

    // check if symbol already exists using lookup
    if (lookupSymbolInCurrentScope(symbol->name) != NULL) {
        return 0;
    }

    table->table[table->nextAvailableLocation] = symbol;
    table->nextAvailableLocation++;
    return 1;
} 

// Insert a symbol into the symbol table but only check the local scope
int insertSymbolLocalScope(SymbolTable* table, Symbol* symbol) {
    if (table->nextAvailableLocation >= TABLE_SIZE) {
        return -1;
    }

    // check if symbol already exists using lookup
    if (lookupSymbol(table, symbol->name) != NULL) {
        return 0;
    }

    table->table[table->nextAvailableLocation] = symbol;
    table->nextAvailableLocation++;
    return 1;
} 

// construct a symbol
Symbol* constructSymbol(char* name, char* type, Kind kind, int number) {
    Symbol* symbol = (Symbol*) malloc(sizeof(Symbol));
    strcpy(symbol->name, name);
    strcpy(symbol->type, type);
    symbol->kind = kind;
    symbol->number = number;
    return symbol;
}

// lookup a symbol in the symbol table
Symbol* lookupSymbol(SymbolTable* table, char* name) {
    for (int i = 0; i < table->nextAvailableLocation; i++) {
        if (strcmp(table->table[i]->name, name) == 0) {
            return table->table[i];
        }
    }
    return NULL;
}

// initialise root node
TreeNode* initRootNode(char* className) {
    TreeNode* root = (TreeNode*) malloc(sizeof(TreeNode));
    root->parent = NULL;
    root->firstChild = NULL;
    root->nextSibling = NULL;
    root->nodeType = rootOfTree;
    root->symbolTable = createSymbolTable(className);

    // set global variables
    TREE_ROOT = root;
    CURRENT_NODE = root;

    return root;
}

// initialise a new child node
TreeNode* initNode(TreeNode* parent, char* className, NodeType nodeType) {
    TreeNode* node = (TreeNode*) malloc(sizeof(TreeNode));
    node->parent = parent;
    node->firstChild = NULL;
    node->nextSibling = NULL;
    node->nodeType = nodeType;
    node->symbolTable = createSymbolTable(className);

    // add to parent's children
    if (parent->firstChild == NULL) {
        parent->firstChild = node;
    } else {
        // find last child
        TreeNode* child = parent->firstChild;
        while (child->nextSibling != NULL) {
            child = child->nextSibling;
        }
        // add new node to end of children
        child->nextSibling = node;
    }

    return node;
}

// clean up the whole structure
void cleanUp(TreeNode* root) {
    if (root == NULL) {
        return;
    }
   // first go to the bottom of the tree
    TreeNode* child = root->firstChild;
    while (child != NULL) {
        cleanUp(child);
        child = child->nextSibling;
    }
    // then clean up the symbol table
    free(root->symbolTable);
    // then clean up the node
    free(root);
}

void cleanUpConstructor(){
    cleanUp(TREE_ROOT);
}


// print the symbol table
void printSymbolTable(SymbolTable* table) {
    printf("Symbol Table %s:\n", table->className);
    for (int i = 0; i < table->nextAvailableLocation; i++) {
        printf("Name: %s, Type: %s, Kind: %d, Number: %d\n", table->table[i]->name, table->table[i]->type, table->table[i]->kind, table->table[i]->number);
    }
}

// print the whole tree
void printTree(TreeNode* root, int level) {

    if (root == NULL) {
        return;
    }
    // print the symbol table
    char* parent;
    if (root->parent == NULL) {
        parent = "NULL";
    } else {
        parent = root->parent->symbolTable->className;
    }
    printf("%i node type with parent %s", root->nodeType, parent);
    printSymbolTable(root->symbolTable);
    // print the children
    TreeNode* child = root->firstChild;
    while (child != NULL) {
        printTree(child, level + 1);
        child = child->nextSibling;
    }
}

// get current node
TreeNode* getCurrentNode() {
    return CURRENT_NODE;
}

// set current node
void setCurrentNode(TreeNode* node) {
    CURRENT_NODE = node;
}

// get root node
TreeNode* getRootNode() {
    return TREE_ROOT;
}

// change current class name
void changeCurrentClassName(char* className) {
    strcpy(CURRENT_NODE->symbolTable->className, className);
}

// perform lookup on the current node and its parents until the root node
Symbol* lookupSymbolInCurrentScope(char* name) {
    TreeNode* node = CURRENT_NODE;
    while (node != NULL) {
        Symbol* symbol = lookupSymbol(node->symbolTable, name);
        //printf("Looked for %s in %s\n", name, node->symbolTable->className);
        //printf("Found %s\n", symbol->name);
        if (symbol != NULL) {
            return symbol;
        }
        node = node->parent;
    }
    return NULL;
}

// find the Tree* node with a given class name
Symbol* findClassNode(char* className) {
    // in the tree_root, iterate through all the values in the symbol table
    // if the name of the symbol is equal to className, it has been found and return symbol
    // else return null
    // since there is just one root symbol table
    TreeNode* topOfTree = TREE_ROOT;
    for (int i = 0; i < topOfTree->symbolTable->nextAvailableLocation; i++) {
        if (strcmp(topOfTree->symbolTable->table[i]->name, className) == 0) {
            return topOfTree->symbolTable->table[i];
        }
    }
    return NULL;
}

TreeNode* findClassNodeReturnNode(char* className){
    TreeNode* topOfTree = TREE_ROOT;
    TreeNode* node = topOfTree->firstChild;
    // go to the bottom of the tree and check if null
    // if not null, check if the class name matches
    // if not, go to the sibling and repeat until null
    while (node != NULL) {
        if (strcmp(node->symbolTable->className, className) == 0) {
            return node;
        }
        node = node->nextSibling;
    }
    return NULL;
}


// find the Tree* with a given method name
TreeNode* findMethodNode(char* methodName, TreeNode* targetClass) {
    TreeNode* topOfTree = TREE_ROOT;
    TreeNode* node = topOfTree->firstChild;
    // go to the bottom of the tree and check if null
    // if not null, check if the method name matches
    // if not, go to the sibling and repeat until null
    while (node != NULL) {
        if (node->firstChild != NULL) {
            TreeNode* child = node->firstChild;
            while (child != NULL && child->parent == targetClass) {
                if (strcmp(child->symbolTable->className, methodName) == 0) {
                    return child;
                }
                child = child->nextSibling;
            }
        }
        node = node->nextSibling;
    }
    return NULL;
}

// check if char* is a jack library
int isJackLibrary(char* name) {
    if (strcmp(name, "Array") == 0) {
        return 1;
    } else if (strcmp(name, "Keyboard") == 0) {
        return 1;
    } else if (strcmp(name, "Math") == 0) {
        return 1;
    } else if (strcmp(name, "Memory") == 0) {
        return 1;
    } else if (strcmp(name, "Output") == 0) {
        return 1;
    } else if (strcmp(name, "Screen") == 0) {
        return 1;
    } else if (strcmp(name, "String") == 0) {
        return 1;
    } else if (strcmp(name, "Sys") == 0) {
        return 1;
    } else {
        return 0;
    }
    
}

// check if a method is a jack library math method
int isJackLibraryMathMethod(char* name) {
    if (strcmp(name, "abs") == 0) {
        return 1;
    } else if (strcmp(name, "multiply") == 0) {
        return 1;
    } else if (strcmp(name, "divide") == 0) {
        return 1;
    } else if (strcmp(name, "min") == 0) {
        return 1;
    } else if (strcmp(name, "max") == 0) {
        return 1;
    } else if (strcmp(name, "sqrt") == 0) {
        return 1;
    } else {
        return 0;
    }
}

// check if a method is a JACKMemoryLibraryMethods
int isJackLibraryMemoryMethod(char* name) {
    if (strcmp(name, "peek") == 0) {
        return 1;
    } else if (strcmp(name, "poke") == 0) {
        return 1;
    } else if (strcmp(name, "alloc") == 0) {
        return 1;
    } else if (strcmp(name, "deAlloc") == 0) {
        return 1;
    } else {
        return 0;
    }
}

// check if a method is a JACKOutputLibraryMethods
int isJackLibraryOutputMethod(char* name) {
    if (strcmp(name, "moveCursor") == 0) {
        return 1;
    } else if (strcmp(name, "printChar") == 0) {
        return 1;
    } else if (strcmp(name, "printInt") == 0) {
        return 1;
    } else if (strcmp(name, "printString") == 0) {
        return 1;
    } else if (strcmp(name, "println") == 0) {
        return 1;
    } else if (strcmp(name, "backSpace") == 0) {
        return 1;
    } else {
        return 0;
    }
}

// check if a method is a JACKScreenLibraryMethods
int isJackLibraryScreenMethod(char* name) {
    if (strcmp(name, "clearScreen") == 0) {
        return 1;
    } else if (strcmp(name, "setColor") == 0) {
        return 1;
    } else if (strcmp(name, "drawPixel") == 0) {
        return 1;
    } else if (strcmp(name, "drawLine") == 0) {
        return 1;
    } else if (strcmp(name, "drawRectangle") == 0) {
        return 1;
    } else if (strcmp(name, "drawCircle") == 0) {
        return 1;
    } else {
        return 0;
    }
}

// check if a method is a JACKStringLibraryMethods
int isJackLibraryStringMethod(char* name) {
    if (strcmp(name, "new") == 0) {
        return 1;
    } else if (strcmp(name, "dispose") == 0) {
        return 1;
    } else if (strcmp(name, "length") == 0) {
        return 1;
    } else if (strcmp(name, "charAt") == 0) {
        return 1;
    } else if (strcmp(name, "setCharAt") == 0) {
        return 1;
    } else if (strcmp(name, "appendChar") == 0) {
        return 1;
    } else if (strcmp(name, "setInt") == 0) {
        return 1;
    } else if (strcmp(name, "eraseLastChar") == 0) {
        return 1;
    } else if (strcmp(name, "intValue") == 0) {
        return 1;
    } else if (strcmp(name, "backSpace") == 0) {
        return 1;
    } else if (strcmp(name, "doubleQuote") == 0) {
        return 1;
    } else if (strcmp(name, "newLine") == 0) {
        return 1;
    } else {
        return 0;
    }
}

// check if a method is a JACKSysLibraryMethods
int isJackLibrarySysMethod(char* name) {
    if (strcmp(name, "halt") == 0) {
        return 1;
    } else if (strcmp(name, "error") == 0) {
        return 1;
    } else if (strcmp(name, "wait") == 0) {
        return 1;
    } else {
        return 0;
    }
}

// check if a method is a JACKKeyboardLibraryMethods
int isJackLibraryKeyboardMethod(char* name) {
    if (strcmp(name, "keyPressed") == 0) {
        return 1;
    } else if (strcmp(name, "readChar") == 0) {
        return 1;
    } else if (strcmp(name, "readLine") == 0) {
        return 1;
    } else if (strcmp(name, "readInt") == 0) {
        return 1;
    } else {
        return 0;
    }
}

// check if a method is a JACKArrayLibraryMethods
int isJackLibraryArrayMethod(char* name) {
    if (strcmp(name, "new") == 0) {
        return 1;
    } else if (strcmp(name, "dispose") == 0) {
        return 1;
    } else {
        return 0;
    }
}

// a method which call the appropriate method checker based on class name as char*
int isJackLibraryMethod(char* className, char* methodName) {
    if (strcmp(className, "Math") == 0) {
        return isJackLibraryMathMethod(methodName);
    } else if (strcmp(className, "Memory") == 0) {
        return isJackLibraryMemoryMethod(methodName);
    } else if (strcmp(className, "Output") == 0) {
        return isJackLibraryOutputMethod(methodName);
    } else if (strcmp(className, "Screen") == 0) {
        return isJackLibraryScreenMethod(methodName);
    } else if (strcmp(className, "String") == 0) {
        return isJackLibraryStringMethod(methodName);
    } else if (strcmp(className, "Sys") == 0) {
        return isJackLibrarySysMethod(methodName);
    } else if (strcmp(className, "Keyboard") == 0) {
        return isJackLibraryKeyboardMethod(methodName);
    } else if (strcmp(className, "Array") == 0) {
        return isJackLibraryArrayMethod(methodName);
    } else {
        return 0;
    }
}
