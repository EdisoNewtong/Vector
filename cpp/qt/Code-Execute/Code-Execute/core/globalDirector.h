#ifndef  GLOBAL_DIRECTOR_H
#define  GLOBAL_DIRECTOR_H

#include "parserBase.h"
#include "blankParser.h" 
#include "singlelineCommentParser.h" 
#include "multilineCommentParser.h" 
#include "operatorParser.h"
#include "sequenceParser.h" 
#include "semicolonParser.h" 

#include "buff.h"
#include "commonEnum.h"

#include <string>

#include <QString>

class GlobalDirector
{
public:
    GlobalDirector();
    virtual ~GlobalDirector();

    void setData(Buff* data);

    void doParse();
protected:
    void switchParser(ParserBase::E_PARSER_TYPE type);
    QString inneralLog0(ChInfo& chInfo);
    QString inneralLog1(ParserBase::E_PARSER_TYPE oldtp, ParserBase::E_PARSER_TYPE newtp);
    QString inneralLog2(bool moveNext);
    void printAllVaribles();
protected:
    ParserBase*       m_pCurrentParser;   // default : 0

    ParserBase*       m_pBaseParser;      // 0
    BlankParser*      m_pBlankParser;     // 1
    SinglelineCommentParser*    m_pSinglelineCommentParser;  // 2
    MultilineCommentParser*     m_pMultilineCommentParser;   // 3
    OperatorParser*   m_pOperatorParser;  // 4
    SequenceParser*   m_pSequenceParser;  // 5
    SemiColonParser*  m_pSemicolonParser;  // 6

    Buff*            m_buff;

    ParserBase::E_PARSER_TYPE  m_currentParserType;

};


#endif
