#ifndef OPERATOR_PARSER_H
#define OPERATOR_PARSER_H

#include "parserBase.h"

class OperatorParser : public ParserBase
{
public:
    OperatorParser();
    virtual ~OperatorParser();

    virtual ParserBase::E_PARSER_TYPE appendChar(const ChInfo& rChInfo,  E_ParserAction& afterAction) override;
    virtual TokenBase* generateToken() override;

    virtual bool isParsedSeqValid() override;
protected:

};

#endif
