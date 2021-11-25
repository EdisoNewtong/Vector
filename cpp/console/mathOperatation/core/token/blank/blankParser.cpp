#include "blankParser.h"

#include "tokenParserMgr.h"


using namespace std;

BlankParser::BlankParser(E_PaserType tp)
	: TokenParserBase(tp)
{

}

// virtual 
BlankParser::~BlankParser()
{

}

// virtual 
void BlankParser::init() // override
{
	//
	m_AllAvalibleCharacters.insert(  make_pair(' ',  CharUtil::getCharBaseInfo(' ')  ) );
	m_AllAvalibleCharacters.insert(  make_pair('\t', CharUtil::getCharBaseInfo('\t') ) );
	m_AllAvalibleCharacters.insert(  make_pair('\r', CharUtil::getCharBaseInfo('\r') ) );
	m_AllAvalibleCharacters.insert(  make_pair('\n', CharUtil::getCharBaseInfo('\n') ) );

	m_tokenType = E_TOKEN_BLANK;
	m_tokenSubType = E_TOKEN_BLANK;
    m_exceptionCode = E_BLANK_INVALID_FORMAT;
	m_parserName = "BlankParser";
}


// virtual 
E_PaserType  BlankParser::appendContent(ParsedCharInfo* pInfo) // override
{
	auto curCh = pInfo->currentChar;
	auto pBlankCharInfo = getInsideCharSetBaseInfo(curCh);
	if ( pBlankCharInfo != nullptr ) {
		m_alreadyTravelsaledString += curCh;
		m_endInfo = pInfo->position;
		if ( pInfo->isLastChar ) {
			m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
			return E_P_DEFAULT;
		}
	} else {
		// Blank buffer is <End>
		m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
		return E_P_DEFAULT;
	}

	return m_type;
}




// virtual 
bool BlankParser::isEnd(ParsedCharInfo* pInfo) // override
{
	// (void)pInfo;
	if ( m_alreadyTravelsaledString.empty() ) {
		return false;
	}

	bool isAllInside = true;
	for( const auto& ch : m_alreadyTravelsaledString )
	{
		auto pCharInfo = getInsideCharSetBaseInfo(ch);
		if ( pCharInfo == nullptr ) {
			isAllInside = false;
			break;
		}
	}

	return !isAllInside;
}


