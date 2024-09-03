#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

FILE* inputFile;

char* nameOfFile;
int line;
int slashCommentAdded = 0;
int starCommentAdded = 0; 
Token globalErrorToken;

int InitLexer (char* inputDirectory){
	nameOfFile = inputDirectory;
	line = 1;
    inputFile = fopen(inputDirectory, "r");
	if (inputFile == 0)
	{
		printf("Unable to open source file\n");
		return 0;
	}
	return 1;
}

char whiteSpaceRemover(char c){
	while (isspace(c) != 0){
		if (c == '\n') {
            line++;
        }
		c = getc(inputFile);
    }
	return c;
}

Token EOFError(Token token){
	strcpy(token.fl, nameOfFile);
	token.tp = EOFile;
	token.lx[0] = '\0';
	token.ln=line;
	return token;
}

Token EOFInCommentError(Token token){
	token.tp = ERR;
	strcpy(token.fl, nameOfFile);
	strcpy(token.lx, "Error: unexpected eof in comment");
	token.ec = EofInCom;
	token.ln=line;
	return token;
}

Token RESWORDToken(Token token, char* temp){
	token.tp = RESWORD;
	strcpy(token.lx, temp);
	strcpy(token.fl, nameOfFile);
	token.ln=line;
	return token;
}

Token NewLineInStringError(Token token){
	token.tp = ERR;
	strcpy(token.fl, nameOfFile);
	strcpy(token.lx, "Error: new line in string constant");
	token.ec = NewLnInStr;
	token.ln=line;
	return token;
}

Token EOFInStringError(Token token){
	token.tp = ERR;
	strcpy(token.lx, "Error: unexpected eof in string constant");
	token.ec = EofInStr;
	strcpy(token.fl, nameOfFile);
	token.ln=line;
	return token;
}

Token StringToken(Token token, char* temp){
	token.tp = STRING;
	strcpy(token.lx, temp);
	strcpy(token.fl, nameOfFile);
	token.ln=line;
	return token;
}

Token IDToken(Token token, char* temp){
	token.tp = ID;
	strcpy(token.lx, temp);
	strcpy(token.fl, nameOfFile);
	token.ln=line;
	return token;
}

Token SymbolToken(Token token){
	token.tp = SYMBOL;
	strcpy(token.fl, nameOfFile);
	token.ln=line;
	return token;
}

Token illegalSymbolError(Token token){
	token.tp = ERR;
	strcpy(token.fl, nameOfFile);
	strcpy(token.lx, "Error: illegal symbol in source file");
	token.ln=line;
	return token;
}

char slashComment(char c){
	if (c=='/'){
		c = getc(inputFile);
		if (c=='/'){
			// this is a valid comment
			while (c != '\n'){
				c = getc(inputFile);
				if (c == EOF){
					return EOF;
				}
			}
			// this character has to be new line so get a new character and return it
			c = getc(inputFile);
			slashCommentAdded = 1;
			line++;
			return c;
			// the comment is over
		}
		else{
			ungetc(c, inputFile);
			return '/';
		}
	}
	else{
		return c;
	}
}

char starComment(char c){
	if (c=='/'){
		c = getc(inputFile);
		if (c=='*'){
			// this is a valid comment
			while (c != EOF){
				c = getc(inputFile);
				if (c == '*'){
					c = getc(inputFile);
					if (c == '/'){
						// the comment is over
						break;
					}
				}
				// if new line is found, increment line
				if (c == '\n'){
					line++;
				}
			}
			// the comment is over
			if (c == EOF){
				return EOF;
			}
			starCommentAdded = 1;
			c = getc(inputFile);
			return c;
		}
		else{
			ungetc(c, inputFile);
			return '/';
		}
	}
	else{
		return c;
	}
}

Token GetNextToken (){
    Token token;
	token.ln=0;
	int c = getc(inputFile);
	// Gets spaces
	c = whiteSpaceRemover(c);
	// add code to consume comments
	if (c == EOF)
	{
		return EOFError(token);
	}
	// 
	
	while (c == '/'){
		c = slashComment(c);
		if (c == EOF){
			return EOFInCommentError(token);
		}
		if (slashCommentAdded == 1){
			slashCommentAdded = 0;
			c = whiteSpaceRemover(c);
			continue;
		}
		c = starComment(c);
		if (c == EOF){
			return EOFInCommentError(token);
		}
		if (starCommentAdded == 1){
			starCommentAdded = 0;
			c = whiteSpaceRemover(c);
			continue;
		}
		break;
	}
	// whitespce remover again
	c = whiteSpaceRemover(c);
	// check if eof
	if (c == EOF)
	{
		return EOFError(token);
	}

	char temp[128];
	int i = 0;
	// add code to handle strings
	if (c == '"')
	{
		c = getc(inputFile);
		while (c != '"')
		{
			if (c == EOF)
			{
				return EOFInStringError(token);
			}
			if (c == '\n')
			{
				return NewLineInStringError(token);
			}
			
			temp[i++] = c;
			c = getc(inputFile);
		}
		temp[i] = c;
		temp[i++] = '\0';
		return StringToken(token, temp);
	}
	// add code to handle identifiers and reserved words
	if (isalpha(c)||(c=='_')){
		// this is the start of keyword or id
		while (isalpha(c)||(c=='_')||(isdigit(c)))
		{
			temp[i++] = c;
			c = getc(inputFile);
		}
		ungetc(c, inputFile);
		temp[i] = '\0';

		// check if this is a reserved word
		if (strcmp(temp, "class") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "constructor") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "function") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "method") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "field") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "static") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "var") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "int") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "char") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "boolean") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "void") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "true") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "false") == 0)
		{
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "null")==0){
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "this")==0){
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "let")==0){
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "do")==0){
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "if")==0){
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "else")==0){
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "while")==0){
			return RESWORDToken(token, temp);
		}
		else if (strcmp(temp, "return")==0){
			return RESWORDToken(token, temp);
		}
		// if not a reserved word, then it is an id
		else
		{
			return IDToken(token, temp);
		}
	}
	// add code to handle integers
	if (isdigit(c))
	{
		// this is the start of number
		i = 0;
		while (isdigit(c))
		{
			if ((c == '\n')||(c == EOF)){
				token.tp = INT;
				strcpy(token.lx, temp);
				strcpy(token.fl, nameOfFile);
				token.ln=line;
				return token;
			}
			temp[i++] = c;
			c = getc(inputFile);
		}
		ungetc(c, inputFile);

		temp[i] = '\0';
		token.tp = INT;
		strcpy(token.lx, temp);
		strcpy(token.fl, nameOfFile);
		token.ln=line;
		return token;
	}
	// else it must be a symbol
	if ((c=='(')||(c==')')||(c=='{')||(c=='}')||(c=='[')||(c==']')||(c=='.')||(c==',')||(c==';')||(c=='+')||(c=='-')||(c=='*')||(c=='/')||(c=='&')||(c=='|')||(c=='<')||(c=='>')||(c=='=')||(c=='~')){
		token.lx[0] = c;
		token.lx[1] = '\0';
		return SymbolToken(token);
	}
	else{
		return illegalSymbolError(token);
	}
}

Token PeekNextToken (){
	
	Token token;
	token = GetNextToken ();
	// return the token to the input stream
	if (token.tp==STRING){
		ungetc('"', inputFile);
	}
	for(int i = strlen(token.lx)-1; i >= 0; i--) {
		ungetc(token.lx[i], inputFile);
	}
	if (token.tp==STRING){
		ungetc('"', inputFile);
	}
	return token;
}

int StopLexer (){
	// close the file
	fclose(inputFile);
	return 0;
}
