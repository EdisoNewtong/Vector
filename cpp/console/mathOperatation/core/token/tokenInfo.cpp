#include "tokenInfo.h"


TokenInfo::TokenInfo(E_TokenType tp, E_TokenType subtype)
	: m_type(tp)
	, m_subType(subtype)
	, m_strSequence("")
    , m_uUCnt(0)
    , m_lLCnt(0)
    , m_fCnt(0)
	, m_beginPos()
	, m_endPos()
    , m_dataType(E_TP_S_INT)
    , m_hasDoIntergerPromotion(false)
    , m_hasDoDataTypeConvertion(false)
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



void TokenInfo::setDataType(E_DataType tp)
{
    m_dataType = tp;
}


E_DataType TokenInfo::getDataType()
{
    return m_dataType;
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




