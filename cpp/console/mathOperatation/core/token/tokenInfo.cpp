#include "tokenInfo.h"


TokenInfo::TokenInfo(E_TokenType tp, E_TokenType subtype)
	: m_type(tp)
	, m_subType(subtype)
{

}



// virtual 
TokenInfo::~TokenInfo()
{
}

E_TokenType TokenInfo::getType()
{
	return m_type;
}


E_TokenType TokenInfo::getSubType()
{
	return m_subType;
}


