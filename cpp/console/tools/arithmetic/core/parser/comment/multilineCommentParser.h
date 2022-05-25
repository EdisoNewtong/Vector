#ifndef  MULTI_LINE_COMMENT_PARSER_H
#define  MULTI_LINE_COMMENT_PARSER_H

#include "parserBase.h"
#include <string>

class MultilineCommentParser : public ParserBase
{
public:
    MultilineCommentParser();
    virtual ~MultilineCommentParser();

    virtual ParserBase::E_PARSER_TYPE appendChar(const ChInfo& rChInfo,  E_ParserAction& afterAction) override;

    virtual void resetInternalState() override;
    virtual void receiverTransfered( const std::string& content, const ChInfo& rChInfo) override;

    virtual bool isParsedSeqValid(std::string& errorMsg) override;
protected:
    static const std::string s_COMMENT_PREFIX_STR;
    static const std::string s_COMMENT_SUFFIX_STR;

};


#endif


