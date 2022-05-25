#ifndef OPERATOR_PARSER_H
#define OPERATOR_PARSER_H

#include "parserBase.h"

class OperatorParser : public ParserBase
{
public:
    OperatorParser();
    virtual ~OperatorParser() override;

    virtual ParserBase::E_PARSER_TYPE appendChar(const ChInfo& rChInfo,  E_ParserAction& afterAction) override;
    virtual TokenBase* generateToken() override;

    virtual bool isParsedSeqValid(std::string& errorMsg) override;
protected:

};

#endif
