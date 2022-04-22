#ifndef PARSER_BASE_H
#define PARSER_BASE_H

#include <string>
#include "buff.h"
#include "commonEnum.h"
#include "tokenBase.h"

class ParserBase
{
public:
    enum E_PARSER_TYPE
    {
        E_BASE = 0,
        E_BLANK = 1,
        E_SINGLE_LINE_COMMENT = 2,
        E_MULTI_LINE_COMMENT  = 3,
        E_OPERATOR = 4,
        E_SEQUENCE = 5,
        E_SEMICOLON = 6,
    };

    ParserBase();
    virtual ~ParserBase();

    ParserBase::E_PARSER_TYPE getType();

    std::string getSequence();
    void        clearSequence();

    virtual void resetInternalState(); 
    virtual void receiverTransfered( const std::string& content, const ChInfo& rChInfo);

    virtual ParserBase::E_PARSER_TYPE appendChar(const ChInfo& rChInfo, E_ParserAction& afterAction );
    virtual void pushHeadChar(char ch, const ChInfo& chInfo);

    virtual TokenBase* generateToken();
    virtual bool isParsedSeqValid();

    // ChInfo    getBeginPos();
    // ChInfo    getEndPos();
protected:
    E_PARSER_TYPE   m_type;
    E_TokenType     m_tokenType;
    std::string     m_parsedSeq;

    ChInfo          m_beginChPos;
    ChInfo          m_endChPos;

};

#endif
