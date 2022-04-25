#include "operatorParser.h"
#include "charUtil.h"
#include "enumUtil.h"
#include "myException.h"

OperatorParser::OperatorParser()
    : ParserBase()
{
    m_type = E_PARSER_TYPE::E_OPERATOR;
    m_tokenType = E_TOKEN_OPERATOR;
}
    
// virtual 
OperatorParser::~OperatorParser()
{
}


// virtual 
ParserBase::E_PARSER_TYPE OperatorParser::appendChar(const ChInfo& rChInfo,  E_ParserAction& afterAction) // override;
{
    using namespace charutil;

    auto setEndPosFlag = false;
    ParserBase::E_PARSER_TYPE retType = m_type;

    char curCh = rChInfo.ch;

    auto firstCh = m_parsedSeq.at(0);
    auto isCurOp = isOperator(curCh);
    if ( isCurOp ) {
        // 2nd is an operator type
        switch ( firstCh )
        {
        case '/':
            {
                if ( curCh == '/' ) {
                    setEndPosFlag = true;
                    m_parsedSeq = "//";
                    afterAction = E_TRANSFER_TO_OTHER;
                    retType = ParserBase::E_SINGLE_LINE_COMMENT;
                } else if ( curCh == '*' ) {
                    setEndPosFlag = true;
                    m_parsedSeq = "/*";
                    afterAction = E_TRANSFER_TO_OTHER;
                    retType = ParserBase::E_MULTI_LINE_COMMENT;
                } else {
                    afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                    retType = ParserBase::E_BASE;
                }
            }
            break;
        case '<':
            {
                if ( curCh == '<' ) {
                    setEndPosFlag = true;
                    m_parsedSeq = "<<";
                    afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_STEP_NEXT;
                    retType = ParserBase::E_BASE;
                } else {
                     MyException e(E_THROW_INVALID_CHAR_AFTER_LEFT_BRACKET, rChInfo );
                     throw e;
                }
            }
            break;
        case '>':
            {
                if ( curCh == '>' ) {
                    setEndPosFlag = true;
                    m_parsedSeq = ">>";
                    afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_STEP_NEXT;
                    retType = ParserBase::E_BASE;
                } else {
                    MyException e(E_THROW_INVALID_CHAR_AFTER_RIGHT_BRACKET, rChInfo );
                    throw e;
                }
            }
            break;
        default:
            afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
            retType = ParserBase::E_BASE;
            break;
        }

    } else {
        // 2nd is not an operator type
        afterAction  = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        retType = ParserBase::E_BASE;
    }

    if ( setEndPosFlag ) {
        m_endChPos = rChInfo;
    }

    return retType;
}


// virtual 
TokenBase* OperatorParser::generateToken() // override;
{
    TokenBase* pGenToken = new TokenBase(E_TOKEN_OPERATOR);

    auto firstCh = m_parsedSeq.at(0);
    auto opTp = EnumUtil::getOpType(firstCh);

    pGenToken->setOpType( opTp );
    pGenToken->setTokenContent( m_parsedSeq );
    pGenToken->setBeginPos( m_beginChPos );
    pGenToken->setEndPos( m_endChPos );

    return pGenToken;
}


// virtual 
bool OperatorParser::isParsedSeqValid(std::string& errorMsg) // override
{
    auto bret = false;
    auto sz = static_cast<int>( m_parsedSeq.size() );
    if ( sz == 1 ) {
        const auto& ch = m_parsedSeq.front();
        if ( ch == '<' || ch == '>' ) {
            bret = false;
            errorMsg = "Operator with 1 char is incomplete , <  or > ";
        } else {
            bret = true;
        }
    } else if ( sz == 2 ) {
        bret = (m_parsedSeq == "<<" ||  m_parsedSeq == ">>");
        if ( !bret  ) {
            errorMsg = "Operator with 2 chars is neither  <<   nor   >>";
        }
    } else {
        bret = false;
        errorMsg = "Operator with more than 2 chars ";
    }

    return bret;
}

