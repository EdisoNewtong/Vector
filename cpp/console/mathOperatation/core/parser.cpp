#include <iostream>

#include "parser.h"
#include "enum2name.h"
#include "parserOptions.h"
#include "tokenParserMgr.h"


using namespace std;

Parser::Parser()
	: m_buf(nullptr)
	, m_size(0)
    // Token Parser Part
	, m_defaultParser( nullptr )
	, m_currentParser( nullptr )
	, m_currentPaserType( E_P_DEFAULT )
	, m_pInfo()
{

    m_defaultParser = TokenParserMgr::getParserByType(E_P_DEFAULT); // set default parser
	m_currentParser = m_defaultParser;                              // set current parser

}

// virtual 
Parser::~Parser()
{
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
	auto argsDbgOption = ParserOptions::getDebugOption();
	auto argsFlag      = ParserOptions::getFlag();        
	(void)argsFlag;

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
			if ( argsDbgOption >= 2 ) {
				cout << "[Changed] : " << Enum2Name::getParserName(m_currentPaserType) << " -> " << Enum2Name::getParserName(nextParserType) << " , " <<  m_pInfo.position.getPosStr() << endl;
			}

			auto pushRet = 0;
			auto switchFlag = m_currentParser->getSwitchFlag();
			switch( switchFlag )
			{
			case TokenParserBase::E_TOKEN_TERMINATE_TO_DEFAULT:
				{
					// Other -> Default
					pushRet = m_TokenListAnalyzer.pushToken( m_currentParser->generateToken() );
				}
				break;
			case TokenParserBase::E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE:
				{
					// push previous terminated Token
					checkEndLogic();

					pushRet = m_TokenListAnalyzer.pushToken( m_currentParser->generateToken() );
					m_currentParser->reset();

					auto next_MiddleParser = TokenParserMgr::getParserByType(nextParserType);
					auto nextParserType2 = next_MiddleParser->appendContent(&m_pInfo);
					auto newParser = TokenParserMgr::getParserByType(nextParserType2);
					next_MiddleParser->transferToken( newParser, &m_pInfo );

					auto bEndFlag = newParser->isEnd(&m_pInfo);
					if ( bEndFlag ) {
						// Core return to default Parser
						pushRet = m_TokenListAnalyzer.pushToken( newParser->generateToken() );
						if ( argsDbgOption >= 2 ) {
							cout << "  [Inner Changed] : " << Enum2Name::getParserName(nextParserType2) << " -> " << Enum2Name::getParserName(E_P_DEFAULT) << " , " <<  m_pInfo.position.getPosStr() << endl;
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
						pushRet = m_TokenListAnalyzer.pushToken( newParser->generateToken() );
						if (  argsDbgOption >= 2 ) {
							cout << "  [Inner Changed] : " <<  Enum2Name::getParserName(nextParserType) << " -> " << Enum2Name::getParserName(E_P_DEFAULT) << " , " <<  m_pInfo.position.getPosStr() << endl;
						}
						nextParserType = E_P_DEFAULT;
					} 
				}
				break;
			default:
				break;
			}

			if ( pushRet == 1 ) {
				m_TokenListAnalyzer.evaluateExp();
				m_TokenListAnalyzer.clearEvaluateList();
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
		if (   lastChangeStatus == TokenParserBase::E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE 
			|| lastChangeStatus == TokenParserBase::E_TOKEN_CONVERT_TO_OTHER ) 
		{
			checkEndLogic();
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




void Parser::checkEndLogic()
{
	if ( m_currentParser != nullptr ) {
		auto tp = m_currentParser->getType();
		if ( tp != E_P_DEFAULT ) {
			if ( !(m_currentParser->isTokenValid()) ) {
				string errorMsg = Enum2Name::getParserName(tp);
				errorMsg += ", \"";
				errorMsg += m_currentParser->getVisitedStr();
				errorMsg += "\" is invalid";
				m_currentParser->throwErrMsg(&m_pInfo, errorMsg);
			}
		}
	}
}
