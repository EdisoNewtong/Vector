#ifndef ENUM_2_NAME_H
#define ENUM_2_NAME_H

#include "commonEnum.h"
#include <string>

namespace Enum2Name
{
	std::string getParserName(E_PaserType tp);
	std::string getTokenName(E_TokenType tp);
	std::string getDataType(E_DataType tp);

}

#endif


