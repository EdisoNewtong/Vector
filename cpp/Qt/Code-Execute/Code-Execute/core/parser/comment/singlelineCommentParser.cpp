#include "singlelineCommentParser.h"
#include "charUtil.h"

using namespace std;

// static 
const string SinglelineCommentParser::s_SingleCommentPrefix("//");

SinglelineCommentParser::SinglelineCommentParser()
    : ParserBase()
    , m_bIsFinished(false)
{
    m_type = E_PARSER_TYPE::E_SINGLE_LINE_COMMENT;
    m_tokenType = E_TOKEN_SINGLE_LINE_COMMENT;
}
    

// virtual 
SinglelineCommentParser::~SinglelineCommentParser()
{
}


// virtual 
ParserBase::E_PARSER_TYPE SinglelineCommentParser::appendChar(const ChInfo& rChInfo,  E_ParserAction& afterAction) // override;
{
    using namespace charUtil;

    auto retType = m_type;

    if ( rChInfo.isLastCh ) {
        m_bIsFinished = true;
        m_endChPos = rChInfo;

        retType  = ParserBase::E_BASE;
        afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_STEP_NEXT;
        return retType;
    }

    char curCh = rChInfo.ch;
    char lastCh = m_parsedSeq.back();
    if ( lastCh == NEW_LINE_R ) {
        if ( curCh == NEW_LINE_N ) {
            // \r\n
            // a
            // ^
            //     move cursor to which ^ point at ( move to next 'a' and parse 'a' by default )
            m_parsedSeq += curCh; // add \n , so the content is    "//xxxxxxxx\r\n"
            retType = ParserBase::E_BASE;
            afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_STEP_NEXT;

            m_endChPos = rChInfo;
            m_bIsFinished = true;
        } else {
            // \r
            // a
            // ^(parser 'a')   
            //                so the content is    "//xxxxxxxx\r"
            retType  = ParserBase::E_BASE;
            afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
            m_bIsFinished = true;
        }
    } else if ( lastCh == NEW_LINE_N ) {
        // \n
        // a
        // ^(parser 'a')
        //                  so the content is    "//xxxxxxxx\n"
        retType  = ParserBase::E_BASE;
        afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        m_bIsFinished = true;
    } else {
        m_parsedSeq += curCh;
        m_endChPos = rChInfo;
    }

    return retType;
}


//virtual 
void SinglelineCommentParser::receiverTransfered(const std::string& content, const ChInfo& rChInfo) // override;
{
    ParserBase::receiverTransfered(content, rChInfo);

    m_endChPos = rChInfo;

    if ( rChInfo.isLastCh ) {
        m_bIsFinished = true;
    }
}

// virtual 
void SinglelineCommentParser::resetInternalState() // override; 
{
    m_bIsFinished = false;
}


// virtual 
bool SinglelineCommentParser::isParsedSeqValid(string& errorMSg) // override;
{
    using namespace charUtil;
    auto bret = false;

    string prefix2 = m_parsedSeq.substr(0,2);
    char   lastCh  = m_parsedSeq.back();
    bret = (prefix2 == s_SingleCommentPrefix) 
        && ( m_bIsFinished || (lastCh == NEW_LINE_N || lastCh == NEW_LINE_R) );
    if ( !bret ) {
        errorMSg = "singlelineComment is not startwith //   or  endwith   \\r | \\n";
    }

    return bret;
}

