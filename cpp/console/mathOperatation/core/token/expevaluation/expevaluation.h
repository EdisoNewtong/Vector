#ifndef EXP_EVALUATION_H
#define EXP_EVALUATION_H

#include "commonEnum.h"
#include "tokenParserBase.h"

/*

   When Meet  ';'  at the end
   Evaluate the expression and  execute the code while the expression is Valid

*/
class ExpEvaluation : public TokenParserBase
{
public:
	ExpEvaluation(E_PaserType tp);
	virtual ~ExpEvaluation();

	virtual void init() override;
	virtual bool isEnd(ParsedCharInfo* pInfo) override;
};

#endif
