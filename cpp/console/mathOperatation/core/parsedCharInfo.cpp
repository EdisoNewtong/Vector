#include "parsedCharInfo.h"

using namespace std;


std::string PosInfo::getLineInfo()
{
	string s("@");
	s += to_string(nLine);
	s += ":";
	s += to_string(nCol);
	s += " ";
	s += to_string(nCharIdx);
	return s;
}

ParsedCharInfo::ParsedCharInfo()
	: position()
	, hasPreviousChar(false)
    , isLastChar(false)
	, previousChar(0)
	, currentChar(0)
	, baseInfo(nullptr)
{ 
	position.nLine = 1;
	position.nCol = 0;
	position.nCharIdx = 0;
}

