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

