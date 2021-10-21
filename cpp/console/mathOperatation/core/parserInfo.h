#ifndef PARSER_FLAG_H
#define PARSER_FLAG_H

#include <cstdlib>

struct ParserInfo
{
	ParserInfo();

	bool hasPreviousChar;
	char previousChar;
	int  nLine;
	int  nCol;

	size_t nCharIdx;
};

#endif
