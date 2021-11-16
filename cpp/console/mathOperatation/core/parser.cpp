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
	, m_currentPaserType( E_P_DEFAULT )
	, m_pInfo()
	, m_optKeyWordprefix_1()
	, m_optKeyWord_1()
	, m_optKeyWordprefix_2()
	, m_optKeyWord_2()
{
	m_tokenList.clear();

    m_defaultParser = TokenParserMgr::getParserByType(E_P_DEFAULT); // set default parser
	m_currentParser = m_defaultParser;                              // set current parser

	prepareOptionKeyWord();
}

// virtual 
Parser::~Parser()
{
	// clear token list
	for ( auto& pToken : m_tokenList ) {
		delete pToken;
		pToken = nullptr;
	}
	m_tokenList.clear();
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


/*

a-z  A-Z  0-9  _

=

+   -      *   /  %

&   |   ^    ~

<<    >>    

( )

;

<Space>   <Tab>   \r \n

*/


void Parser::processLineInfo(char ch, size_t idx)
{
    //
	// set base info , ch , index and flag
    //
	m_pInfo.position.nCharIdx = idx;
    m_pInfo.isLastChar = (idx == m_size-1);
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
	for ( idx = 0; idx < m_size; ++idx )
	{
		char ch = m_buf[idx];
		// common process
		processLineInfo(ch,idx);

		if ( idx == 0 ) {
			// current must be default
			m_currentParser->markBeginTag(&m_pInfo);
		}

		/*
		auto nextParserType = m_currentParser->appendContent(&m_pInfo, &m_tokenList);
		if ( nextParserType != m_currentPaserType ) {
			if ( m_debugOption == 1 ) {
				cout << "[Changed] : " << getstrParserType(m_currentPaserType) << " -> " << getstrParserType(nextParserType) << " , " <<  m_pInfo.position.getLineInfo() << endl;
			}

			auto oldParserType = m_currentPaserType;
			if ( oldParserType == E_P_DEFAULT ) {
				// from   Current(E_P_DEFAULT)    -->     Other-Parser
			} else {
				// from   Other-Parser   -->     ???
			}


			m_currentPaserType = nextParserType;
		}
		*/


		// treat this char as previous char
		m_pInfo.previousChar = ch;
		m_pInfo.hasPreviousChar = true; 
	}


	//
	// TODO : Endless Logic Check
	//

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

