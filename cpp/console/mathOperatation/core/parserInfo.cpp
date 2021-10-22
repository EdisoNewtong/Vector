#include "parserInfo.h"

ParserInfo::ParserInfo()
	: nLine(1)
	, nCol(0)
	, nCharIdx(0)
	, hasPreviousChar(false)
    , isLastChar(false)
	, previousChar(0)
{ 
}

