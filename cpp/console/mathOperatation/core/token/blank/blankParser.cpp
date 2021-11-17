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
}


// virtual 
E_PaserType  BlankParser::appendContent(ParsedCharInfo* pInfo) // override
{
	auto curCh = pInfo->currentChar;
	if ( pInfo->baseInfo == nullptr || !( pInfo->baseInfo->isBlank() ) ) {
		//
		// blank buffer is End, TODO Check TokenList Logic
		//

		auto concreteParserType = TokenParserBase::appendContent(pInfo);
		auto pConcreteParser = TokenParserMgr::getParserByType( concreteParserType );
		if ( pConcreteParser != nullptr && pConcreteParser->isEnd() ) {
			return E_P_DEFAULT;
		} else {
			return concreteParserType;
		}
	} else {
		m_alreadyTravelsaledString += curCh;
	}


	return m_type;
}

// virtual 
TokenInfo* BlankParser::generateToken() // override
{
	auto retInfo = new TokenInfo(E_TOKEN_BLANK, E_TOKEN_BLANK);
	m_token = m_alreadyTravelsaledString;

	retInfo->setDetail(m_token);
	return retInfo;
}


// virtual 
bool BlankParser::isEnd() // override
{
	if ( m_alreadyTravelsaledString.empty() ) {
		return false;
	}

	// not empty
	char lastCh = m_alreadyTravelsaledString.back();
	auto pChInfo = getInsideCharSetBaseInfo(lastCh);
	return (pChInfo == nullptr);
}


