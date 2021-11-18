#include <iostream>
#include <cassert>

#include "tokenParserBase.h"
#include "parserException.h"

using namespace std;

TokenParserBase::TokenParserBase(E_PaserType tp)
	: m_alreadyTravelsaledString("")
	, m_token("")
	, m_type(tp)
	, m_tokenType( E_TOKEN_UNKNOWN )
	, m_isValidEnd(0)
	, m_switchFlag( E_TERMINAL_NONE )
	, m_beginInfo()
	, m_endInfo()
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

	m_tokenType = E_TOKEN_UNKNOWN;
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
		if ( pInfo->baseInfo->isBlank() ) {
			// <Space> <Tab> <\r> <\n>
			return E_P_BLANK;
		} else if ( pInfo->baseInfo->isVaribleHeadChar() ) {
			// a-z A-Z _
			return E_P_VARIBLE;
		} else if ( pInfo->baseInfo->isNumber() ) {
			// 0-9
			return E_P_DECIMAL; // guess it as an decimal number
		} else if ( pInfo->baseInfo->isOpType()  ) {
			//  +  -  *  /  %    ...
			return E_P_OPERATOR;
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
	auto pRetToken = new TokenInfo( m_tokenType , m_tokenType);
	m_token = m_alreadyTravelsaledString;
	pRetToken->setDetail( m_token );
	return pRetToken;
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


void TokenParserBase::markBeginTag(const PosInfo& posInfo)
{
	m_beginInfo = posInfo;
}



TokenParserBase::E_TERMINAL_STATUS TokenParserBase::getSwitchFlag()
{
	return m_switchFlag;
}


// virtual 
bool TokenParserBase::isEnd(ParsedCharInfo* pInfo)
{
	(void)pInfo;
	return false;
}

