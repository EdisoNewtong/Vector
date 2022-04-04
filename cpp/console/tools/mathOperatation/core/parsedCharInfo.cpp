#include "parsedCharInfo.h"

using namespace std;


string PosInfo::getPosStr(int withCharIdx /* = 1 */) const
{
	string s("@");
	s += to_string(nLine);
	s += ":";
	s += to_string(nCol);

	if ( withCharIdx == 1 ) {
		s += ", charIdx = ";
		s += to_string( static_cast<int>(nCharIdx) );
	}

	s += " ";
	return s;
}

ParsedCharInfo::ParsedCharInfo()
	: position()
	, hasPreviousChar(false)
    , isLastChar(false)
	, previousChar(0)
	, currentChar(0)
	, nextChar(0)
	, baseInfo(nullptr)
{ 
	position.nLine = 1;
	position.nCol = 0; // set as 0 not 1 , when read the 1st char , nCol <-- 1
	position.nCharIdx = 0;
}

