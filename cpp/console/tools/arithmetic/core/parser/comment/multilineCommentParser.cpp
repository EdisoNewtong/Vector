#include "multilineCommentParser.h"
#include "charUtil.h"
using namespace std;


// static 
const string MultilineCommentParser::s_COMMENT_PREFIX_STR = "/*";
const string MultilineCommentParser::s_COMMENT_SUFFIX_STR = "*/";


MultilineCommentParser::MultilineCommentParser()
    : ParserBase()
{
    m_type = E_PARSER_TYPE::E_MULTI_LINE_COMMENT;
    m_tokenType = E_TOKEN_MULTI_LINE_COMMENT;
}
    

// virtual 
MultilineCommentParser::~MultilineCommentParser()
{
}


// virtual 
ParserBase::E_PARSER_TYPE MultilineCommentParser::appendChar(const ChInfo& rChInfo,  E_ParserAction& afterAction) // override;
{
    using namespace charutil;

    m_endChPos = rChInfo;

    char ch = rChInfo.ch;
    auto retType = m_type;
    char lastCh = m_parsedSeq.back();
    
    int sz = static_cast<int>( m_parsedSeq.size() );
    m_parsedSeq += ch;
    ++sz;

    if ( lastCh == '/' && ch == '*' ) {
        //  /* 
        //      /*  inside multi-line-comment     "/*"
        ++m_warningCnt;
        m_warningContent += (to_string(m_warningCnt) + ". ");
        m_warningContent += (" \"/*\" is inside  multilineComment , @" + rChInfo.getPos(0) + "\n");
    }


    if ( sz >= 4 ) {
        string last2ch = m_parsedSeq.substr(sz-2);
        if ( last2ch == MultilineCommentParser::s_COMMENT_SUFFIX_STR ) {
            // last 2 sequence is    "*/" , Meet  the multi-line-comment end's    Demand
            afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_STEP_NEXT;
            retType = ParserBase::E_BASE;
        }
    }

    return retType;
}

// virtual 
void MultilineCommentParser::receiverTransfered( const std::string& content, const ChInfo& rChInfo) // override;
{
    // "/*" 
    ParserBase::receiverTransfered(content,rChInfo);
    m_endChPos = rChInfo;
}





// virtual 
void MultilineCommentParser::resetInternalState() // override;
{
    m_warningCnt = 0;
    m_warningContent = "";
}



// virtual 
bool MultilineCommentParser::isParsedSeqValid(string& errorMsg) // override;
{
    auto bret = false;
    const int atLeastSize = 4;
    int sz = static_cast<int>( m_parsedSeq.size() );
    if ( sz < atLeastSize ) {
        return false;
    }

    // sz >= atLeastSize
    string prefixStr = m_parsedSeq.substr(0, 2);
    string suffixStr = m_parsedSeq.substr(sz - 2);
    bret = (prefixStr == s_COMMENT_PREFIX_STR  &&  suffixStr == s_COMMENT_SUFFIX_STR);
    if ( !bret ) {
        errorMsg = "MultilineComment is not startwith  \"/*\"   or  endwith   \"*/\"";
    }

    return bret;
}

