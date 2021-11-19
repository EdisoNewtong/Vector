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
}

