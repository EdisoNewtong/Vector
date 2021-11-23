// #include <iostream>
#include "octalParser.h"
#include "parserException.h"

using namespace std;

OctalParser::OctalParser(E_PaserType tp)
	: DecimalParser(tp)
{
}

//
// virtual 
//
OctalParser::~OctalParser()
{
}

// virtual 
void OctalParser::init() // override
{
	// m_AllAvalibleCharacters.insert( make_pair('+', CharUtil::getCharBaseInfo('+') ) );
	// m_AllAvalibleCharacters.insert( make_pair('-', CharUtil::getCharBaseInfo('-') ) );
	
	// for( char ch = '0'; ch <='7'; ++ch ) {
	// 	m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	// }

	/*
	     so   range is [0-9]
		 Not  range [0-7]
	*/
	// normal  :  01234567u   01234567l  01234567L
	// special :  091.1       08e    // is valid
	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}



	m_AllAvalibleCharacters.insert( make_pair('e', CharUtil::getCharBaseInfo('e') ) );
	m_AllAvalibleCharacters.insert( make_pair('E', CharUtil::getCharBaseInfo('E') ) );

	m_AllAvalibleCharacters.insert( make_pair('.', CharUtil::getCharBaseInfo('.') ) );

	m_AllAvalibleCharacters.insert( make_pair('u', CharUtil::getCharBaseInfo('u') ) );
	m_AllAvalibleCharacters.insert( make_pair('U', CharUtil::getCharBaseInfo('U') ) );
	m_AllAvalibleCharacters.insert( make_pair('l', CharUtil::getCharBaseInfo('l') ) );
	m_AllAvalibleCharacters.insert( make_pair('L', CharUtil::getCharBaseInfo('L') ) );

	m_tokenType = E_TOKEN_OCTAL_NUMBER;
    m_exceptionCode = E_OCT_INVALID_FORMAT;
	m_parserName = "OctalParser";

}

// virtual 
E_PaserType  OctalParser::appendContent(ParsedCharInfo* pInfo) // override
{
	char curCh = pInfo->currentChar;
	// int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	auto inSideCharInfo = getInsideCharSetBaseInfo( curCh );
	if ( inSideCharInfo == nullptr ) {
		//  0123+
		//      ^
		string sMsg;
		auto b = isOctBufValid(sMsg);
		if ( !b ) {
			throwErrMsg(pInfo,sMsg);
		} else {
			m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
			return E_P_DEFAULT;
		}
	} else {
		// inSideCharInfo != nullptr 
		if ( inSideCharInfo->isNumber() ) { /* inSideCharInfo->isOctNumber() */
			if ( isSuffixExisted() ) {
				// throw
				throwErrMsg(pInfo,  " Can't append Oct-Code after suffix u/U l/L");
			} else {
				m_alreadyTravelsaledString += curCh;
			}
		} else if ( is_dot(curCh) || is_eE(curCh) ) {
			if ( isSuffixExisted() ) {
				//  throw
				throwErrMsg(pInfo,  " Can't append Oct-Code after suffix u/U l/L");
			} else {

				m_alreadyTravelsaledString += curCh;
				m_switchFlag = E_TOKEN_CONVERT_TO_OTHER;
				return E_P_FLOAT;
			}
		} else {
			// u/U  l/L
			update_uU_lLCnt(curCh, pInfo);
			string sMsg;
			auto b = isOctBufValid(sMsg);
			// cout << "isOctBufValid(sMsg) , b = " << (b ? "true" : "false") << endl;
			if ( !b ) {
				throwErrMsg(pInfo,sMsg);
			}
		}
	}

	return m_type;	
}

// virtual
bool OctalParser::isEnd(ParsedCharInfo* pInfo) // override;
{
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	if ( sz <= 1 ) {
		return false;
	}

	//  sz >=2
	char prefix = m_alreadyTravelsaledString.at(0);
	if ( prefix != '0' ) {
		return false;
	}

	char ch2nd = m_alreadyTravelsaledString.at(1);
	auto inSideCharInfo = getInsideCharSetBaseInfo( ch2nd );
	if ( inSideCharInfo == nullptr || !(inSideCharInfo->isOctNumber()) ) {
		return false;
	}

	return true;
}


bool OctalParser::isOctBufValid(string& errMsg)
{
	// cout << "\"" << m_alreadyTravelsaledString << "\"" << endl;
	if ( m_alreadyTravelsaledString.empty() ) {
		errMsg = "Empty Octal Buffer";
		return false;
	}	

	char firstCh = m_alreadyTravelsaledString.at(0);
	if ( firstCh != '0' ) {
		errMsg = "First character is not '0'";
		return false;
	}


	// Number count
	int validNumberCnt = 0;
	bool isValid = true;
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	for ( int i = 1; i < sz; ++i ) {
		auto ch = m_alreadyTravelsaledString.at(i);
		if ( is_uU_lL(ch) ) {
			if ( validNumberCnt == 0 ) {
				isValid = false;
				errMsg = "No valid oct number before u/U l/L ";
			}	
			break;
		} else {
			if ( isInnerOctCode(ch) ) {
				++validNumberCnt;
			} else {
				errMsg += "'";
				errMsg += ch;
				errMsg += "'";
				errMsg += " is not a valid oct number";
				isValid = false;
				break;
			}
		}
	}


	return isValid;
}




bool OctalParser::isInnerOctCode(char ch)
{
	return ch>='0' && ch<='7';
}

