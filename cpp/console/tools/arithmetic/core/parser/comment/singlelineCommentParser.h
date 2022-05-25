#ifndef  SINGLE_LINE_COMMENT_PARSER_H
#define  SINGLE_LINE_COMMENT_PARSER_H

#include "parserBase.h"
#include <string>

class SinglelineCommentParser : public ParserBase
{
public:
    SinglelineCommentParser();
    virtual ~SinglelineCommentParser();

    virtual ParserBase::E_PARSER_TYPE appendChar(const ChInfo& rChInfo,  E_ParserAction& afterAction) override;
    virtual void receiverTransfered( const std::string& content, const ChInfo& rChInfo) override;

    virtual void resetInternalState() override; 

    virtual bool isParsedSeqValid(std::string& errorMSg) override;
protected:
    bool m_bIsFinished;
    
    static const std::string s_SingleCommentPrefix;

};


#endif

