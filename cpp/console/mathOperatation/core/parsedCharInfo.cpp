#include "parsedCharInfo.h"

ParsedCharInfo::ParsedCharInfo()
	: nLine(1)
	, nCol(0)
	, nCharIdx(0)
	, hasPreviousChar(false)
    , isLastChar(false)
	, previousChar(0)
{ 
}

