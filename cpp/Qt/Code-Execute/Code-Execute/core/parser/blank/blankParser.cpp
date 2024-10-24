#include "blankParser.h"
#include "charUtil.h"


BlankParser::BlankParser()
    : ParserBase()
{
    m_type = ParserBase::E_BLANK;
    m_tokenType = E_TOKEN_BLANK;
}


// virtual
BlankParser::~BlankParser()
{

}




// virtual 
ParserBase::E_PARSER_TYPE BlankParser::appendChar(const ChInfo& rChInfo, E_ParserAction& afterAction) // override;
{
    using namespace charUtil;

    ParserBase::E_PARSER_TYPE retType = m_type;

    char currentCh = rChInfo.ch;
    if ( isBlank(currentCh) ) {
        m_parsedSeq += currentCh;
        m_endChPos = rChInfo;
    } else {
        afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        retType = ParserBase::E_BASE;
    }

    return retType;
}


// virtual 
bool BlankParser::isParsedSeqValid(std::string& errorMsg) // override;
{
    using namespace charUtil;

    auto isAllBlank = true;
    int sz = static_cast<int>( m_parsedSeq.size() );
    for( int i = 0; i < sz; ++i )
    {
        char ch =  m_parsedSeq.at(0);
        if ( !isBlank(ch) ) {

            errorMsg = "Invalid Blank char : '";
            errorMsg += ch;
            errorMsg += '\'';

            isAllBlank = false;
            break;
        }
    }
    return isAllBlank;
}

/*
// virtual 
TokenBase* BlankParser::generateToken() // override;
{
    TokenBase* pGenToken = new TokenBase(E_TOKEN_BLANK);
    pGenToken->setTokenContent( m_parsedSeq );
    return pGenToken;
}
*/

