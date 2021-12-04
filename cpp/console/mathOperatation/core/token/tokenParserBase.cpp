#include <iostream>
#include <cassert>

#include "tokenParserBase.h"
#include "parserException.h"

using namespace std;

TokenParserBase::TokenParserBase(E_PaserType tp)
	: m_alreadyTravelsaledString("")
	, m_token("")
	, m_parserName("")
	, m_type(tp)
	, m_tokenType( E_TOKEN_UNKNOWN )
	, m_tokenSubType( E_TOKEN_UNKNOWN )
	, m_exceptionCode(E_UNKNOWN_CHAR)
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
	m_tokenSubType = E_TOKEN_UNKNOWN;
	m_exceptionCode = E_UNKNOWN_CHAR;
	m_parserName = "TokenParserBase";
}


CharBaseInfo* TokenParserBase::commonCheckWithException(char ch, ParsedCharInfo* pInfo)
{
	auto pCharInfo = getInsideCharSetBaseInfo(ch);

	if ( pCharInfo == nullptr ) {
		throwErrMsg(pInfo," unknown character");
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
		m_beginInfo = pInfo->position;
		m_alreadyTravelsaledString += ch;
		m_endInfo = pInfo->position;

		E_PaserType nextParserType =  m_type;
		if ( pInfo->baseInfo->isBlank() ) {
			// <Space> <Tab> <\r> <\n>
			nextParserType =  E_P_BLANK;
		} else if ( pInfo->baseInfo->isVaribleHeadChar() ) {
			// a-z A-Z _
			nextParserType = E_P_VARIBLE;
		} else if ( pInfo->baseInfo->isNumber() ) {
			// 0-9
			nextParserType =  E_P_DECIMAL; // guess it as an decimal number
		} else if ( pInfo->baseInfo->isOpType()  ) {
			//  +  -  *  /  %    ...
			nextParserType = E_P_OPERATOR;
		} else {
			if ( ch == '.' ) {
				nextParserType = E_P_FLOAT;
			} else if ( ch == ';' ) {
				//    ;
				nextParserType = E_P_ENDLESS_SEMICOLON;
			}
		}

		m_switchFlag = E_TOKEN_CONVERT_TO_OTHER;
		return nextParserType;
	} else {
		commonCheckWithException(ch, pInfo);
	}

	return m_type;
}

// virtual
TokenInfo* TokenParserBase::generateToken()
{
	auto pRetToken = new TokenInfo( m_tokenType , m_tokenSubType);
	m_token = m_alreadyTravelsaledString;
	pRetToken->setDetail( m_token );
	pRetToken->setPosInfo( m_beginInfo, m_endInfo);
	return pRetToken;
}


// virtual 
void TokenParserBase::reset()
{
	m_switchFlag = E_TERMINAL_NONE;
	m_alreadyTravelsaledString.clear();
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



void TokenParserBase::transferToken(TokenParserBase* pNewParser, ParsedCharInfo* pInfo)
{
	pNewParser->m_alreadyTravelsaledString = this->m_alreadyTravelsaledString;
	this->reset();

	// Set Begin Flag and End Flag
	pNewParser->m_beginInfo = this->m_beginInfo;
	pNewParser->m_endInfo   = pInfo->position;
}


E_PaserType TokenParserBase::getType()
{
	return m_type;
}


string TokenParserBase::getVisitedStr()
{
	return m_alreadyTravelsaledString;
}

string TokenParserBase::getToken()
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

string TokenParserBase::genPositionCharStr(ParsedCharInfo* pInfo)
{
	string retstr;

	if ( pInfo != nullptr ) {
		retstr += '\t';
		retstr += "@";
		retstr += m_parserName;
		retstr += " , ";
		retstr += "@";
		retstr += to_string(pInfo->position.nLine);
		retstr += ":";
		retstr += to_string(pInfo->position.nCol);
		retstr += ", charIdx = ";
		retstr += to_string( static_cast<int>(pInfo->position.nCharIdx) );
		retstr += ", retCh = ";

		//
		// Output Char Info
		//
		char ch = pInfo->currentChar;
		if ( ch >= 0 ) {
			// < 32
			if ( ch < 32 ) {
				if ( ch == '\t' ) {
					retstr += "'\\t'";
				} else if ( ch == '\r' ) {
					retstr += "'\\r'";
				} else if ( ch == '\n' ) {
					retstr += "'\\n'";
				} else {
					retstr += " ? , code = ";
					retstr += std::to_string( static_cast<int>(ch & 0xFFU) );
				}
			} else {
				// char >= 32
				if ( ch == 32 ) {
					retstr += "<space>";
				} else if ( ch <= 127 ) {
					retstr += "'";
					retstr += ch;
					retstr += "'";
				} else {
					retstr += " ? , code = ";  
					retstr += std::to_string( static_cast<int>(ch & 0xFFU) );
				}
			}
		} else {
			// < 0
			retstr += " ? , code = ";  
			retstr += std::to_string( static_cast<int>(ch & 0xFFU) );
		}
		// retstr += "\n";

	}

	return retstr;
}


void TokenParserBase::throwErrMsg(ParsedCharInfo* pInfo, const string& errMsg)
{
	ParserException err(m_exceptionCode);
	auto detailstr = genPositionCharStr(pInfo);
	detailstr += " ==>      \"";
	detailstr += m_alreadyTravelsaledString;
	detailstr += "\" , ";
	detailstr += errMsg;
	err.setDetail( detailstr );
	throw err;
}


// virtual 
bool TokenParserBase::isTokenValid()
{
	return true;
}


