#include <cassert>

#include "parserBase.h" 
#include "charUtil.h"
#include "myException.h"

using namespace std;

ParserBase::ParserBase()
    : m_type(ParserBase::E_BASE)
    , m_tokenType( E_TOKEN_UNKNOWN )
    , m_parsedSeq()
    , m_beginChPos()
    , m_endChPos()
    , m_warningCnt(0)
    , m_warningContent()
{

}

// virtual 
ParserBase::~ParserBase()
{

}


ParserBase::E_PARSER_TYPE ParserBase::getType()
{
    return m_type;
}

string ParserBase::getSequence()
{
    return m_parsedSeq;
}

void ParserBase::clearSequence()
{
    m_parsedSeq.clear();
}


// virtual 
void ParserBase::resetInternalState()
{
    // Do Nothing
    return;
}



ParserBase::E_PARSER_TYPE ParserBase::appendChar(const ChInfo& rChInfo, E_ParserAction& afterAction)
{
    using namespace charUtil;

    m_beginChPos = rChInfo;
    m_endChPos   = rChInfo;

    assert( m_parsedSeq.empty() );

    char ch = rChInfo.ch;
    pushHeadChar(ch, rChInfo); // always push from ParserBase  
    auto switchType = m_type;

    switch( getCharType(ch) )
    {
    case E_CHAR_BLANK:
        switchType = ParserBase::E_BLANK;
        break;
    case E_CHAR_ALPHA:
    case E_CHAR_NUMBER:
        switchType = ParserBase::E_SEQUENCE;
        break;
    case E_CHAR_OPERATOR:
        switchType = ParserBase::E_OPERATOR;
        break;
    case E_CHAR_MISC:
        {
            if ( isSemicolon(ch) ) {
                switchType = ParserBase::E_SEMICOLON; // ';'
            } else {
                switchType = ParserBase::E_SEQUENCE; // '_'   or  '.'
            }
        }
        break;
    case E_CHAR_UNKNOWN:
        {
            MyException e(E_THROW_UNKNOWN_CHAR, rChInfo );
            throw e;
        }
        break;
    default:
        break;
    }

    afterAction = E_SWITCH_FROM_BASE_TO_SPECIAL_PARSER;

    return switchType;;
}




// virtual 
void ParserBase::pushHeadChar(char ch, const ChInfo& chInfo)
{
    assert( m_parsedSeq.empty() );
    m_parsedSeq += ch;
    m_beginChPos = chInfo;
    m_endChPos   = chInfo;
}


// virtual
void ParserBase::receiverTransfered(const string& content, const ChInfo& rChInfo)
{
    (void)rChInfo;

    assert( m_parsedSeq.empty() );
    m_parsedSeq = content;
}


// virtual 
TokenBase* ParserBase::generateToken()
{
    auto pGenRet = new TokenBase( m_tokenType );
    pGenRet->setTokenContent( m_parsedSeq );
    pGenRet->setBeginPos( m_beginChPos );
    pGenRet->setEndPos(   m_endChPos );
    if ( m_warningCnt > 0 ) {
        string title = string("Catched ") + to_string( m_warningCnt ) + (m_warningCnt>1 ? " warings : " : " warning : ");
        pGenRet->setWarningContent( title + "\n" + m_warningContent );
    }
    return pGenRet;
}

// virtual 
bool ParserBase::isParsedSeqValid(string& errorMsg)
{
    (void)errorMsg;
    return true;
}



// ChInfo    ParserBase::getBeginPos()
// {
//     return m_beginChPos;
// }
// 
// ChInfo    ParserBase::getEndPos()
// {
//     return m_endChPos;
// }




