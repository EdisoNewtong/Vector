#ifndef  BLANK_PARSER_H
#define  BLANK_PARSER_H 

#include "parserBase.h"

class BlankParser : public ParserBase
{
public:
    BlankParser();
    virtual ~BlankParser() override;

    virtual ParserBase::E_PARSER_TYPE appendChar(const ChInfo& rChInfo,  E_ParserAction& afterAction) override;

    // virtual void pushHeadChar(char ch, const ChInfo& chInfo) override;
    // virtual void resetInternalState() override;
    virtual bool isParsedSeqValid(std::string& errorMSg) override;

    // virtual TokenBase* generateToken() override;
protected:

};


#endif

