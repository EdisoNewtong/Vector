#include "parsedCharInfo.h"

ParsedCharInfo::ParsedCharInfo()
	: pos()
	, hasPreviousChar(false)
    , isLastChar(false)
	, previousChar(0)
	, baseInfo(nullptr)
{ 
	pos.nLine = 1;
	pos.nCol = 0;
	pos.nCharIdx = 0;
}

