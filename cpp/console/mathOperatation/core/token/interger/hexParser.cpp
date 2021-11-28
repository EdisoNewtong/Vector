#include <iostream>
#include "hexParser.h"
#include "parserException.h"

using namespace std;

HexParser::HexParser(E_PaserType tp)
	: DecimalParser(tp)
{
}

//
// virtual 
//
HexParser::~HexParser()
{
}


// virtual 
void HexParser::init() // override
{
	// m_AllAvalibleCharacters.insert( make_pair('+', CharUtil::getCharBaseInfo('+') ) );
	// m_AllAvalibleCharacters.insert( make_pair('-', CharUtil::getCharBaseInfo('-') ) );

	// 0x     or     0X
	// m_AllAvalibleCharacters.insert( make_pair('x', CharUtil::getCharBaseInfo('x') ) );
	// m_AllAvalibleCharacters.insert( make_pair('X', CharUtil::getCharBaseInfo('X')  ) );

	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	for( char ch = 'a'; ch <='f'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	for( char ch = 'A'; ch <='F'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	m_AllAvalibleCharacters.insert( make_pair('u', CharUtil::getCharBaseInfo('u') ) );
	m_AllAvalibleCharacters.insert( make_pair('U', CharUtil::getCharBaseInfo('U') ) );
	m_AllAvalibleCharacters.insert( make_pair('l', CharUtil::getCharBaseInfo('l') ) );
	m_AllAvalibleCharacters.insert( make_pair('L', CharUtil::getCharBaseInfo('L') ) );

	m_tokenType = E_TOKEN_INTEGER_NUMBER;
	m_tokenSubType = E_TOKEN_HEX_NUMBER;
    m_exceptionCode = E_HEX_INVALID_FORMAT;
	m_parserName = "HexParser";
}


// virtual 
E_PaserType HexParser::appendContent(ParsedCharInfo* pInfo) // override
{
	char curCh = pInfo->currentChar;
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	auto inSideCharInfo = getInsideCharSetBaseInfo( curCh );
	if ( sz == 2 ) {
		// start with 0x or 0X
		if ( inSideCharInfo == nullptr || !(inSideCharInfo->isHexNumber()) ) {
			/* 
			   e.g.
			        0x+
					  ^
			*/

			// 3rd char must be inside a-f A-F  0-9
			//  u/U  l/L is not allowed
			// update_uU_lLCnt(curCh, pInfo);

			throwErrMsg(pInfo,  "After prefix '0x/0x' , must append at least One Hex code");
		} else {
			
			m_alreadyTravelsaledString += curCh;
		}
	} else {
		// sz >= 3
		if ( inSideCharInfo == nullptr ) {
			/* 
			   e.g.
			        0x333+
					     ^
			*/
			m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
			return E_P_DEFAULT;
		} else {
			if ( isSuffixExisted() ) {
				if ( inSideCharInfo->isHexNumber()  ) {
					//  throw  HexCode is end ,can't append  a-f A-F 0-9
					throwErrMsg(pInfo, "  HexCode 0-9a-fA-F  can't append after u/U l/L");
				} else {
					// u/U  l/L
					update_uU_lLCnt(curCh, pInfo);
				}
			} else {
				if ( inSideCharInfo->isHexNumber()  ) {
					m_alreadyTravelsaledString += curCh;
				} else {
					// u/U  l/L
					update_uU_lLCnt(curCh, pInfo);
				} 
			}
		}
	}

	return m_type;	
}


// virtual 
bool HexParser::isEnd(ParsedCharInfo* pInfo) // override;
{
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	if ( sz <= 2) {
		return false;
	}

	//  sz >=3
	string prefix = m_alreadyTravelsaledString.substr(0,2);
	if ( !(prefix == "0x" || prefix =="0X") ) {
		return false;
	}

	char ch3rd = m_alreadyTravelsaledString.at(2);
	auto inSideCharInfo = getInsideCharSetBaseInfo( ch3rd );
	if ( inSideCharInfo == nullptr ||  !( inSideCharInfo->isHexNumber() ) ) {
		return false;
	}

	return true;
}


// virtual 
bool HexParser::isTokenValid() // override;
{
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	if ( sz <= 2 ) {
		return false;
	} 

	// else >= 3
	string first2str = m_alreadyTravelsaledString.substr(0,2);
	if ( !(first2str == "0x" || first2str == "0X") ) {
		return false;
	}

	//
	// Prefix is 0x / 0X
	//
	string reststr = m_alreadyTravelsaledString.substr(2);
	int restsz = static_cast<int>( reststr.size() );

	int prefixNumberCnt = 0;
	int suffixNumberCnt = 0;
	auto meetSuffix = false;

	for( int i = 0 ; i <  restsz; ++i ) {
		char ch = reststr.at(i);
		if ( isHexCode(ch) ) {
			if ( !meetSuffix ) {
				++prefixNumberCnt;
			} else {
				++suffixNumberCnt;
			}
		} else if ( is_uU_lL(ch) ) {
			meetSuffix = true;
		}
	}

	return (prefixNumberCnt > 0 && suffixNumberCnt == 0);
}

bool HexParser::isHexCode(char ch)
{
	return    (ch>='0' && ch<='9')
		   || (ch>='a' && ch<='f')
		   || (ch>='A' && ch<='F');
}



