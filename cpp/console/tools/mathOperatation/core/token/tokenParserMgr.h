#ifndef TOKEN_PARSER_MGR_H
#define TOKEN_PARSER_MGR_H

#include <unordered_map>
#include "commonEnum.h"
#include "tokenParserBase.h"

class TokenParserMgr
{
public:
	static void init();
	static void finalize();
	static TokenParserBase* getParserByType(E_PaserType type);
protected:
#ifdef USE_INT_INSTEAD_OF_ENUM
	static std::unordered_map<int, TokenParserBase*> s_parserMap;
#else
	static std::unordered_map<E_PaserType, TokenParserBase*> s_parserMap;
#endif
};


#endif
