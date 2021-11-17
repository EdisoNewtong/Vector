#include <iostream>
#include <cassert>

#include "tokenParserBase.h"
#include "parserException.h"

using namespace std;

TokenParserBase::TokenParserBase(E_PaserType tp)
	: m_alreadyTravelsaledString("")
	, m_token("")
	, m_type(tp)
	, m_isValidEnd(0)
	, m_switchFlag( E_TERMINAL_NONE )
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
	// copy
	auto pMap = CharUtil::getAllCharSet();
	m_AllAvalibleCharacters = *pMap;
}


CharBaseInfo* TokenParserBase::commonCheck(char ch, ParsedCharInfo* pInfo)
{
	auto pCharInfo = getInsideCharSetBaseInfo(ch);

	if ( pCharInfo == nullptr ) {
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
		info += std::to_string(pInfo->position.nCharIdx);
		info += " @Line ";
		info += std::to_string(pInfo->position.nLine);
		info += ":";
		info += std::to_string(pInfo->position.nCol);

		e.setDetail(info);

		throw e;
	}

	return pCharInfo;
}

// virtual 
E_PaserType  TokenParserBase::appendContent(ParsedCharInfo* pInfo) // override;
{
	auto ch = pInfo->currentChar;

	// must be empty
	if ( !m_alreadyTravelsaledString.empty() ) {
		cout << "[ERROR] In Default Parser, m_alreadyTravelsaledString != \"\" = \"" << m_alreadyTravelsaledString << "\"" << endl;
		assert(false);
	}

	
	if ( pInfo->baseInfo != nullptr ) {
		m_alreadyTravelsaledString += ch;
		if ( pInfo->baseInfo->isOpType() ) {
			// m_switchFlag = TokenParserBase::E_TRANSFER_NON_DEFAULT_PARSER_WAIT_NEXT;
			return E_P_OPERATOR;
		} else if ( pInfo->baseInfo->isBlank() ) {
			// m_switchFlag = TokenParserBase::E_TRANSFER_NON_DEFAULT_PARSER_WAIT_NEXT;
			return E_P_BLANK;
		} else if ( pInfo->baseInfo->isVaribleHeadChar() ) {
			// m_switchFlag = TokenParserBase::E_TRANSFER_NON_DEFAULT_PARSER_WAIT_NEXT;
			return E_P_VARIBLE;
		} else if ( pInfo->baseInfo->isNumber() ) {
			// guess it as an decimal number
			// m_switchFlag = TokenParserBase::E_TRANSFER_NON_DEFAULT_PARSER_WAIT_NEXT;
			return E_P_DECIMAL; // guess it as an decimal number
		} else {
			if ( ch == '.' ) {
				return E_P_FLOAT;
			} else {
				//    ;
				return E_P_ENDLESEE_SEMICOLON;
			}
		}
	} else {
		commonCheck(ch, pInfo);
	}


	return m_type;	
}

// virtual
TokenInfo* TokenParserBase::generateToken()
{
	return new TokenInfo(E_TOKEN_UNKNOWN, E_TOKEN_IGNORE);
}


// virtual 
void TokenParserBase::reset()
{
	m_switchFlag = E_TERMINAL_NONE;
}



CharBaseInfo* TokenParserBase::getInsideCharSetBaseInfo(char ch)
{
	auto it = m_AllAvalibleCharacters.find(ch);
	if ( it != m_AllAvalibleCharacters.end() ) {
		return it->second;
	} else {
		return nullptr;
	}
}



void TokenParserBase::transferToken(TokenParserBase* pNewParser)
{
	pNewParser->m_alreadyTravelsaledString = this->m_alreadyTravelsaledString;
	this->m_alreadyTravelsaledString.clear();

	// Set Begin Flag
	pNewParser->m_beginInfo = this->m_beginInfo;
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



TokenParserBase::E_TERMINAL_STATUS TokenParserBase::getSwitchFlag()
{
	return m_switchFlag;
}


// virtual 
bool TokenParserBase::isEnd()
{
	return false;
}

