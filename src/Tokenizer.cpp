#include "tokenizer.h"

void Tokenizer::setString( std::string str ) {
	buffer = str; // initialising buffer
	currentPosition = buffer.begin();
}

void Tokenizer::setDelim( std::string str ) {
	delim = str; // initialising delimiter variable
}

void Tokenizer::resetPosition(void) {
	currentPosition = buffer.begin();
}

void Tokenizer::setPosition( int pos ) {
	int thisPos = 0;
	while( thisPos <= pos && firstToken() != "" ) {
		thisPos++;
	}
}

bool Tokenizer::isDelim( char c ) {
	return delim.find(c) != std::string::npos;
}

void Tokenizer::skipDelim(void) {
	std::string::iterator iter = currentPosition;
	for( ; isDelim(*iter); ++iter )
		;
	currentPosition = iter;
}

std::string Tokenizer::currentToken(void) {
	return thisToken;
}

std::string Tokenizer::firstToken(void) {
	thisToken.erase();
	skipDelim();
	std::string::iterator iter = currentPosition;
	for( ; *iter && !isDelim(*iter); iter++ ) {
		thisToken += *iter;
	}
	(*iter == NULL) ?
		(currentPosition = iter) : (currentPosition = ++iter);
	return thisToken;
}

// token that comes after the first token
std::string Tokenizer::nextToken(void) {
	if(firstToken() != "") {
		return firstToken();
	}
	return thisToken;
}

std::string Tokenizer::lastToken(void) {
	std::string theLastToken;
	while(firstToken() != "") {
		theLastToken = thisToken;
	}
	return theLastToken;
}

int Tokenizer::countTokens(void) {
	// reset position before starting to count tokens
	resetPosition();
	int tokenNumber = 0;
	for(; firstToken() != ""; tokenNumber++ )
		;
	resetPosition(); 
	return tokenNumber;
}

int Tokenizer::getTokenNumber( std::string str ) {
	setString(str);
	return countTokens();
}

void Tokenizer::cleanString( std::string &str ) {
	setString(str);
	std::string _str;
	vstring temp;
	tokenize(temp);
	int vec_size = temp.size();
	for( int i = 0; i < vec_size; i++ ) {
		_str += temp[i] + " ";
	}
	_str.erase( _str.length() - 1, 1 );
	str = _str;
}

// tokenize's the string completely
// and puts the tokens into a vector
void Tokenizer::tokenize( vstring &v ) {
	if(buffer.length() == 0) {
		return;
	}
	resetPosition();
	for( ; firstToken() != ""; v.push_back(thisToken))
		;
	resetPosition();
}

void Tokenizer::tokenize(std::string str, vstring &v) {
	setString(str); tokenize(v);
}

void Tokenizer::cleanString(std::string &str, std::string delim) {
	setDelim(delim); cleanString(str);
}




	
