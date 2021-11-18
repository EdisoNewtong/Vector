#include "expevaluation.h"

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
	m_tokenType = E_TOKEN_SEMICOLON;
}

