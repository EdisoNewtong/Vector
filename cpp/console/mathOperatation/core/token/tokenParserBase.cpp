#include "tokenParserBase.h"
#include "parserException.h"

using namespace std;

TokenParserBase::TokenParserBase(E_PaserType tp)
	: m_alreadyTravelsaledString("")
	, m_token("")
	, m_type(tp)
	, m_isValidEnd( false )
{
	m_AllAvalibleCharacters.clear();

}


// virtual 
TokenParserBase::~TokenParserBase()
{
}


// virtual 
void TokenParserBase::init()
{

}


CharBaseInfo* TokenParserBase::commonCheck(char ch, ParsedCharInfo* pInfo)
{
	auto pCharInfo = isInsideCharSet(ch);
	if ( pCharInfo == nullptr  ) {
		ParserExpection	e(E_ExceptionCode::E_UNKNOWN_CHAR);

		string info;
		if ( ch >= 0 ) {
			// < 32
			if ( ch < 32 ) {
				if ( ch == '\t' ) {
					info += "'\\t'";
				} else if ( ch == '\r' ) {
					info += "'\\r'";
				} else if ( ch == '\n' ) {
					info += "'\\n'";
				} else {
					info += " ? , code = ";  
					info += std::to_string( static_cast<int>(ch & 0xFFU) );
				}
			} else {
				if ( ch == 32 ) {
					info += "' '";
				} else {
					info += "'";
					info += ch;
					info += "'";
				}
			}
		} else {
			// < 0
			info += " ? , code = ";  
			info += std::to_string( static_cast<int>(ch & 0xFFU) );
		}

		info += "  | index = ";
		info += std::to_string(pInfo->pos.nCharIdx);
		info += " @Line ";
		info += std::to_string(pInfo->pos.nLine);
		info += ":";
		info += std::to_string(pInfo->pos.nCol);

		e.setDetail(info);

		throw e;
	}

	return pCharInfo;
}

// virtual 
E_PaserType  TokenParserBase::appendContent(ParsedCharInfo* pInfo, list<TokenInfo*>* pTokenList) // override;
{
	auto isTokenListEmpty = pTokenList->empty();
	if ( isTokenListEmpty ) {
		if ( pInfo->baseInfo != nullptr ) {
			char ch = pInfo->baseInfo->getCh();

			m_alreadyTravelsaledString += ch;

			if ( pInfo->baseInfo->isOpType() ) {
				if ( ch == '/' ) {
					return E_P_OPERATOR;
				}
			} else if ( pInfo->baseInfo->isBlank() ) {
				return E_P_BLANK;
			} else if ( pInfo->baseInfo->isVaribleHeadChar() ) {
				return E_P_VARIBLE;
			} else if ( pInfo->baseInfo->isNumber() ) {
				// guess it as an decimal number
				return E_P_DECIMAL; // guess it as an decimal number
			}
		} else {

		}
	}

	return m_type;	
}

// virtual
TokenInfo* TokenParserBase::generateToken()
{
	return new TokenInfo(E_TOKEN_UNKNOWN, E_TOKEN_IGNORE);
}



CharBaseInfo* TokenParserBase::isInsideCharSet(char ch)
{
	auto it = m_AllAvalibleCharacters.find(ch);
	if ( it != m_AllAvalibleCharacters.end() ) {
		return it->second;
	} else {
		return nullptr;
	}
}



void TokenParserBase::transferToken(TokenParserBase* pBase)
{
	this->m_alreadyTravelsaledString = pBase->m_alreadyTravelsaledString;
	pBase->m_alreadyTravelsaledString.clear();

	// Set Begin Flag
	this->m_beginInfo = pBase->m_beginInfo;
}


E_PaserType TokenParserBase::getType()
{
	return m_type;
}


std::string TokenParserBase::getToken()
{
	return m_token;
}


void TokenParserBase::markBeginTag(ParsedCharInfo* pInfo)
{
	m_beginInfo = *pInfo;
}
