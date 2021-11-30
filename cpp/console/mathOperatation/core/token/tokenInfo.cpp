#include "tokenInfo.h"


TokenInfo::TokenInfo(E_TokenType tp, E_TokenType subtype)
	: m_type(tp)
	, m_subType(subtype)
	, m_strSequence("")
	, m_beginPos()
	, m_endPos()
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


void TokenInfo::resetSubType(E_TokenType tp)
{
	m_subType = tp;
}



std::string TokenInfo::getDetail()
{
	return m_strSequence;
}

void TokenInfo::setDetail(const std::string& token)
{
	m_strSequence = token;
}


void TokenInfo::setPosInfo(const PosInfo& begPos, const PosInfo& endPos)
{
	m_beginPos = begPos;
	m_endPos   = endPos;
}


const PosInfo& TokenInfo::getBeginPos() const
{
	return m_beginPos;
}

const PosInfo& TokenInfo::getEndPos() const
{
	return m_endPos;
}

