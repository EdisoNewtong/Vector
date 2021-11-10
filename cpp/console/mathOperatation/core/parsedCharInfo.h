#ifndef PARSED_CHAR_INFO_H
#define PARSED_CHAR_INFO_H

#include <cstdlib>
#include "charUtil.h"

struct PosInfo
{
	int  nLine;
	int  nCol;

	size_t nCharIdx;
};

struct ParsedCharInfo
{
	ParsedCharInfo();

	PosInfo pos;

	bool hasPreviousChar;
	bool isLastChar;
	char previousChar;

	CharBaseInfo* baseInfo;
};

#endif
