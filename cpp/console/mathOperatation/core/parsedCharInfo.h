#ifndef PARSED_CHAR_INFO_H
#define PARSED_CHAR_INFO_H

#include <cstdlib>
#include "charUtil.h"

struct ParsedCharInfo
{
	ParsedCharInfo();

	int  nLine;
	int  nCol;

	size_t nCharIdx;

	bool hasPreviousChar;
	bool isLastChar;
	char previousChar;

	CharBaseInfo* baseInfo;
};

#endif
