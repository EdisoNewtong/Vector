#include "expevaluation.h"

using namespace std;

ExpEvaluation::ExpEvaluation(E_PaserType tp)
	: TokenParserBase(tp)
{
}

// virtual
ExpEvaluation::~ExpEvaluation()
{

}

// virtual 
void ExpEvaluation::init() // override
{
	m_AllAvalibleCharacters.insert(  make_pair(';', CharUtil::getCharBaseInfo(';')  ) );

	m_tokenType = E_TOKEN_SEMICOLON;
    m_exceptionCode = E_EXP_EVALUATION_INVALID_FORMAT;
	m_parserName = "ExpEvaluation";
}


// virtual 
bool ExpEvaluation::isEnd(ParsedCharInfo* pInfo) // override;
{
	return      (m_alreadyTravelsaledString.size() == 1)
		   &&   (m_alreadyTravelsaledString.at(0) == ';');
}

