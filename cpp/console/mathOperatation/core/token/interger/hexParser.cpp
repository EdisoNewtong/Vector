#include "hexParser.h"

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

	m_tokenType = E_TOKEN_HEX_NUMBER;
}


// virtual 
E_PaserType HexParser::appendContent(ParsedCharInfo* pInfo) // override
{
	char curCh = pInfo->currentChar;
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	auto inSideCharInfo = getInsideCharSetBaseInfo( curCh );
	if ( sz == 2 ) {
		// start with 0x or 0X
		if ( inSideCharInfo == nullptr ) {
			/* 
			   e.g.
			        0xK
					  ^
			*/
			m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
			return E_P_DEFAULT;
		} else {
			if ( isHexCode(curCh) ) {
				m_alreadyTravelsaledString += curCh;
			} else {
				// 3rd char must be inside a-f A-F  0-9
				//  u/U  l/L is not allowed
				// update_uU_lLCnt(curCh);
			}
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
				if ( isHexCode(curCh) ) {
					// TODO : throw  HexCode is end ,can't append  a-f A-F 0-9
				} else {
					// u/U  l/L
					update_uU_lLCnt(curCh);
				}
			} else {
				if ( isHexCode(curCh) ) {
					m_alreadyTravelsaledString += curCh;
				} else {
					// u/U  l/L
					update_uU_lLCnt(curCh);
				} 
			}
		}
	}

	return E_P_DEFAULT;	
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
	if ( prefix != "0x" && prefix !="0X" ) {
		return false;
	}

	char ch3rd = m_alreadyTravelsaledString.at(2);
	if ( !isHexCode(ch3rd) ) {
		return false;
	}

	return true;
}

bool HexParser::isHexCode(char ch)
{
	return    (ch>='0' && ch<='9')
		   || (ch>='a' && ch<='f')
		   || (ch>='A' && ch<='F');
}



