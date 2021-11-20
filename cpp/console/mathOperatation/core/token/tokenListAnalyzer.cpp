#include "tokenListAnalyzer.h"

TokenListAnalyzer::TokenListAnalyzer()
{
	m_tokenList.clear();
}
	
// virtual 
TokenListAnalyzer::~TokenListAnalyzer()
{
	for( auto it = m_tokenList.begin(); it != m_tokenList.end(); ++it )
	{
		if ( *it != nullptr ) {
			delete (*it);
		}
	}
	m_tokenList.clear();

}



bool TokenListAnalyzer::checkParserIsValid(E_PaserType tp)
{
	(void)tp;
	return true;
}

bool TokenListAnalyzer::pushToken(TokenInfo* pToken)
{
	m_tokenList.push_back( pToken );
	return true;
}

TokenInfo* TokenListAnalyzer::getPreviousValidToken()
{
	if ( m_tokenList.empty() ) {
		return nullptr;
	}

	TokenInfo* pRetTokenInfo = nullptr;
	for( auto r_it = m_tokenList.rbegin(); r_it != m_tokenList.rend();  )
	{
		auto pTokenInfo = *r_it;
		auto tp = pTokenInfo->getType();
		if ( tp == E_TOKEN_BLANK || tp == E_TOKEN_SINGLE_COMMENT || tp == E_TOKEN_MULTI_COMMENT ) {
			++r_it;
		} else {
			pRetTokenInfo = pTokenInfo;
			break;
		}
	}

	return pRetTokenInfo;
}

