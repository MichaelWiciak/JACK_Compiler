#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"

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

    // we need to consume the identifier token

    token = GetNextToken();
    if(token.tp != ID){
        // printf("Error in ParameterList: identifier expected, but %s found\n", token.lx);
        info.tk = token;
        info.er = idExpected;
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
        // consume the identifier token
        token = GetNextToken();
        if(token.tp != ID){
            // printf("2. Error in ParameterList: identifier expected, but %s found\n", token.lx);
            info.tk = token;
            info.er = idExpected;
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

    info = type();
    if(info.er != none){
        return info;
    }
    // it is the type

    token = GetNextToken();
    if(token.tp != ID){
        // printf("2. Error in variableDeclarationStatement: identifier expected, but %s found\n", token.lx);
        info.tk = token;
        info.er = idExpected;
        return info;
    }
    // it is the identifier

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

    info = type();
    if(info.er != none){
        return info;
    }
    // it is the type

    token = GetNextToken();
    if(token.tp != ID){
        // printf("2. Error in classVariableDeclaration: identifier expected, but %s found\n", token.lx);
        info.tk = token;
        info.er = idExpected;
        return info;
    }
    // it is the identifier

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

    // peek to see if its not an expression as it can be empty
    token = PeekNextToken();
    if (token.tp == SYMBOL && token.lx[0] == ')'){
        // return the info
        info.tk = token;
        info.er = none;
        return info;
    }

    info = expression();
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

    token = GetNextToken();
    // consume the identifier
    if (token.tp != ID){
        // printf("1. Error: identifier expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = idExpected;
        return info;
    }
    // peek to see if the next token is . or (
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
    }

    // consume the ( token
    token = GetNextToken();
    if (token.tp != SYMBOL || token.lx[0] != '('){
        // printf("3. Error: ( expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = openParenExpected;
        return info;
    }

    info = expressionList();
    if(info.er != none){
        return info;
    }
    // it is the expressionList


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

    info.tk = token;
    info.er = none;
    return info;
}

ParserInfo operand(){
    // rule: operand -> int | identifier [.identifier ] [ [ expression ] | (expressionList) ] | (expression) | string | true | false | null | this
    // print message for debugging

    ParserInfo info;
    Token token;

    token = PeekNextToken();
    // first check if it is an int | string | true | false | null | this
    if (token.tp == INT || token.tp == STRING || token.tp == RESWORD){
        // check if it is a RESWORD, then it must be true | false | null | this
        if (token.tp == RESWORD){
            if (token.tp == RESWORD && (strcmp(token.lx, "true") == 0 || strcmp(token.lx, "false") == 0 || strcmp(token.lx, "null") == 0 || strcmp(token.lx, "this") == 0)){
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
            // consume the token
            token = GetNextToken();
            info.tk = token;
            info.er = none;
            return info;
        }
    }

    // check if it is a identifier type
    if (token.tp == ID){
        // consume the identifier
        token = GetNextToken();
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
            info = expressionList();
            if(info.er != none){
                return info;
            }
            // it is the expressionList

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
    // it is the identifier token

    // peek to see if the next token is [
    token = PeekNextToken();

    // if it is, then we need to consume the [ token
    if (token.tp == SYMBOL && token.lx[0] == '['){
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

    // consume the constructor, function, or method token
    token = GetNextToken();
    if (token.tp != RESWORD || (strcmp(token.lx, "constructor") != 0 && strcmp(token.lx, "function") != 0 && strcmp(token.lx, "method") != 0)){
        // printf("28. Error: constructor, function, or method expected but %s found, line %d\n", token.lx, token.ln);
        info.tk = token;
        info.er = subroutineDeclarErr;
        return info;
    }
    // it is the constructor, function, or method token

    // peek next token to see if its void
    token = PeekNextToken();
    if (token.tp == RESWORD && strcmp(token.lx, "void") == 0){
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