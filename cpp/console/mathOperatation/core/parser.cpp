#include <iostream>

#include "parser.h"

#include "tokenParserMgr.h"


using namespace std;

Parser::Parser()
	: m_buf(nullptr)
	, m_size(0)
	, m_debugOption(0)
	, m_flag(0)
    // Parser
	, m_defaultParser( nullptr )
	, m_currentParser( nullptr )
	, m_currentPaserType( E_P_DEFAULT )
	, m_pInfo()
	, m_optKeyWordprefix_1()
	, m_optKeyWord_1()
	, m_optKeyWordprefix_2()
	, m_optKeyWord_2()
{


	prepareOptionKeyWord();
}

// virtual 
Parser::~Parser()
{
}


void Parser::prepareOptionKeyWord()
{
	  m_optKeyWordprefix_1 = "--option=";
	  m_optKeyWord_1 = "--option=<number>";
	  m_optKeyWordprefix_2 = "--flags=";
	  m_optKeyWord_2 = "--flags=<number>";
}



bool Parser::analyzeOptionArgs(const std::list<string>& argList, string& errorMsg)
{

    m_defaultParser = TokenParserMgr::getParserByType(E_P_DEFAULT); // set default parser
	m_currentParser = m_defaultParser;                              // set current parser

	auto bret = true;

	for ( const auto& sOption : argList ) 
	{
		auto pos1 = sOption.find(m_optKeyWordprefix_1);
		auto pos2 = sOption.find(m_optKeyWordprefix_2);

		if ( pos1 != string::npos ) {
			// set debug option
			string optValue = sOption.substr( pos1 + m_optKeyWordprefix_1.size() );
			m_debugOption = atoi( optValue.c_str() );
		} else if ( pos2 != string::npos ) {
			string flagValue = sOption.substr( pos2 + m_optKeyWordprefix_2.size() );
			m_flag = atoi( flagValue.c_str() );
		} else {
			bret = false;
			errorMsg = sOption;
			break;
		}
	}

	return bret;
}



void Parser::setContent(const char* buf, size_t sz)
{
	m_buf = buf;
	m_size = sz;
}



void Parser::processLineInfo(char ch, size_t idx)
{
    //
	// set base info , ch , index and flag
    //
	m_pInfo.position.nCharIdx = idx;
    m_pInfo.isLastChar = (idx == m_size-1);

	// set next char in advance
	if ( !m_pInfo.isLastChar ) {
		m_pInfo.nextChar = m_buf[idx+1];
	}

	m_pInfo.currentChar = ch;

	//  Set Ch BaseInfo
	m_pInfo.baseInfo = CharUtil::getCharBaseInfo(ch);

	if ( m_pInfo.hasPreviousChar ) {
		if ( m_pInfo.previousChar == '\r' ) {
			if ( ch == '\n' ) {
				//  \r \n
				++m_pInfo.position.nCol;
			} else {
				/*
				  string s = "Hello World";  \r
				  ++a;
				*/
				++m_pInfo.position.nLine;
				m_pInfo.position.nCol = 1;
			}
		} else if ( m_pInfo.previousChar == '\n' ) {
			++m_pInfo.position.nLine;
			m_pInfo.position.nCol = 1;
		} else {
			++m_pInfo.position.nCol;
		}
	} else {
		++m_pInfo.position.nCol;
	}

}



int Parser::doParse()
{
	size_t idx;

	auto isLastCharChanged = false;
	TokenParserBase::E_TERMINAL_STATUS lastChangeStatus = TokenParserBase::E_TERMINAL_NONE;

	for ( idx = 0; idx < m_size; ++idx )
	{
		char ch = m_buf[idx];
		// common process
		processLineInfo(ch,idx);

		auto nextParserType = m_currentParser->appendContent(&m_pInfo);
		if ( nextParserType != m_currentPaserType ) {
			if ( m_debugOption == 1 ) {
				cout << "[Changed] : " << getstrParserType(m_currentPaserType) << " -> " << getstrParserType(nextParserType) << " , " <<  m_pInfo.position.getLineInfo() << endl;
			}

			auto switchFlag = m_currentParser->getSwitchFlag();
			switch( switchFlag )
			{
			case TokenParserBase::E_TOKEN_TERMINATE_TO_DEFAULT:
				{
					// Other -> Default
					checkEndLogic();

					m_TokenListAnalyzer.pushToken( m_currentParser->generateToken() );
				}
				break;
			case TokenParserBase::E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE:
				{
					// push previous terminated Token
					checkEndLogic();

					m_TokenListAnalyzer.pushToken( m_currentParser->generateToken() );
					m_currentParser->reset();

					auto next_MiddleParser = TokenParserMgr::getParserByType(nextParserType);
					auto nextParserType2 = next_MiddleParser->appendContent(&m_pInfo);
					auto newParser = TokenParserMgr::getParserByType(nextParserType2);
					next_MiddleParser->transferToken( newParser, &m_pInfo );

					auto bEndFlag = newParser->isEnd(&m_pInfo);
					if ( bEndFlag ) {
						// Core return to default Parser
						m_TokenListAnalyzer.pushToken( newParser->generateToken() );
						if ( m_debugOption == 1 ) {
							cout << "  [Inner Changed] : " << getstrParserType(nextParserType2) << " -> " << getstrParserType(E_P_DEFAULT) << " , " <<  m_pInfo.position.getLineInfo() << endl;
						}
						nextParserType = E_P_DEFAULT;
					} else {
						nextParserType = nextParserType2;
					}
				}
				break;
			case TokenParserBase::E_TOKEN_CONVERT_TO_OTHER:
				{
					auto newParser = TokenParserMgr::getParserByType( nextParserType );
					m_currentParser->transferToken( newParser, &m_pInfo );
					// m_currentParser->reset();

					auto bEndFlag = newParser->isEnd(&m_pInfo);
					if ( bEndFlag ) {
						// Core return to default Parser
						m_TokenListAnalyzer.pushToken( newParser->generateToken() );
						if ( m_debugOption == 1 ) {
							cout << "  [Inner Changed] : " << getstrParserType(nextParserType) << " -> " << getstrParserType(E_P_DEFAULT) << " , " <<  m_pInfo.position.getLineInfo() << endl;
						}
						nextParserType = E_P_DEFAULT;
					} 
				}
				break;
			default:
				break;
			}

			if ( m_pInfo.isLastChar ) {
				isLastCharChanged = true;
				lastChangeStatus = switchFlag;
			}

			// Final Switch to the proper Parser
			m_currentParser = TokenParserMgr::getParserByType( nextParserType );
			m_currentPaserType = nextParserType;
		}

		//
		// treat this char as previous char
		//
		m_pInfo.previousChar = ch;
		m_pInfo.hasPreviousChar = true; 
	}

	//
	// Endless Logic Check
	//
	if ( isLastCharChanged ) {
		switch ( lastChangeStatus )
		{
		case TokenParserBase::E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE:
		case TokenParserBase::E_TOKEN_CONVERT_TO_OTHER:
			{
				checkEndLogic();
			}
			break;
		default:
			break;
		}
	} else {
		checkEndLogic();
	}

	return 1;
}


bool Parser::checkPreviousTokenIsValid()
{
	return true;
}


// static 
string Parser::getstrParserType(E_PaserType tp)
{
	string retStr;

	switch( tp ) 
	{
	case E_P_DEFAULT:
		retStr = "E_P_DEFAULT";
		break;
	case E_P_DECIMAL:
		retStr = "E_P_DECIMAL";
		break;
	case E_P_OCTAL:
		retStr = "E_P_OCTAL";
		break;
	case E_P_HEX:
		retStr = "E_P_HEX";
		break;
	case E_P_FLOAT:
		retStr = "E_P_FLOAT";
		break;
	case E_P_VARIBLE:
		retStr = "E_P_VARIBLE";
		break;
	case E_P_OPERATOR:
		retStr = "E_P_OPERATOR";
		break;
	case E_P_SINGLE_LINE_COMMENT:
		retStr = "E_P_SINGLE_LINE_COMMENT";
		break;
	case E_P_MULTI_LINE_COMMENT:
		retStr = "E_P_MULTI_LINE_COMMENT";
		break;
	case E_P_BLANK:
		retStr = "E_P_BLANK";
		break;
	default:
		retStr = "E_P_UNKNOWN ???";
		break;
	}

	return retStr;
}


string Parser::getUserManual()
{
	string strUserManul;
	strUserManul += "==================================================\n";
	strUserManul += "Usage : \n";
	strUserManul += "\t<programName> ";
	strUserManul += m_optKeyWord_1;
	strUserManul += "  ";
	strUserManul += m_optKeyWord_2;

	strUserManul += "   ";
	strUserManul += "<src-FileName>\n";
	strUserManul += "==================================================\n";

	return strUserManul;
}


void Parser::checkEndLogic()
{
	if ( m_currentParser != nullptr ) {
		auto tp = m_currentParser->getType();
		if ( tp != E_P_DEFAULT ) {
			if ( !(m_currentParser->isTokenValid()) ) {
				string errorMsg = getstrParserType(tp);
				errorMsg += "\"";
				errorMsg += m_currentParser->getVisitedStr();
				errorMsg += "\" is invalid";
				m_currentParser->throwErrMsg(&m_pInfo, errorMsg);
			}
		}
	}
}
