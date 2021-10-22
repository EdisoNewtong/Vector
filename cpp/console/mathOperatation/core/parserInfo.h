#ifndef PARSER_FLAG_H
#define PARSER_FLAG_H

#include <cstdlib>

struct ParserInfo
{
	ParserInfo();

	int  nLine;
	int  nCol;

	size_t nCharIdx;

	bool hasPreviousChar;
	bool isLastChar;
	char previousChar;
};

#endif
