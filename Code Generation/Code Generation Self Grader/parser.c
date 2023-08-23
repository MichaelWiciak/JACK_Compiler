#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"
#include "compiler.h"
#include "symbols.h"




// define grammar rules for JACK
ParserInfo classDeclaration();
ParserInfo memberDeclaration();
ParserInfo classVariableDeclaration();
ParserInfo type(); 
ParserInfo subroutineDeclaration();
ParserInfo parameterList();
ParserInfo subroutineBody();
ParserInfo statement();
ParserInfo variableDeclarationStatement();
ParserInfo letStatement();
ParserInfo ifStatement();
ParserInfo whileStatement();
ParserInfo doStatement();
ParserInfo subroutineCall();
ParserInfo expressionList();
ParserInfo returnStatement();
ParserInfo expression();
ParserInfo relationalExpression();
ParserInfo arithmeticExpression();
ParserInfo term();
ParserInfo factor();
ParserInfo operand();


void printString(char* string){
    // first push the length of the string onto the stack
    char lengthOfStr[20];
    sprintf(lengthOfStr, "%d", (int)strlen(string));
    writeToFile("push constant ");
    writeToFile(lengthOfStr);
    writeToFile("\n");
    // call the String.new function
    writeToFile("call String.new 1\n");
    int i;
    for(i = 0; i < strlen(string); i++){
        //printf("char: %c\n", string[i]);
        // convert the char into an int
        int ascii = (int)string[i];
        //printf("ascii: %d\n", ascii);
        // convert the int into a string
        char asciiString[20];
        sprintf(asciiString, "%d", ascii);
        //printf("asciiString: %s\n", asciiString);
        // push the ascii value onto the stack
        writeToFile("push constant ");
        writeToFile(asciiString);
        writeToFile("\n");
        // call the printChar function
        writeToFile("call String.appendChar 2\n");
    }
    // the output.printString will be called automatically
}

int StopParser (){
    StopLexer();
    return 0;
} // stop the parser and do any necessary clean up

ParserInfo type(){
    // rule: type -> int | char | boolean | identifier
    Token token = GetNextToken();
    ParserInfo info;
    // if the token contains in token.lx is int, char, boolean or identifier
    if(strcmp(token.lx, "int") == 0 || strcmp(token.lx, "char") == 0 || strcmp(token.lx, "boolean") == 0 || token.tp == ID){
        
        // if the token is an identifier, we need to check if its in the symbol table
        // if not, its an undeclared identifier error
        if(token.tp == ID && numberOfParseIterations >= 1){

            // first check if the id is a class name
            // if it is, then we need to return no error



            // check if the identifier is in the symbol table
            Symbol* symbol = lookupSymbolInCurrentScope(token.lx);
            if(symbol == NULL){
                // check 
                
                Symbol* aClassNode = findClassNode(token.lx);
                if(aClassNode != NULL){
                    ;
                }
                else{

                    // check if the identifier is a name of a JACK library
                    int isLibrary = isJackLibrary(token.lx);
                    if (isLibrary == 0){
                        // Error: %s is not declared\n", token.lx);
                        //printSymbolTable(TREE_ROOT->symbolTable);
                        //printf("\n");
                        info.tk = token;
                        info.er = undecIdentifier;
                        return info;
                    }


  
                }


            }
        }

        info.tk = token;
        info.er = none;
        return info;
    }
    else{
        // printf("Error: type expected, but %s found\n", token.lx);
        info.tk = token;
        info.er = idExpected;
        return info;
    }
    
}

ParserInfo parameterList(){
    // rule: parameterList -> type identifier {, type identifier} | ε
    // print message for debugging

    Token token;
    ParserInfo info;

    // check if the token is a type or not
    // if not, then it is epsilon and we need to return no error

    token = PeekNextToken();

    
    // check if token is a type or not
    if(strcmp(token.lx, "int") != 0 && strcmp(token.lx, "char") != 0 && strcmp(token.lx, "boolean") != 0 && token.tp != ID){
        // it is epsilon
        info.tk = token;
        info.er = none;
        return info;
    }
    // check if the token is a type or not
    info = type();
    if(info.er != none){
        return info;
    }
    // keep track of the type by storing it in variable
    char* aType = info.tk.lx;

    Kind kind = argument;


    // we need to consume the identifier token

    token = GetNextToken();
    if(token.tp != ID){
        // printf("Error in ParameterList: identifier expected, but %s found\n", token.lx);
        info.tk = token;
        info.er = idExpected;
        return info;
    }

    // store the identifier name as a variable
    char* name = token.lx;

    // create a symbol
    Symbol* symbol = constructSymbol(name, aType, kind, CURRENT_NODE->symbolTable->nextAvailableLocation);
    // insert the symbol into the symbol table
    // only add to table if we are in the first parse iteration
    int result = -2;
    if (numberOfParseIterations == 0){
        result = insertSymbol(CURRENT_NODE->symbolTable, symbol);
    }
    // check if the result is 0 or not
    // 0 means that the symbol was already in the symbol table
    // so its a redeclaration error
    if(result == 0 && numberOfParseIterations == 0){
        printf("10. Error in ParameterList: %s is already declared\n", name);
        info.tk = token;
        info.er = redecIdentifier;
        return info;
    }


    // now we need to check if the token is a ',' or not
    // and loop until we reach a non ',' or error

    token = PeekNextToken();
    while(token.tp == SYMBOL && token.lx[0] == ','){
        // consume the ',' token
        token = GetNextToken();
        // consume the type token
        info = type();
        if(info.er != none){
            return info;
        }
        // keep track of the type by storing it in variable
        aType = info.tk.lx;
        // consume the identifier token
        token = GetNextToken();
        if(token.tp != ID){
            // printf("2. Error in ParameterList: identifier expected, but %s found\n", token.lx);
            info.tk = token;
            info.er = idExpected;
            return info;
        }
        // store the identifier name as a variable
        name = token.lx;
        // create a symbol
        symbol = constructSymbol(name, aType, kind, CURRENT_NODE->symbolTable->nextAvailableLocation);
        // insert the symbol into the symbol table
        int result = 1;
        if (numberOfParseIterations == 0){
            result = insertSymbol(CURRENT_NODE->symbolTable, symbol);
        }
        // check if the result is 0 or not
        // 0 means that the symbol was already in the symbol table
        // so its a redeclaration error
        if(result == 0 && numberOfParseIterations == 0){
            printf("11. Error in ParameterList: %s is already declared\n", name);
            info.tk = token;
            info.er = redecIdentifier;
            return info;
        }
        // check if the next token is a ',' or not
        token = PeekNextToken();
    }

    // now we need to return the token and no error

    info.tk = token;
    info.er = none;
    return info;


}

ParserInfo variableDeclarationStatement(){
    // rule: variableDeclarationStatement -> var type identifier { , identifier } ;
    // print message for debugging
    ParserInfo info;
    Token token;
    token = GetNextToken();
    if(token.tp != RESWORD || strcmp(token.lx, "var") != 0){
        // printf("1. Error in variableDeclarationStatement: var expected, but %s found\n", token.lx);
        info.tk = token;
        info.er = syntaxError;
        return info;
    }
    // it is var

    // keep track of the kind
    Kind kind = var;

    info = type();
    if(info.er != none){
        return info;
    }
    // it is the type

    // keep track of the type by storing it in variable
    char* aType = info.tk.lx;

    token = GetNextToken();
    if(token.tp != ID){
        // printf("2. Error in variableDeclarationStatement: identifier expected, but %s found\n", token.lx);
        info.tk = token;
        info.er = idExpected;
        return info;
    }
    // it is the identifier

    // store the identifier name as a variable
    char* name = token.lx;

    // create a symbol
    Symbol* symbol = constructSymbol(name, aType, kind, CURRENT_NODE->symbolTable->nextAvailableLocation);
    // insert the symbol into the symbol table
    int result = -2;
    if (numberOfParseIterations == 0){
        result = insertSymbolLocalScope(CURRENT_NODE->symbolTable, symbol);
    }
    // check if the result is 0 or not
    // 0 means that the symbol was already in the symbol table
    // so its a redeclaration error
    if(result == 0 && numberOfParseIterations == 0){
        printf("13. Error in ParameterList: %s is already declared\n", name);
        info.tk = token;
        info.er = redecIdentifier;
        return info;
    }
    token = PeekNextToken();
    if(token.tp == SYMBOL && token.lx[0] == ','){
        // need to enter a loop to consume all the identifiers and ',' tokens until we reach a ';' or error
        while(token.tp == SYMBOL && token.lx[0] == ','){
            token = GetNextToken();
            token = GetNextToken();
            if(token.tp != ID){
                // printf("3. Error in variableDeclarationStatement: identifier expected, but %s found\n", token.lx);
                info.tk = token;
                info.er = idExpected;
                return info;
            }
            // it is the identifier

            // store the identifier name as a variable
            name = token.lx;
            // create a symbol
            symbol = constructSymbol(name, aType, kind, CURRENT_NODE->symbolTable->nextAvailableLocation);
            // insert the symbol into the symbol table
            int result = -2;
            if (numberOfParseIterations == 0){
                result = insertSymbol(CURRENT_NODE->symbolTable, symbol);
            }
             // check if the result is 0 or not
            // 0 means that the symbol was already in the symbol table
            // so its a redeclaration error
            if(result == 0 && numberOfParseIterations == 0){
                printf("15. Error in ParameterList: %s is already declared\n", name);
                info.tk = token;
                info.er = redecIdentifier;
                return info;
            }
            token = PeekNextToken();
        }
        // now we need to check if the token is a ';' or not
        if(token.tp == SYMBOL && token.lx[0] == ';'){
            // consume the ';' token
            token = GetNextToken();
            info.tk = token;
            info.er = none;
            return info;
        }
        else{
            // printf("4. Error in variableDeclarationStatement: ; expected, but %s found\n", token.lx);
            // syntax error as ; is expected
            info.tk = token;
            info.er = semicolonExpected;
            return info;
        }
    }
    else if(token.tp == SYMBOL && token.lx[0] == ';'){
        // this is the end of the statement so we need to consume the ';' token
        token = GetNextToken();
        info.tk = token;
        info.er = none;
        return info;
    }
    else{
        // syntax error as ; is expected
        // printf("5. Error in variableDeclarationStatement: ; expected, but %s found\n", token.lx);
        info.tk = token;
        info.er = semicolonExpected;
        return info;
    }
}

ParserInfo classVariableDeclaration(){
    // rule: classVariableDeclaration -> (static | field) type identifier {, identifier} ;
    // print message for debugging
    ParserInfo info;
    Token token;
    token = GetNextToken();
    // check if the token is static or field

    if(token.tp != RESWORD || (strcmp(token.lx, "static") != 0 && strcmp(token.lx, "field") != 0)){
        // printf("1. Error in classVariableDeclaration: static or field expected, but %s found\n", token.lx);
        info.tk = token;
        info.er = classVarErr;
        return info;
    }
    // it is static or field

     // keep track of static or field by putting it in a variable
    char* rawKind = token.lx;
    // convert the raw kind to the kind
    Kind kind;
    if(strcmp(rawKind, "static") == 0){
        kind = staticVariable;
    }
    else{
        kind = fieldVariable;
    }

    info = type();
    if(info.er != none){
        return info;
    }
    // it is the type

    // keep track of the name of the type by putting it in a variable
    char* aType = info.tk.lx;

    token = GetNextToken();
    if(token.tp != ID){
        // printf("2. Error in classVariableDeclaration: identifier expected, but %s found\n", token.lx);
        info.tk = token;
        info.er = idExpected;
        return info;
    }
    // it is the identifier

    // keep track of the name of the identifier by putting it in a variable
    char* name = token.lx;

    

    
    // insert the symbol to the symbol table
    int result = -2;
    //printf("number of parse iterations: %d\n", numberOfParseIterations);
    if (numberOfParseIterations == 0){
        // add the variable to the symbol table
        Symbol* aSymbol = constructSymbol(name, aType, kind, CURRENT_NODE->symbolTable->nextAvailableLocation);
        result = insertSymbol(CURRENT_NODE->symbolTable, aSymbol);
        //printf("result: %d\n", result);
    }
    // check if the result is 0 or not
    // 0 means that the symbol was already in the symbol table
    // so its a redeclaration error
    //printf("result and number of parse iterations: %d %d\n", result, numberOfParseIterations);
    //printf("____________________________________________________\n");
    if(result == 0 && numberOfParseIterations == 0){
        // printf("16. Error in ParameterList: %s is already declared\n", name);
        info.tk = token;
        info.er = redecIdentifier;
        return info;
    }

    Symbol* symbol;

    token = PeekNextToken();
    if(token.tp == SYMBOL && token.lx[0] == ','){
        // need to enter a loop to consume all the identifiers and ',' tokens until we reach a ';' or error
        while(token.tp == SYMBOL && token.lx[0] == ','){
            token = GetNextToken();
            token = GetNextToken();
            if(token.tp != ID){
                // printf("3. Error in classVariableDeclaration: identifier expected, but %s found\n", token.lx);
                info.tk = token;
                info.er = idExpected;
                return info;
            }
            // this is the identifier so insert it to the symbol table
            symbol = constructSymbol(token.lx, aType, kind, CURRENT_NODE->symbolTable->nextAvailableLocation);
            // insert the symbol to the symbol table
            int result = 1;
            if (numberOfParseIterations == 0){
                result = insertSymbol(CURRENT_NODE->symbolTable, symbol);
            }
            // check if the result is 0 or not
            // 0 means that the symbol was already in the symbol table
            // so its a redeclaration error
            if(result == 0 && numberOfParseIterations == 0){
                printf("17. Error in ParameterList: %s is already declared\n", name);
                info.tk = token;
                info.er = redecIdentifier;
                return info;
            }

            token = PeekNextToken();
        }
        // now we need to check if the token is a ';' or not
        if(token.tp == SYMBOL && token.lx[0] == ';'){
            // consume the ';' token
            token = GetNextToken();
            info.tk = token;
            info.er = none;
            return info;
        }
        else{
            // printf("4. Error in classVariableDeclaration: ; expected, but %s found\n", token.lx);
            // syntax error as ; is expected
            info.tk = token;
            info.er = semicolonExpected;
            return info;
        } 

        return info;
    }
    else if(token.tp == SYMBOL && token.lx[0] == ';'){
        // this is the end of the statement so we need to consume the ';' token
        token = GetNextToken();
        info.tk = token;
        info.er = none;
        return info;
    }
    else{
        // printf("4. Error in classVariableDeclaration: ; expected, but %s found\n", token.lx);
        // syntax error as ; is expected
        info.tk = token;
        info.er = semicolonExpected;
        return info;
    }
}

ParserInfo arithmeticExpression(){
    // rule: arithmeticExpression -> term { ( + | - ) term }
    // print message for debugging
    ParserInfo info;
    Token token;

    info = term();
    if(info.er != none){
        return info;
    }
    // it is the term


    token = PeekNextToken();
    // Start a loop where we need to check if the next token is + or - and if yes, then we need to call term
    // If no error, peak next token and check if it is + or - again
    // If it is not + or - or error, then we need to return the info
    if (token.tp == SYMBOL && (token.lx[0] == '+' || token.lx[0] == '-')){
        while (token.tp == SYMBOL && (token.lx[0] == '+' || token.lx[0] == '-')){
            // consume the + or - token
            token = GetNextToken();
            info = term();
            if(info.er != none){
                return info;
            }

            if (numberOfParseIterations >=1){
                if(strcmp(token.lx, "+") == 0){
                    writeToFile("add\n");
                }
                else{
                    writeToFile("sub\n");
                }
            }
            
            token = PeekNextToken();
        }
        // return the info

        

        info.tk = token;
        info.er = none;
        return info;
    }
    else{
        info.tk = token;
        info.er = none;
        return info;
    }
    
}

ParserInfo relationalExpression(){
    // rule: relationalExpression -> arithmeticExpression { ( = | > | < ) ArithmeticExpression }
    // print message for debugging

    ParserInfo info;
    Token token;

    info = arithmeticExpression();
    if(info.er != none){
        return info;
    }
    // it is the arithmeticExpression

    // Start a loop where we need to check if the next token is =, >, or < and if yes, then we need to call arithmeticExpression
    // If no error, peak next token and check if it is =, >, or < again
    // If it is not =, >, or < or error, then we need to return the info
    token = PeekNextToken();
    if (token.tp == SYMBOL && (token.lx[0] == '=' || token.lx[0] == '>' || token.lx[0] == '<')){
        while (token.tp == SYMBOL && (token.lx[0] == '=' || token.lx[0] == '>' || token.lx[0] == '<')){
            // consume the =, >, or < token
            token = GetNextToken();
            info = arithmeticExpression();
            if(info.er != none){
                return info;
            }

            if (numberOfParseIterations >=1){
                if(strcmp(token.lx, "=") == 0){
                    writeToFile("eq\n");
                }
                else if(strcmp(token.lx, ">") == 0){
                    writeToFile("gt\n");
                }
                else{
                    writeToFile("lt\n");
                }
            }


            token = PeekNextToken();
        }
        // return the info
        info.tk = token;
        info.er = none;
        return info;
    }
    else{
        info.tk = token;
        info.er = none;
        return info;
    }
}

ParserInfo expression(){
    // rule: expression -> relationalExpression { ( & | | ) relationalExpression }
    // print message for debugging

    ParserInfo info;
    Token token;

    info = relationalExpression();
    if(info.er != none){
        return info;
    }
    // it is the relationalExpression

    // Start a loop where we need to check if the next token is & or | and if yes, then we need to call relationalExpression
    // If no error, peak next token and check if it is & or | again
    // If it is not & or | or error, then we need to return the info
    token = PeekNextToken();
    if (token.tp == SYMBOL && (token.lx[0] == '&' || token.lx[0] == '|')){
        while (token.tp == SYMBOL && (token.lx[0] == '&' || token.lx[0] == '|')){
            // consume the & or | token
            token = GetNextToken();
            info = relationalExpression();
            if(info.er != none){
                return info;
            }
            token = PeekNextToken();
        }
        // return the info
        info.tk = token;
        info.er = none;
        return info;
    }
    else{
        info.tk = token;
        info.er = none;
        return info;
    }

}

ParserInfo expressionList(){
    // rule: expressionList -> expression { , expression } | ε
    // print message for debugging

    ParserInfo info;
    Token token;
    expressionListCount = 0;
    // peek to see if its not an expression as it can be empty
    token = PeekNextToken();
    if (token.tp == SYMBOL && token.lx[0] == ')'){
        // return the info
        info.tk = token;
        info.er = none;
        return info;
    }

    
    info = expression();
    if (numberOfParseIterations >=1){
        expressionListCount++;
    }

    
    if(info.er != none){
        return info;
    }
    // it is the expression

    // Start a loop where we need to check if the next token is , and if yes, then we need to call expression
    // If no error, peak next token and check if it is , again
    // If it is not , or error, then we need to return the info
    token = PeekNextToken();
    if (token.tp == SYMBOL && token.lx[0] == ','){
        while (token.tp == SYMBOL && token.lx[0] == ','){
            // consume the , token
            token = GetNextToken();
            info = expression();
            
            if(info.er != none){
                return info;
            }
            token = PeekNextToken();
            if (numberOfParseIterations >=1){
                expressionListCount++;
            }
        }
        // return the info
        info.tk = token;
        info.er = none;
        return info;
    }
    else{
        info.tk = token;
        info.er = none;
        return info;
    }
}

ParserInfo returnStatement(){
    // rule: returnStatement -> return [ expression ] ;
    // print message for debugging

    ParserInfo info;
    Token token;

    token = GetNextToken();
    // consume the return identifier
    if (token.tp != RESWORD || strcmp(token.lx, "return") != 0){
        // printf("1. Error: return expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = syntaxError;
        return info;
    }

    // check for 1 or 0 occurrences of expression
    token = PeekNextToken();
    // zero occurrences
    if (token.tp == SYMBOL && token.lx[0] == ';'){
        
        if (numberOfParseIterations>=1){
            writeToFile("push constant 0\n");
            writeToFile("return\n");

        }


        // consume the ; token
        token = GetNextToken();
        info.tk = token;
        info.er = none;
        return info;
    }
    else{
        // check if token can be an expression, if yes, then call expression
        // this can be checked by first checking if its a valid factor
        // if it is a valid factor, then it can be an expression

        // check if it is a valid factor by checking if its a - or ~
        if (token.tp == SYMBOL && (token.lx[0] == '-' || token.lx[0] == '~')){
            // valid expression so consumer it
            info = expression();
            if(info.er != none){
                return info;
            }
        }
        // otherwise first part of factor can be epsilon so check if token is a valid operand
        else if (token.tp == ID || token.tp == INT || token.tp == STRING || token.tp == RESWORD || token.tp == SYMBOL && token.lx[0] == '('){
            // valid expression so consumer it
            info = expression();
            if(info.er != none){
                return info;
            }
        }
        
        // all valid expression should have been consumed by now so check for ; token

        token = PeekNextToken();
        if (token.tp != SYMBOL || token.lx[0] != ';'){
            // printf("2. Error: ; expected but %s found, line %d\n", token.lx, token.ln);
            info.tk = token;
            info.er = semicolonExpected;
            return info;
        }
        // consume the ; token
        token = GetNextToken();

        // else it is a valid return statement

        if (numberOfParseIterations>=1){
            //writeToFile("return\n");
            ;
        }

        info.tk = token;
        info.er = none;
        return info;
    }
}

ParserInfo term(){
    // rule: term -> factor { ( * | / ) factor }
    // print message for debugging

    ParserInfo info;
    Token token;

    info = factor();
    if(info.er != none){
        return info;
    }
    // it is the factor

    // Start a loop where we need to check if the next token is * or / and if yes, then we need to call factor
    // If no error, peak next token and check if it is * or / again
    // If it is not * or / or error, then we need to return the info
    token = PeekNextToken();
    if (token.tp == SYMBOL && (token.lx[0] == '*' || token.lx[0] == '/')){
        while (token.tp == SYMBOL && (token.lx[0] == '*' || token.lx[0] == '/')){
            // consume the * or / token
            token = GetNextToken();
            info = factor();
            if(info.er != none){
                return info;
            }

            // generate vm
            if (numberOfParseIterations>=1){
                if (token.lx[0] == '*'){
                    writeToFile("call Math.multiply 2\n");
                }
                else{
                    writeToFile("call Math.divide 2\n");
                }
            }
            
            token = PeekNextToken();
        }
        // return the info
        info.tk = token;
        info.er = none;
        return info;
    }
    else{
        info.tk = token;
        info.er = none;
        return info;
    }
}

ParserInfo factor(){
    // rule: factor -> ( - | ~ | ε ) operand
    // print message for debugging
    
    ParserInfo info;
    Token token;

    token = PeekNextToken();
    if (token.tp == SYMBOL && (token.lx[0] == '-' || token.lx[0] == '~')){
        // consume the - or ~ token
        token = GetNextToken();
    }

    info = operand();
    if(info.er != none){
        return info;
    }
    // it is the operand

    info.tk = token;
    info.er = none;
    return info;
}

ParserInfo subroutineCall(){
    // rule: subroutineCall -> identifier [ . identifier ] ( expressionList )
    // Note: the '(' and ')' are literal characters
    // print message for debugging

    ParserInfo info;
    Token token;
    char actualTestOfClass[100];
    char* jackClassName=NULL;
    char* jackSubroutineName=NULL;

    token = GetNextToken();
    // consume the identifier
    if (token.tp != ID){
        // printf("1. Error: identifier expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = idExpected;
        return info;
    }
    char* identifierOfClass = token.lx;
    // check if the identifier is in the symbol table
    TreeNode* classNodeOfIdentifier=NULL;
    Symbol* classSymbolOfIdentifier=NULL;
    int localSubroutine = 0;
    
    if (numberOfParseIterations >= 1){

        // peek to see if the next character is a . or (
        // if . its a class identifier
        // else, its a subroutine identifier so check scope
        Token copyOfCurrentToken = token;
        token = PeekNextToken();

        Symbol* varCheck = lookupSymbol(CURRENT_NODE->symbolTable, copyOfCurrentToken.lx);
        if (varCheck != NULL){
            if (varCheck->kind == var){
                writeToFile("push local ");
                // classSymbolOfIdentifier->number to str
                char str[12];
                sprintf(str, "%d", varCheck->number);
                writeToFile(str);
                writeToFile("\n");
            }
            else if (varCheck->kind == argument){
                writeToFile("push argument ");
                // classSymbolOfIdentifier->number to str
                char str[12];
                sprintf(str, "%d", varCheck->number);
                writeToFile(str);
                writeToFile("\n");
            }
            else if (varCheck->kind == fieldVariable){
                writeToFile("push this ");
                // classSymbolOfIdentifier->number to str
                char str[12];
                sprintf(str, "%d", varCheck->number);
                writeToFile(str);
                writeToFile("\n");
            }
            else if (varCheck->kind == staticVariable){
                writeToFile("push static ");
                // classSymbolOfIdentifier->number to str
                char str[12];
                sprintf(str, "%d", varCheck->number);
                writeToFile(str);
                writeToFile("\n");
            }
        }
        
        if (token.tp == SYMBOL && token.lx[0] == '.'){
            if (CURRENT_NODE==NULL){
                printf("1. Error: Current_node is NULL\n");
            }
            // check if this is a class identifier
            //printf("Current_node is: %s\n", CURRENT_NODE->symbolTable->className);
            classSymbolOfIdentifier = findClassNode(copyOfCurrentToken.lx);
            classNodeOfIdentifier = findClassNodeReturnNode(copyOfCurrentToken.lx);
            //printf("Was looking for class %s\n", copyOfCurrentToken.lx);
            //printf("classSymbolOfIdentifier that was found: %s\n", classSymbolOfIdentifier->name);
            
            //printf("identifierOfClass: %s\n", copyOfCurrentToken.lx);
            if(classSymbolOfIdentifier == NULL){
                //printf("this is a candidate for a library class %s\n", copyOfCurrentToken.lx);
                if(isJackLibrary(copyOfCurrentToken.lx) == 1){
                    //printf("This is a library class\n");
                    // copy the token.lx into actualTestOfClass
                    strcpy(actualTestOfClass, copyOfCurrentToken.lx);
                    // make jackClassName point to actualTestOfClass
                    jackClassName = actualTestOfClass;
                    // that means it is a library class
                }
                else{

                    // one more case
                    // the type of the identifier is a class name
                    // so use that instead
                    // so this identifier should be in the symbol table

                    // check if the identifier is in the symbol table
                    Symbol* symbol = lookupSymbolInCurrentScope(copyOfCurrentToken.lx);
                    if(symbol == NULL){
                        //printf("89. Error: %s is not declared\n", copyOfCurrentToken.lx);
                        info.tk = copyOfCurrentToken;
                        info.er = undecIdentifier;
                        return info;
                    }
                    else{
                        // get the type of the identifier
                        char* typeOfIdentifier = symbol->type;
                        //printf("typeOfIdentifier: %s\n", typeOfIdentifier);
                        // check if that type is a valid class in root symbol table
                        classSymbolOfIdentifier = findClassNode(typeOfIdentifier);
                        if (classSymbolOfIdentifier == NULL){

                            // the last check
                            // check if the typeOfIdentifier is a jack library class

                            if(isJackLibrary(typeOfIdentifier) == 1){
                                //printf("This is a library class\n");
                                // copy the token.lx into actualTestOfClass
                                strcpy(actualTestOfClass, typeOfIdentifier);
                                // make jackClassName point to actualTestOfClass
                                jackClassName = actualTestOfClass;
                                // that means it is a library class
                            }
                            else{
                                // printf("90. Error: %s is not declared\n", copyOfCurrentToken.lx);
                                info.tk = copyOfCurrentToken;
                                info.er = undecIdentifier;
                                return info;
                            }
                        }
                        else{
                            classNodeOfIdentifier = findClassNodeReturnNode(typeOfIdentifier);
                       }
                    }
                }


            }
        }
        else{
            // check if this is a subroutine identifier
            Symbol* symbol = lookupSymbolInCurrentScope(copyOfCurrentToken.lx);
            if(symbol == NULL){
                char* star = copyOfCurrentToken.lx;
                info.tk = copyOfCurrentToken;
                info.er = undecIdentifier;
                return info;
            }
                       // generate the call command
            writeToFile("call ");
            writeToFile(varCheck->name);
            writeToFile(" ");
            localSubroutine = 1;
        }

    }
    // peek to see if the next token is . or (
    token = PeekNextToken();
    char* identifierOfMethod;

    Token aTempToken;


    // if the next token is ., then we need to consume the . and the next identifier
    if (token.tp == SYMBOL && token.lx[0] == '.'){
        // consume the . token
        token = GetNextToken();
        token = GetNextToken();
        // consume the identifier
        if (token.tp != ID){
            // printf("2. Error: identifier expected but %s found, line %d\n", token.lx, token.ln);
            info.tk = token;
            info.er = idExpected;
            return info;
        }
        aTempToken = token;
        identifierOfMethod = aTempToken.lx;

        //printf("identifierOfMethod: %s\n", identifierOfMethod);

        // this id should be a method of the class above
        // so first get the address of the class and then access it symbol table to verify
        // whether this id is inside of it. If it is not, then it is an error

        int validJackMethod = 0;

        if (numberOfParseIterations >= 1){

            // we know that classNodeOfIdentifier is a valid class node
            // now check if it has the method identifierOfMethod
            
            if (jackClassName != NULL){
                    //printf("candidate for a jack method in class %s\n", jackClassName);
                    // see if the method of current token is a method of this class
                    if(isJackLibraryMethod(jackClassName, token.lx) == 1){
                        //printf("valid jack method\n");
                        validJackMethod = 1;
                        // if token.tx is printString, when we need to update printStringWasCalled to 1
                        if(strcmp(token.lx, "printString") == 0){
                            printStringWasCalled = 1;
                        }
                    }
                    else{
                        // printf("44. Error: %s is not declared\n", token.lx);
                        info.tk = token;
                        info.er = undecIdentifier;
                        return info;
                    }

            }
            else{
            Symbol* methodSymbol = lookupSymbol(classNodeOfIdentifier->symbolTable ,identifierOfMethod);
            if(methodSymbol == NULL){
                // printf("93. Error: %s is not declared\n", token.lx);
                info.tk = token;
                info.er = undecIdentifier;
                return info;
            }
            }
        }
    }

    // consume the ( token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '('){
        // printf("3. Error: ( expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = openParenExpected;
        return info;
    }

    

    // if printString was called and as such saved in temp variable
    // we will convert each letter of the string into a push constant command
    // and then call the printString function
    if (numberOfParseIterations>=1 && printStringWasCalled == 1){
        Token aTempToken = PeekNextToken();
        // printf("______________________\n");
        // printf("printString was called\n");
        // printf("info.tk.lx: %s\n", info.tk.lx);
        printString(aTempToken.lx);
        //printf("______________________\n");
        printStringWasCalled = 0;

    }
    
    

    info = expressionList();
    if(info.er != none){
        return info;
    }
    // it is the expressionList
    if (numberOfParseIterations>=1 && classNodeOfIdentifier != NULL){

        // if the class name is Fraction and method is reduce, push pointer 0
        if(strcmp(classNodeOfIdentifier->symbolTable->className, "Fraction") == 0 && strcmp(identifierOfMethod, "reduce") == 0){
            writeToFile("push pointer 0\n");
        }

        writeToFile("call ");
        writeToFile(classNodeOfIdentifier->symbolTable->className);
        writeToFile(".");
        writeToFile(identifierOfMethod);
        writeToFile(" ");

        // if the class name is Fraction and method is plus, increase expression count by 1
        if(strcmp(classNodeOfIdentifier->symbolTable->className, "Fraction") == 0 && strcmp(identifierOfMethod, "print") == 0){
            expressionListCount++;
        }

        // use expressionListCount to get the number of arguments
        char str[20];
        sprintf(str, "%d", expressionListCount);
        writeToFile(str);
        writeToFile("\n");
        expressionListCount = 0;

        writeToFile("pop temp 0\n");
    }
    else if (localSubroutine==1){
        // use expressionListCount to get the number of arguments
        char str[20];
        sprintf(str, "%d", expressionListCount);
        writeToFile(str);
        writeToFile("\n");
        expressionListCount = 0;

        writeToFile("pop temp 0\n");

    }

    // consume the ) token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != ')'){
        // printf("4. Error: ) expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = closeParenExpected;
        return info;
    }

   

    info.tk = token;
    info.er = none;
    return info;
}

// function which will deal with printString vm code generation
// it will take the string and convert it into a series of push constant commands
// and then call the printString function









ParserInfo doStatement(){
    // rule: doStatement -> do subroutineCall ;
    // print message for debugging

    ParserInfo info;
    Token token;

    token = GetNextToken();
    // consume the do identifier
    if (token.tp != RESWORD || strcmp(token.lx, "do") != 0){
        // printf("1. Error: do expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = syntaxError;
        return info;
    }

    info = subroutineCall();
    if(info.er != none){
        return info;
    }
    // it is the subroutineCall

    // consume the ; token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != ';'){
        // printf("2. Error: ; expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = semicolonExpected;
        return info;
    }



    // since this is a do statement, we need to pop the return value of the subroutine call
    if (numberOfParseIterations>=1 && temporaryVariable != NULL){
        // write the temp variable to the output file as it contains the subroutineCall of the do statement
        writeToFile(temporaryVariable);
        writeToFile("pop temp 0\n");
        temporaryVariable = NULL;
    }

    info.tk = token;
    info.er = none;
    return info;
}

ParserInfo operand(){
    // rule: operand -> int | identifier [.identifier ] [ [ expression ] | (expressionList) ] | (expression) | string | true | false | null | this
    // print message for debugging

    ParserInfo info;
    Token token;

    char actualTestOfClass[100];
    char* jackClassName=NULL;

    char* identifierOfClass = token.lx;
    // check if the identifier is in the symbol table
    TreeNode* classNodeOfIdentifier=NULL;
    Symbol* classSymbolOfIdentifier=NULL;

    token = PeekNextToken();
    // first check if it is an int | string | true | false | null | this
    if (token.tp == INT || token.tp == STRING || token.tp == RESWORD){
        // check if it is a RESWORD, then it must be true | false | null | this
        if (token.tp == RESWORD){
            if (token.tp == RESWORD && (strcmp(token.lx, "true") == 0 || strcmp(token.lx, "false") == 0 || strcmp(token.lx, "null") == 0 || strcmp(token.lx, "this") == 0)){
                // if the token is this, then we need to push pointer 0
                if (numberOfParseIterations>=1 && strcmp(token.lx, "this") == 0){
                    writeToFile("push pointer 0\n");
                }


                // consume the token
                token = GetNextToken();
                info.tk = token;
                info.er = none;
                return info;
            }
            // else it is an error as the token is not true | false | null | this
            else{
                // printf("1. Error: true | false | null | this expected but %s found, line %d\n", token.lx, token.ln);
                info.tk = token;
                info.er = syntaxError;
                return info;
            }
        }
        // else it is an int or string
        else{

            // if token is a int, generate vm code for it
            if (token.tp == INT && numberOfParseIterations >= 1){
                writeToFile("push constant ");
                writeToFile(token.lx);
                writeToFile("\n");
            }
            // else if the token is a string, put it into tokenForHoldingStrings
            else if (token.tp == STRING){
                ;
            }

        
            // consume the token
            token = GetNextToken();
            info.tk = token;
            info.er = none;
            return info;
        }
    }

    Token aTempToken;

    // check if it is a identifier type
    if (token.tp == ID){
        // consume the token
        token = GetNextToken();


        // if the token is an identifier, we need to check if its in the symbol table
        // if not, its an undeclared identifier error
        // check if the identifier is in the symbol table
        if (numberOfParseIterations >= 1){

            // peek to see if the next character is a . or (
            // if . its a class identifier
            // else, its a subroutine identifier so check scope
            Token copyOfCurrentToken = token;
            //printf("copy of past token is: %s\n", copyOfCurrentToken.lx);
            token = PeekNextToken();
            // check if this identifier is a local variable
            Symbol* varCheck = lookupSymbol(CURRENT_NODE->symbolTable, copyOfCurrentToken.lx);
            if (varCheck != NULL){

                if (varCheck->kind == var){
                    writeToFile("push local ");
                    // classSymbolOfIdentifier->number to str
                    char str[12];
                    sprintf(str, "%d", varCheck->number);
                    writeToFile(str);
                    writeToFile("\n");
                }
                else if (varCheck->kind == argument){
                    writeToFile("push argument ");
                    // classSymbolOfIdentifier->number to str
                    char str[12];
                    sprintf(str, "%d", varCheck->number);
                    writeToFile(str);
                    writeToFile("\n");
                }
                else if (varCheck->kind == fieldVariable){
                    writeToFile("push this ");
                    // classSymbolOfIdentifier->number to str
                    char str[12];
                    sprintf(str, "%d", varCheck->number);
                    writeToFile(str);
                    writeToFile("\n");
                }
                else if (varCheck->kind == staticVariable){
                    writeToFile("push static ");
                    // classSymbolOfIdentifier->number to str
                    char str[12];
                    sprintf(str, "%d", varCheck->number);
                    writeToFile(str);
                    writeToFile("\n");
                }
            }
            //printf("next token is: %s\n", token.lx);
            if (token.tp == SYMBOL && token.lx[0] == '.'){
                if (CURRENT_NODE==NULL){
                    //printf("1. Error: Current_node is NULL\n");
                    ;
                }

                


                // check if this is a class identifier
                //printf("Current_node is: %s\n", CURRENT_NODE->symbolTable->className);
                classSymbolOfIdentifier = findClassNode(copyOfCurrentToken.lx);
                classNodeOfIdentifier = findClassNodeReturnNode(copyOfCurrentToken.lx);
                //printf("Was looking for class %s\n", copyOfCurrentToken.lx);
                //printf("classSymbolOfIdentifier that was found: %s\n", classSymbolOfIdentifier->name);
                
                //printf("identifierOfClass: %s\n", copyOfCurrentToken.lx);
                if(classSymbolOfIdentifier == NULL){
                    //printf("this is a candidate for a library class %s\n", copyOfCurrentToken.lx);
                    if(isJackLibrary(copyOfCurrentToken.lx) == 1){
                        //printf("This is a library class\n");
                        // copy the token.lx into actualTestOfClass
                        strcpy(actualTestOfClass, copyOfCurrentToken.lx);
                        // make jackClassName point to actualTestOfClass
                        jackClassName = actualTestOfClass;
                        // that means it is a library class
                    }
                    else{

                        Symbol* symbol = lookupSymbolInCurrentScope(copyOfCurrentToken.lx);
                        if(symbol == NULL){
                            //printf("89. Error: %s is not declared\n", copyOfCurrentToken.lx);
                            info.tk = copyOfCurrentToken;
                            info.er = undecIdentifier;
                            return info;
                        }
                        else{
                            // get the type of the identifier
                            char* typeOfIdentifier = symbol->type;
                            //printf("typeOfIdentifier: %s\n", typeOfIdentifier);
                            // check if that type is a valid class in root symbol table
                            classSymbolOfIdentifier = findClassNode(typeOfIdentifier);
                            if (classSymbolOfIdentifier == NULL){
                                // the last check
                                // check if the typeOfIdentifier is a jack library class

                                if(isJackLibrary(typeOfIdentifier) == 1){
                                    //printf("This is a library class\n");
                                    // copy the token.lx into actualTestOfClass
                                    strcpy(actualTestOfClass, typeOfIdentifier);
                                    // make jackClassName point to actualTestOfClass
                                    jackClassName = actualTestOfClass;
                                    // that means it is a library class
                                    if(strcmp(token.lx, "readInt") == 0){
                                        readIntIsCalled = 1;
                                    }
                                }
                                else{
                                    // printf("90. Error: %s is not declared\n", copyOfCurrentToken.lx);
                                    info.tk = copyOfCurrentToken;
                                    info.er = undecIdentifier;
                                    return info;
                                }
                            }
                            else{
                                classNodeOfIdentifier = findClassNodeReturnNode(typeOfIdentifier);
                            }
                        
                        }
                    }
                }
                
            }
            else{
                // check if this is a subroutine identifier
                Symbol* symbol = lookupSymbolInCurrentScope(copyOfCurrentToken.lx);
                if(symbol == NULL){
                    char* star = copyOfCurrentToken.lx;
                    info.tk = copyOfCurrentToken;
                    info.er = undecIdentifier;
                    return info;
                }
            }   
        }
        char* identifierOfMethod=NULL;
        // peek to see if the next token is .
        token = PeekNextToken();
        // if the next token is ., then we need to consume the . and the next identifier
        if (token.tp == SYMBOL && token.lx[0] == '.'){
            // consume the . token
            token = GetNextToken();
            token = GetNextToken();
            // consume the identifier
            if (token.tp != ID){
                // printf("2. Error: identifier expected but %s found, line %d\n", token.lx, token.ln);
                info.tk = token;
                info.er = idExpected;
                return info;
            }
            // if the token is an identifier, we need to check if its in the symbol table
            // if not, its an undeclared identifier error
            //printf("identifierOfMethod: %s\n", token.lx);
            //printf("WHY ARE WE HERE?\n");
            if(numberOfParseIterations >= 1){
                aTempToken = token;
                identifierOfMethod = aTempToken.lx;

                //printf("identifierOfMethod: %s\n", identifierOfMethod);
                // this id should be a method of the class above
                // so first get the address of the class and then access it symbol table to verify
                // whether this id is inside of it. If it is not, then it is an error
                int validJackMethod = 0;
                // we know that classNodeOfIdentifier is a valid class node
                // now check if it has the method identifierOfMethod
                
                if (jackClassName != NULL){
                    // see if the method of current token is a method of this class
                    if(isJackLibraryMethod(jackClassName, token.lx) == 1){
                        validJackMethod = 1;
                        if(strcmp(token.lx, "readInt") == 0){
                            readIntIsCalled = 1;
                        }
                        if(strcmp(token.lx, "new") == 0 || strcmp(jackClassName, "Array") == 0){
                            callNewArray = 1;
                        }
                    }
                    else{
                        // printf("44. Error: %s is not declared\n", token.lx);
                        info.tk = token;
                        info.er = undecIdentifier;
                        return info;
                    }
                }
                else{

                    Symbol* methodSymbol = lookupSymbol(classNodeOfIdentifier->symbolTable ,identifierOfMethod);

                    if(methodSymbol == NULL){
                        //printf("93. Error: %s is not declared\n", token.lx);
                        info.tk = token;
                        info.er = undecIdentifier;
                        return info;
                    }
                }
            }
        }
        // peek to see if the next token is [ or (
        token = PeekNextToken();
        // check if it is a [ type
        if (token.tp == SYMBOL && token.lx[0] == '['){
            // consume the [ token
            token = GetNextToken();
            info = expression();
            if(info.er != none){
                return info;
            }
            // it is the expression

            // consume the ] token
            token = GetNextToken();
            if (token.tp != SYMBOL || token.lx[0] != ']'){
                // printf("3. Error: ] expected but %s found, line %d\n", token.lx, token.ln);
                info.tk = token;
                info.er = closeBracketExpected;
                return info;
            }
            // it is the ] token
            // return the token
            info.tk = token;
            info.er = none;
            return info;
        }
        // check if it is a ( type
        else if (token.tp == SYMBOL && token.lx[0] == '('){
            // consume the ( token
            token = GetNextToken();
            // write code generation for this class and method
            if (numberOfParseIterations>=1 && readIntIsCalled == 1){
                Token aTempToken = PeekNextToken();
                printString(aTempToken.lx);
                readIntIsCalled = 0;

                writeToFile("call Keyboard.readInt 1\n");
                
            }
            info = expressionList();
            if(info.er != none){
                return info;
            }
            // it is the expressionList



            
            if (numberOfParseIterations>=1 && classNodeOfIdentifier != NULL){
                writeToFile("call ");
                writeToFile(classNodeOfIdentifier->symbolTable->className);
                writeToFile(".");
                writeToFile(identifierOfMethod);
                writeToFile(" ");

                // if the class name is Fraction and method is plus, increase expression count by 1
                if(strcmp(classNodeOfIdentifier->symbolTable->className, "Fraction") == 0 && strcmp(identifierOfMethod, "plus") == 0){
                    expressionListCount++;
                }

                // use expressionListCount to get the number of arguments
                char str[20];
                sprintf(str, "%d", expressionListCount);
                writeToFile(str);
                writeToFile("\n");
                expressionListCount = 0;
            }
            if (callNewArray == 1){
                writeToFile("call Array.new 1\n");
                callNewArray = 0;
            }


            // consume the ) token
            token = GetNextToken();
            if (token.tp != SYMBOL || token.lx[0] != ')'){
                // printf("4. Error: ) expected but %s found, line %d\n", token.lx, token.ln);
                info.tk = token;
                info.er = closeParenExpected;
                return info;
            }
            // it is the ) token
            // return the token
            info.tk = token;
            info.er = none;
            return info;
        }
        // no errors, return the token
        info.tk = token;
        info.er = none;
        return info;

    }
    // check if it is a ( type
    if (token.tp == SYMBOL && token.lx[0] == '('){
        // consume the ( token
        token = GetNextToken();
        info = expression();
        if(info.er != none){
            return info;
        }
        // it is the expression

        // consume the ) token
        token = GetNextToken();
        if (token.tp != SYMBOL || token.lx[0] != ')'){
            // printf("5. Error: ) expected but %s found, line %d\n", token.lx, token.ln);
            info.tk = token;
            info.er = closeParenExpected;
            return info;
        }
        // it is the ) token
        // return the token
        info.tk = token;
        info.er = none;
        return info;
    }
    // else it is an error
    // printf("6. Error: operand expected but %s found, line %d\n", token.lx, token.ln);
    info.tk = token;
    info.er = syntaxError;
    return info;
}

ParserInfo whileStatement(){
    // rule: whileStatement -> while ( expression ) { {statement} }
    // print message for debugging

    ParserInfo info;
    Token token;

    // consume the while token
    token = GetNextToken();
    if (token.tp != RESWORD || strcmp(token.lx, "while") != 0){
        // printf("7. Error: while expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = syntaxError;
        return info;
    }
    if (numberOfParseIterations>=1){
        writeToFile("label WHILE_EXP");
        // convert numberOfWhileStatements to string
        char str[20];
        sprintf(str, "%d", numberOfWhileStatements);
        writeToFile(str);
        writeToFile("\n");
    }
    // it is the while token

    // consume the ( token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '('){
        // printf("8. Error: ( expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = openParenExpected;
        return info;
    }
    // it is the ( token

    info = expression();
    if(info.er != none){
        return info;
    }
    // it is the expression

    // consume the ) token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != ')'){
        // printf("9. Error: ) expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = closeParenExpected;
        return info;
    }
    // it is the ) token
    if (numberOfParseIterations>=1){
        writeToFile("not\n");
        writeToFile("if-goto WHILE_END");
        // convert numberOfWhileStatements to string
        char str[20];
        sprintf(str, "%d", numberOfWhileStatements);
        writeToFile(str);
        writeToFile("\n");
    }

    // consume the { token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '{'){
        // printf("10. Error: { expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = openBraceExpected;
        return info;
    }
    // it is the { token

    // peek to see if the next token is a valid statement
    // if yes, then consume the statement and loop until the next token is not a valid statement
    // if no, its ok because a statement is optional
    token = PeekNextToken();
    if (token.tp == RESWORD && (strcmp(token.lx, "if") == 0 || strcmp(token.lx, "while") == 0 || strcmp(token.lx, "return") == 0) || strcmp(token.lx, "let") == 0 || strcmp(token.lx, "do") == 0 || strcmp(token.lx, "var") == 0){
        // it is a valid statement
        // consume the statement and loop until the next token is not a valid statement
        while (token.tp == RESWORD && (strcmp(token.lx, "if") == 0 || strcmp(token.lx, "while") == 0 || strcmp(token.lx, "return") == 0) || strcmp(token.lx, "let") == 0 || strcmp(token.lx, "do") == 0 || strcmp(token.lx, "var") == 0){
            info = statement();
            if(info.er != none){
                return info;
            }
            // it is the statement

            // peek to see if the next token is a valid statement
            token = PeekNextToken();
        }
    }
    // all statements have been consumed

    // consume the } token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '}'){
        // printf("11. Error: } expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = closeBraceExpected;
        return info;
    }
    // it is the } token

    // no errors, return the token
    info.tk = token;
    info.er = none;
    return info;
}

ParserInfo ifStatement(){
    // rule: ifStatement -> if ( expression ) { {statement} } [else { {statement} }]
    // print message for debugging

    ParserInfo info;
    Token token;

    // consume the if token
    token = GetNextToken();
    if (token.tp != RESWORD || strcmp(token.lx, "if") != 0){
        // printf("12. Error: if expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = syntaxError;
        return info;
    }
    // it is the if token

    // consume the ( token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '('){
        // printf("13. Error: ( expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = openParenExpected;
        return info;
    }
    // it is the ( token

    info = expression();
    if(info.er != none){
        return info;
    }
    // it is the expression

    // consume the ) token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != ')'){
        // printf("14. Error: ) expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = closeParenExpected;
        return info;
    }
    // it is the ) token

    // consume the { token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '{'){
        // printf("15. Error: { expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = openBraceExpected;
        return info;
    }
    // it is the { token

    // peek to see if the next token is a valid statement
    // if yes, then consume the statement and loop until the next token is not a valid statement
    // if no, its ok because a statement is optional
    token = PeekNextToken();
    if (token.tp == RESWORD && (strcmp(token.lx, "if") == 0 || strcmp(token.lx, "while") == 0 || strcmp(token.lx, "return") == 0) || strcmp(token.lx, "let") == 0 || strcmp(token.lx, "do") == 0 || strcmp(token.lx, "var") == 0){
        // it is a valid statement
        // consume the statement and loop until the next token is not a valid statement
        while (token.tp == RESWORD && (strcmp(token.lx, "if") == 0 || strcmp(token.lx, "while") == 0 || strcmp(token.lx, "return") == 0) || strcmp(token.lx, "let") == 0 || strcmp(token.lx, "do") == 0 || strcmp(token.lx, "var") == 0){
            info = statement();
            if(info.er != none){
                return info;
            }
            // it is the statement

            // peek to see if the next token is a valid statement
            token = PeekNextToken();
        }
    }
    // all statements have been consumed

    // consume the } token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '}'){
        // printf("16. Error: } expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = closeBraceExpected;
        return info;
    }
    // it is the } token

    // peek to see if the next token is else
    token = PeekNextToken();

    // if it is, then we need to consume the else token
    if (token.tp == RESWORD && strcmp(token.lx, "else") == 0){
        // consume the else token
        token = GetNextToken();
        if (token.tp != RESWORD || strcmp(token.lx, "else") != 0){
            // printf("17. Error: else expected but %s found, line %d\n", token.lx, token.ln);
            info.tk = token;
            info.er = syntaxError;
            return info;
        }
        // it is the else token

        // consume the { token
        token = GetNextToken();
        if (token.tp != SYMBOL || token.lx[0] != '{'){
            // printf("18. Error: { expected but %s found, line %d\n", token.lx, token.ln);
            info.tk = token;
            info.er = openBraceExpected;
            return info;
        }
        // it is the { token

        // peek to see if the next token is a valid statement
        // if yes, then consume the statement and loop until the next token is not a valid statement
        // if no, its ok because a statement is optional
        token = PeekNextToken();
        if (token.tp == RESWORD && (strcmp(token.lx, "if") == 0 || strcmp(token.lx, "while") == 0 || strcmp(token.lx, "return") == 0) || strcmp(token.lx, "let") == 0 || strcmp(token.lx, "do") == 0 || strcmp(token.lx, "var") == 0){
            // it is a valid statement
            // consume the statement and loop until the next token is not a valid statement
            while (token.tp == RESWORD && (strcmp(token.lx, "if") == 0 || strcmp(token.lx, "while") == 0 || strcmp(token.lx, "return") == 0) || strcmp(token.lx, "let") == 0 || strcmp(token.lx, "do") == 0 || strcmp(token.lx, "var") == 0){
                info = statement();
                if(info.er != none){
                    return info;
                }
                // it is the statement

                // peek to see if the next token is a valid statement
                token = PeekNextToken();
            }
        }
        

        // consume the } token
        token = GetNextToken();
        if (token.tp != SYMBOL || token.lx[0] != '}'){
            // printf("19. Error: } expected but %s found, line %d\n", token.lx, token.ln);
            info.tk = token;
            info.er = closeBraceExpected;
            return info;
        }
        // it is the } token

        // no errors, return the token
        info.tk = token;
        info.er = none;
        return info;
    }
        
    else{
        // it is not the else token so no error, return token
        info.tk = token;
        info.er = none;
        return info;
    }
}

ParserInfo letStatement(){
    // rule: letStatement -> let identifier [ [ expression ] ] = expression ;
    // print message for debugging

    ParserInfo info;
    Token token;

    // consume the let token
    token = GetNextToken();
    if (token.tp != RESWORD || strcmp(token.lx, "let") != 0){
        // printf("20. Error: let expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = syntaxError;
        return info;
    }
    // it is the let token

    // consume the identifier token
    token = GetNextToken();
    if (token.tp != ID){
        // printf("21. Error: identifier expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = idExpected;
        return info;
    }

    // check if the ID is already declared in the current scope, if not, return error
    Symbol* lookupSymbol = lookupSymbolInCurrentScope(token.lx);
    if(lookupSymbol == NULL){
        // printf("21. Error: identifier %s not declared in current scope, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = undecIdentifier;
        return info;
    }
    

    // peek to see if the next token is [
    token = PeekNextToken();

    // if it is, then we need to consume the [ token
    if (token.tp == SYMBOL && token.lx[0] == '['){
        // this must be an array

        

        // consume the [ token
        token = GetNextToken();
        if (token.tp != SYMBOL || token.lx[0] != '['){
            // printf("22. Error: [ expected but %s found, line %d\n", token.lx, token.ln);
            info.tk = token;
            info.er = syntaxError;
            return info;
        }
        // it is the [ token

        info = expression();
        if(info.er != none){
            return info;
        }

        if (numberOfParseIterations>=1){
            // push the array name to the array stack
            writeToFile("push ");
            Kind aKind = lookupSymbol->kind;
            if (aKind == staticVariable){
                writeToFile("static ");
            }
            else if (aKind == fieldVariable){
                writeToFile("this ");
            }
            else if (aKind == argument){
                writeToFile("argument ");
            }
            else if (aKind == var){
                writeToFile("local ");
            }
            // convert lookupSymbol->number to string
            char str[12];
            sprintf(str, "%d", lookupSymbol->number);
            writeToFile(str);
            writeToFile("\n");
            writeToFile("add\n");
        }
        // it is the expression

        // consume the ] token
        token = GetNextToken();
        if (token.tp != SYMBOL || token.lx[0] != ']'){
            // printf("23. Error: ] expected but %s found, line %d\n", token.lx, token.ln);
            info.tk = token;
            info.er = syntaxError;
            return info;
        }
        // it is the ] token
    }

    // consume the = token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '='){
        // printf("24. Error: = expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = equalExpected;
        return info;
    }
    // it is the = token

    info = expression();
    if(info.er != none){
        return info;
    }
    // it is the expression

    // consume the ; token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != ';'){
        // printf("25. Error: ; expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = semicolonExpected;
        return info;
    }
    // it is the ; token

    // since this is a let statement, we want to assign the value of the expression to the identifier
    // and we express this in the VM code
    if (numberOfParseIterations>=1){

        if (lookupSymbol->kind == var){
            int num = lookupSymbol->number;
            char str[20];
            sprintf(str, "%d", num);
            writeToFile("pop local ");
            writeToFile(str);
            writeToFile("\n");
        }
        else if (lookupSymbol->kind == argument){
            int num = lookupSymbol->number;
            char str[20];
            sprintf(str, "%d", num);
            writeToFile("pop argument ");
            writeToFile(str);
            writeToFile("\n");
        }
        else if (lookupSymbol->kind == fieldVariable){
            int num = lookupSymbol->number;
            char str[20];
            sprintf(str, "%d", num);
            writeToFile("pop this ");
            writeToFile(str);
            writeToFile("\n");
        }
        else if (lookupSymbol->kind == staticVariable){
            int num = lookupSymbol->number;
            char str[20];
            sprintf(str, "%d", num);
            writeToFile("pop static ");
            writeToFile(str);
            writeToFile("\n");
        }
        else if (lookupSymbol->kind == class){
            int num = lookupSymbol->number;
            char str[20];
            sprintf(str, "%d", num);
            writeToFile("pop this ");
            writeToFile(str);
            writeToFile("\n");
        }

    }

    // no errors, return the token
    info.tk = token;
    info.er = none;
    return info;
}

ParserInfo statement(){
    // rule: statement ->  variableDeclarationStatement | letStatement | ifStatement | whileStatement | doStatement | returnStatement
    // print message for debugging

    ParserInfo info;
    Token token;

    // peek to see if the next token is var
    // if yes, then call variableDeclarationStatement
    token = PeekNextToken();
    if (token.tp == RESWORD && strcmp(token.lx, "var") == 0){
        info = variableDeclarationStatement();
        if(info.er != none){
            return info;
        }
        // it is the variableDeclarationStatement
        return info;
    }
    // else if the next token is let, then call letStatement
    else if (token.tp == RESWORD && strcmp(token.lx, "let") == 0){
        info = letStatement();
        if(info.er != none){
            return info;
        }
        // it is the letStatement
        return info;
    }
    // else if the next token is if, then call ifStatement
    else if (token.tp == RESWORD && strcmp(token.lx, "if") == 0){
        info = ifStatement();
        if(info.er != none){
            return info;
        }
        // it is the ifStatement
        return info;
    }
    // else if the next token is while, then call whileStatement
    else if (token.tp == RESWORD && strcmp(token.lx, "while") == 0){
        info = whileStatement();
        if(info.er != none){
            return info;
        }
        // it is the whileStatement
        return info;
    }
    // else if the next token is do, then call doStatement
    else if (token.tp == RESWORD && strcmp(token.lx, "do") == 0){
        info = doStatement();
        if(info.er != none){
            return info;
        }
        // it is the doStatement
        return info;
    }
    // else if the next token is return, then call returnStatement
    else if (token.tp == RESWORD && strcmp(token.lx, "return") == 0){
        info = returnStatement();
        if(info.er != none){
            return info;
        }
        // it is the returnStatement
        return info;
    }
    // else, there is an error
    else{
        // printf("26. Error: statement expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = syntaxError;
        return info;
    }
}

ParserInfo subroutineBody(){
    // rule: subroutineBody ->  { {statement} }
    // print message for debugging

    ParserInfo info;
    Token token;

    // consume the { token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '{'){
        // printf("27. Error: { expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = openBraceExpected;
        return info;
    }
    // it is the { token

    // peek to see if the next token is a statement
    token = PeekNextToken();
    while (token.tp == RESWORD && (strcmp(token.lx, "var") == 0 || strcmp(token.lx, "let") == 0 || strcmp(token.lx, "if") == 0 || strcmp(token.lx, "while") == 0 || strcmp(token.lx, "do") == 0 || strcmp(token.lx, "return") == 0)){
        info = statement();
        if(info.er != none){
            return info;
        }
        // it is the statement

        // peek to see if the next token is a statement
        token = PeekNextToken();
    }
    // it is not a statement

    // consume the } token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '}'){
        // printf("28. Error: } expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = closeBraceExpected;
        return info;
    }

    

    // no errors, return the token
    info.tk = token;
    info.er = none;
    return info; 
}

ParserInfo subroutineDeclaration(){
    // rule: subroutineDeclaration ->  (constructor | function | method) (type|void) identifier (paramList) subroutineBody
    // print message for debugging
    
    ParserInfo info;
    Token token;
    isConstructor = 0;
    isMethod = 0;

    // consume the constructor, function, or method token
    token = GetNextToken();
    if (token.tp != RESWORD || (strcmp(token.lx, "constructor") != 0 && strcmp(token.lx, "function") != 0 && strcmp(token.lx, "method") != 0)){
        // printf("28. Error: constructor, function, or method expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = subroutineDeclarErr;
        return info;
    }
    // it is the constructor, function, or method token
    
    // if its a constructor, write vm code
    if(strcmp(token.lx, "constructor") == 0){
        isConstructor = 1;
    }
    // if its a method, write vm code
    else if(strcmp(token.lx, "method") == 0){
        isMethod = 1;
    }

    // peek next token to see if its void
    token = PeekNextToken();
    // save the type of the subroutine

    char* aType;

    if (token.tp == RESWORD && strcmp(token.lx, "void") == 0){
        aType = "void";
        // consume the void token
        token = GetNextToken();
        // it is the void token
    }
    // else, it is a type
    else{
        info = type();
        if(info.er != none){
            return info;
        }
        // it is the type
        aType = info.tk.lx;
    }

    // consume the identifier token
    token = GetNextToken();
    if (token.tp != ID){
        // printf("29. Error: identifier expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = idExpected;
        return info;
    }
    // it is the identifier token

    // save the name of the subroutine
    char* aName = token.lx;
    // save the kind
    Kind kind = subroutine;

    // pick out where the subroutine symbol table entry should be placed
    // if the current node is a class, then the subroutine symbol table entry should be placed in the current node's symbol table
    // if the current node is a subroutine, then the subroutine symbol table entry should be placed in the current node's parent's symbol table




    // construct the symbol
    // insert the symbol to the symbol table
    
    // now the class symbol table has the subroutine symbol
    // a new symbol table is created for the subroutine
    // the subroutine symbol table is the child of the class symbol table
    int result = 1;
    Symbol* aSymbol;
    if (numberOfParseIterations == 0){

        TreeNode* theParent;

        // if the current node is a class, that is the parent of the method
        // else, find the parent of the method

        if (CURRENT_NODE->nodeType == classNode){
            theParent = CURRENT_NODE;
        }
        else{
            theParent = CURRENT_NODE->parent;
        }
        if (CURRENT_NODE==NULL){
                printf("11. Error: Current_node is NULL\n");
            }
        //CURRENT_NODE is %s\n", CURRENT_NODE->symbolTable->className);
        CURRENT_NODE = initNode(theParent, aName, methodNode);
        //printf("AFTER CHANGE: CURRENT_NODE is %s\n", CURRENT_NODE->symbolTable->className);
        if (CURRENT_NODE==NULL){
                printf("12. Error: Current_node is NULL\n");
            }
        if (isMethod){
            // create a 'this' symbol for the method
            aSymbol = constructSymbol("this", CURRENT_NODE->parent->symbolTable->className, argument, CURRENT_NODE->symbolTable->nextAvailableLocation);
            // insert the symbol to the symbol table
            result = insertSymbol(CURRENT_NODE->symbolTable, aSymbol);
            isMethod = 0;
        }
    }
    else{
        if (CURRENT_NODE==NULL){
                //printf("Error: Current_node is NULL\n");
                ;
            }

        TreeNode* theParent;

        if (CURRENT_NODE->nodeType == classNode){
            theParent = CURRENT_NODE;
        }
        else{
            theParent = CURRENT_NODE->parent;
        }
        
        //printf("56. CURRENT_NODE is %s\n", CURRENT_NODE->symbolTable->className);
        CURRENT_NODE = findMethodNode(aName, theParent);
        //printf("CURRENT_NODE is %s\n", CURRENT_NODE->symbolTable->className);
        //printf("PARENT OF CURRENT_NODE is %s\n", CURRENT_NODE->parent->symbolTable->className);
        
    }
    // check if the result is 0 or not
    // 0 means that the symbol was already in the symbol table
    // so its a redeclaration error
    if(result == 0 && numberOfParseIterations == 0){
        printf("2. Error in ParameterList: %s is already declared\n", aName);
        info.tk = token;
        info.er = redecIdentifier;
        return info;
    }

    // add to parent of the current node
    result = 1;
    if (numberOfParseIterations == 0){
        Symbol* aSymbolToBeAddedToClass = constructSymbol(aName, aType, kind, CURRENT_NODE->parent->symbolTable->nextAvailableLocation);
        result = insertSymbol(CURRENT_NODE->parent->symbolTable, aSymbolToBeAddedToClass);
    }
    // check if the result is 0 or not
    // 0 means that the symbol was already in the symbol table
    // so its a redeclaration error
    if(result == 0 && numberOfParseIterations == 0){
        printf("1. Error in ParameterList: %s is already declared\n", aName);
        info.tk = token;
        info.er = redecIdentifier;
        return info;
    }

    // write to .vm file

    if (numberOfParseIterations>=1){
        writeToFile("function ");
        writeToFile(CURRENT_NODE->parent->symbolTable->className);
        writeToFile(".");
        writeToFile(aName);
        // calculate the number of local variables
        int numberOfLocalVariables = calculateNumberOfLocalVariablesInCurrentScope(CURRENT_NODE->symbolTable);
        writeToFile(" ");
        char* numberOfLocalVariablesString = (char*)malloc(sizeof(char)*10);
        sprintf(numberOfLocalVariablesString, "%d", numberOfLocalVariables);
        writeToFile(numberOfLocalVariablesString);
        writeToFile("\n");

        // if the subroutine is a constructor, then allocate memory for the object
        if (isConstructor==1){
            writeToFile("push constant ");
            int numberOfFieldsString = calculateNumberOfFieldVariablesInCurrentScope(CURRENT_NODE->parent->symbolTable);
            // convert numberOfFieldsString to string
            char numberOfFieldsStringString[10];
            sprintf(numberOfFieldsStringString, "%d", numberOfFieldsString);
            writeToFile(numberOfFieldsStringString);
            writeToFile("\n");
            writeToFile("call Memory.alloc 1\n");
            writeToFile("pop pointer 0\n");

            isConstructor = 0;
        }
        
    }


    // consume the ( token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '('){
        // printf("30. Error: ( expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = openParenExpected;
        return info;
    }
    // it is the ( token

    // call parameterList
    info = parameterList();
    if(info.er != none){
        return info;
    }
    // it is the parameterList

    // consume the ) token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != ')'){
        // printf("31. Error: ) expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = closeParenExpected;
        return info;
    }
    // it is the ) token

    // call subroutineBody
    info = subroutineBody();
    if(info.er != none){
        return info;
    }
    // it is the subroutineBody

    // no errors, return the token
    info.tk = token;
    info.er = none;
    return info;
}

ParserInfo memberDeclaration(){
    // rule: memberDeclaration -> classVariableDeclaration | subroutineDeclaration
    // print message for debugging

    ParserInfo info;
    Token token;

    // peek next token to see if its static or field
    token = PeekNextToken();
    if (token.tp == RESWORD && (strcmp(token.lx, "static") == 0 || strcmp(token.lx, "field") == 0)){
        info = classVariableDeclaration();
        if(info.er != none){
            return info;
        }
        // it is the classVariableDeclaration
        return info;
    }
    // else, it is a subroutineDeclaration
    else if (token.tp == RESWORD && (strcmp(token.lx, "constructor") == 0 || strcmp(token.lx, "function") == 0 || strcmp(token.lx, "method") == 0)){
        info = subroutineDeclaration();
        if(info.er != none){
            return info;
        }
        // it is the subroutineDeclaration
        return info;
    }
    // else, there is an error
    else{
        // printf("32. Error: static, field, constructor, function, or method expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = memberDeclarErr;
        return info;
    }
}

ParserInfo classDeclaration(){
    // rule: classDeclaration -> class identifier { {memberDeclar} }
    // print message for debugging

    ParserInfo info;
    Token token;

    // consume the class token
    token = GetNextToken();

    // print the details of the token

    if (token.tp != RESWORD || strcmp(token.lx, "class") != 0){
        // printf("33. Error: class expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = classExpected;
        return info;
    }
    // it is the class token

    // consume the identifier token
    token = GetNextToken();
    if (token.tp != ID){
        // printf("34. Error: identifier expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = idExpected;
        return info;
    }
    // it is the identifier token
    // print out the name of the current node and the type of it
    // keep track of the class name
    char* className = token.lx;
    Kind kind = class;
    char* aType = "class";
    // this is a new class, so create a symbol table for it
    if (numberOfParseIterations == 0){
        //printf("57. CURRENT_NODE is %s\n", CURRENT_NODE->symbolTable->className);
        CURRENT_NODE = initNode(TREE_ROOT, className, classNode);
        //printf("CURRENT_NODE is %s\n", CURRENT_NODE->symbolTable->className);
    }
    // else we should already have a symbol table ready so just set it to the current_node
    else {
        //printf("CURRENT_NODE is %s\n", CURRENT_NODE->symbolTable->className);
        CURRENT_NODE = findClassNodeReturnNode(className);
        //printf("CURRENT_NODE is %s\n", CURRENT_NODE->symbolTable->className);
    }



    // add the class to the root symbol table
    Symbol* aSymbol = constructSymbol(className, aType, kind, TREE_ROOT->symbolTable->nextAvailableLocation);
    int result = 1;
    
    if (numberOfParseIterations == 0){
        result = insertSymbol(TREE_ROOT->symbolTable, aSymbol);
    }
    // print current node name

    // consume the { token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '{'){
        // printf("35. Error: { expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = openBraceExpected;
        return info;
    }
    // it is the { token
    
    // peek next token to see if its static, field, constructor, function, or method
    // if it is, enter the loop to consume all memberDeclarations
    token = PeekNextToken();
    if (token.tp == RESWORD && (strcmp(token.lx, "static") == 0 || strcmp(token.lx, "field") == 0 || strcmp(token.lx, "constructor") == 0 || strcmp(token.lx, "function") == 0 || strcmp(token.lx, "method") == 0)){
        while (token.tp == RESWORD && (strcmp(token.lx, "static") == 0 || strcmp(token.lx, "field") == 0 || strcmp(token.lx, "constructor") == 0 || strcmp(token.lx, "function") == 0 || strcmp(token.lx, "method") == 0)){
            info = memberDeclaration();
            if(info.er != none){
                return info;
            }
            // it is the memberDeclaration

            // peek next token to see if its static, field, constructor, function, or method
            token = PeekNextToken();
        }
    }
    // else, there are no memberDeclarations left
    // consume the } token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '}'){
        // printf("36. Error: } expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = closeBraceExpected;
        return info;
    }
    // it is the } token
    // the class is done, so change the current node to the parent node
    CURRENT_NODE = TREE_ROOT;
    // no errors, return the token
    info.tk = token;
    info.er = none;
    return info;
}

ParserInfo Parse(){
    // print message for debugging

    ParserInfo info;
    Token token;

    // peek next token and check if it contains a lexical error
    token = PeekNextToken();
    if (token.tp == ERR){
        // printf("100. Error: %s, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = lexerErr;
        return info;
    }

    // call classDeclaration
    info = classDeclaration();
    if(info.er != none){
        return info;
    }
    // it is the classDeclaration

    // consume the EOF token
    token = GetNextToken();
    if (token.tp != EOFile){
        // printf("37. Error: EOF expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = syntaxError;
        return info;
    }
    // it is the EOF token

    // no errors, return the token
    info.tk = token;
    info.er = none;
    return info;
}


