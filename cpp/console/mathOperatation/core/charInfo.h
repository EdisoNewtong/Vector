#ifndef CHAR_INFO_H
#define CHAR_INFO_H

#include "commonEnum.h"

struct CharInfo
{
	CharInfo(E_ChType tp, E_Char_Category charCate);

	E_ChType type;
	E_Char_Category cat;
};


#endif
