#ifndef PARSED_CHAR_INFO_H
#define PARSED_CHAR_INFO_H

#include <cstdlib>
#include <string>
#include "charUtil.h"

struct PosInfo
{
	int  nLine;
	int  nCol;

	size_t nCharIdx;
	std::string getLineInfo();
};

struct ParsedCharInfo
{
	ParsedCharInfo();

	PosInfo position;

	bool hasPreviousChar;
	bool isLastChar;
	char previousChar;
	char currentChar;

	CharBaseInfo* baseInfo;
};

#endif
