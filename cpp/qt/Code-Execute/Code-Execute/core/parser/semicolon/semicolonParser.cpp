#include "semicolonParser.h"
#include "charUtil.h"
#include <cassert>
#include <QtGlobal>

SemiColonParser::SemiColonParser()
    : ParserBase()
{
    m_type = ParserBase::E_SEMICOLON;
    m_tokenType = E_TOKEN_SEMICOLON;
}



//virtual 
SemiColonParser::~SemiColonParser()
{

}



ParserBase::E_PARSER_TYPE SemiColonParser::appendChar(const ChInfo& rChInfo, E_ParserAction& afterAction) // override;
{
    Q_UNUSED(rChInfo)
    m_endChPos = m_beginChPos;
    // always Switch to default-Parser no mather which chacater it is
    ParserBase::E_PARSER_TYPE retType = ParserBase::E_BASE;
    afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
    
    return retType;
}


// virtual 
void SemiColonParser::pushHeadChar(char ch, const ChInfo& chInfo) // override;
{
    assert( ch == ';' );
    ParserBase::pushHeadChar(ch, chInfo);
    m_endChPos = chInfo;
}
