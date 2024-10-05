#ifndef SEMICOLON_PARSER_H
#define SEMICOLON_PARSER_H

#include "parserBase.h"

class SemiColonParser : public ParserBase
{
public:
    SemiColonParser();
    virtual ~SemiColonParser() override;

    ParserBase::E_PARSER_TYPE appendChar(const ChInfo& rChInfo, E_ParserAction& afterAction) override;
    virtual void pushHeadChar(char ch, const ChInfo& rChInfo) override;
protected:
};


#endif
