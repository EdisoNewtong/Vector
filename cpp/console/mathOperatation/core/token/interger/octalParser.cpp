#include "octalParser.h"

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
	
	for( char ch = '0'; ch <='7'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	m_AllAvalibleCharacters.insert( make_pair('u', CharUtil::getCharBaseInfo('u') ) );
	m_AllAvalibleCharacters.insert( make_pair('U', CharUtil::getCharBaseInfo('U') ) );
	m_AllAvalibleCharacters.insert( make_pair('l', CharUtil::getCharBaseInfo('l') ) );
	m_AllAvalibleCharacters.insert( make_pair('L', CharUtil::getCharBaseInfo('L') ) );

	m_tokenType = E_TOKEN_OCTAL_NUMBER;

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
		m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
		return E_P_DEFAULT;
	} else {
		// inSideCharInfo != nullptr 
		if( inSideCharInfo->isOctNumber() ) {
			if ( isSuffixExisted() ) {
				// TODO : throw
			} else {
				m_alreadyTravelsaledString += curCh;
			}
		} else {
			// u/U  l/L
			update_uU_lLCnt(curCh);
		}
	}

	return E_P_DEFAULT;	
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


// bool OctalParser::isOctCode(char ch)
// {
// 	return ch>='0' && ch<='7';
// }
