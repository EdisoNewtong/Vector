#include "globalDirector.h"
#include "charUtil.h"
#include "cmdOptions.h"
#include "enumUtil.h"
#include "myException.h"
#include "tokenMgr.h"
#include "variblePool.h"


#include <iostream>
#include <cassert>
using namespace std;

GlobalDirector::GlobalDirector()
    //-------------------------------------
    : m_pCurrentParser(nullptr) 
    //-------------------------------------
    , m_pBaseParser(nullptr)
    , m_pBlankParser(nullptr)
    , m_pSinglelineCommentParser(nullptr)
    , m_pMultilineCommentParser(nullptr)
    , m_pOperatorParser(nullptr)
    , m_pSequenceParser(nullptr)
    , m_pSemicolonParser(nullptr)
    //-------------------------------------
    , m_buff( nullptr )
    , m_currentParserType(ParserBase::E_BASE)
{
    m_pBaseParser = new ParserBase();         // 0
    m_pBlankParser = new BlankParser();       // 1
    m_pSinglelineCommentParser = new SinglelineCommentParser();   // 2
    m_pMultilineCommentParser  = new MultilineCommentParser();     // 3
    m_pOperatorParser = new OperatorParser(); // 4
    m_pSequenceParser = new SequenceParser(); // 5
    m_pSemicolonParser = new SemiColonParser(); // 6

    // Set Current Parser
    m_pCurrentParser = m_pBaseParser;

    m_currentParserType = ParserBase::E_BASE;
}

// virtual 
GlobalDirector::~GlobalDirector()
{
    INNER_SAFE_DELETE(m_pBaseParser)
    INNER_SAFE_DELETE(m_pBlankParser)
    INNER_SAFE_DELETE(m_pSinglelineCommentParser)
    INNER_SAFE_DELETE(m_pMultilineCommentParser)
    INNER_SAFE_DELETE(m_pOperatorParser)
    INNER_SAFE_DELETE(m_pSequenceParser)
    INNER_SAFE_DELETE(m_pSemicolonParser)

    // delete the file content inside Buff::~Buff()
    INNER_SAFE_DELETE(m_buff)

    m_pCurrentParser = nullptr;
}


void GlobalDirector::setData(Buff* data)
{
    m_buff = data;
}


void GlobalDirector::switchParser(ParserBase::E_PARSER_TYPE type)
{

    if ( type == ParserBase::E_BASE ) {
        m_pCurrentParser = m_pBaseParser;
    } else if ( type == ParserBase::E_BLANK ) {
        m_pCurrentParser = m_pBlankParser;
    } else if ( type == ParserBase::E_SINGLE_LINE_COMMENT ) {
        m_pCurrentParser = m_pSinglelineCommentParser;
    } else if ( type == ParserBase::E_MULTI_LINE_COMMENT ) {
        m_pCurrentParser = m_pMultilineCommentParser;
    } else if ( type == ParserBase::E_OPERATOR ) {
        m_pCurrentParser = m_pOperatorParser;
    } else if ( type == ParserBase::E_SEQUENCE ) {
        m_pCurrentParser = m_pSequenceParser;
    } else if ( type == ParserBase::E_SEMICOLON ) {
        m_pCurrentParser = m_pSemicolonParser;
    } else {
        m_pCurrentParser = nullptr;
    }

    m_currentParserType = type;
}



void GlobalDirector::doParse()
{
    using namespace charUtil;

    if ( m_buff == nullptr ) {
        return;
    }


    auto needMove2Next = true;
    ChInfo& rChInfo = m_buff->getCursor();

    auto previousParser = m_pCurrentParser;

    while ( !m_buff->isAtEnd() )
    {
        needMove2Next = true;

        if ( m_pCurrentParser != nullptr ) {
            E_ParserAction act = E_NO_ACTION;

            inneralLog0(rChInfo);

            // core : save previous 
            auto oldType = m_currentParserType;
            previousParser = m_pCurrentParser;

            auto changeType = m_pCurrentParser->appendChar(rChInfo, act);
            if ( changeType != m_currentParserType ) {

                switchParser( changeType );

                assert( m_pCurrentParser != nullptr);
                switch( act )
                {
                case E_SWITCH_FROM_BASE_TO_SPECIAL_PARSER:
                    {
                        m_pCurrentParser->clearSequence();
                        m_pCurrentParser->resetInternalState();
                        m_pCurrentParser->pushHeadChar( previousParser->getSequence().at(0), rChInfo );

                        previousParser->clearSequence();
                    }
                    break;
                case E_TRANSFER_TO_OTHER:
                    {
                        m_pCurrentParser->clearSequence();
                        m_pCurrentParser->resetInternalState();
                        m_pCurrentParser->receiverTransfered( previousParser->getSequence() , rChInfo );

                        previousParser->clearSequence();
                    }
                    break;
                case E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR:
                case E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_STEP_NEXT:
                    {
                        needMove2Next = !(act == E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR);

                        string errorMSg;
                        if ( !previousParser->isParsedSeqValid(errorMSg) ) {
                            MyException e(E_THROW_INVALID_PARSEDSTR_TO_GENERATE_TOKEN , rChInfo);
                            e.setDetail( errorMSg + string(" : ") + previousParser->getSequence() );
                            throw e;
                        } 

                        // else generate token
                        auto token = previousParser->generateToken();
                        TokenMgr::getInstance()->pushToken( token );
                    }
                    break;
                default:
                    break;
                }
            } 

            inneralLog1(oldType, changeType);
        }

        // iterator to next character , and calculate  line/column info
        if ( needMove2Next ) {
            m_buff->moveNext();
        }
        inneralLog2(needMove2Next);

    } // end while

    //
    // After travelsal all buff , push the final token
    //
    if ( m_pCurrentParser != m_pBaseParser ) {
        string errorMSg;
        if ( !m_pCurrentParser->isParsedSeqValid(errorMSg) ) {
            MyException e(E_THROW_INVALID_PARSEDSTR_TO_GENERATE_TOKEN , rChInfo);
            e.setDetail( errorMSg + string(" : ") + m_pCurrentParser->getSequence() );
            throw e;
        }

        // else generate token
        auto token = m_pCurrentParser->generateToken();
        TokenMgr::getInstance()->pushToken( token );
    }


    auto pr = TokenMgr::getInstance()->isLastValidTokenSemicolonOrEmpty();
    if ( !pr.first ) {
        MyException e(E_THROW_LAST_VALID_TOKEN_IS_NOT_SEMICOLON);
        e.setDetail( pr.second );
        throw e;
    }


    printAllVaribles();
}


void GlobalDirector::printAllVaribles()
{
    if ( CmdOptions::needPrintVaribleFinally() ) {
        VariblePool::getPool()->printAllVaribles( CmdOptions::getFlag() );
    }
}


void GlobalDirector::inneralLog0(ChInfo& chInfo)
{
    if ( CmdOptions::needTraceParseTimeStep() ) {
        cerr << chInfo.getPos() << " ";
    }

}



void GlobalDirector::inneralLog1(ParserBase::E_PARSER_TYPE oldtp, ParserBase::E_PARSER_TYPE newtp)
{
    if ( CmdOptions::needTraceParseTimeStep()  ) {
        if ( oldtp == newtp ) {
            cerr << EnumUtil::enumName(oldtp) << " ";
        } else {
            cerr << EnumUtil::enumName(oldtp) << " -> " << EnumUtil::enumName(newtp) << " ";
        }
    }
}


void GlobalDirector::inneralLog2(bool moveNext)
{
    if ( CmdOptions::needTraceParseTimeStep() ) {
        cerr << (moveNext ? " Keep " : " Next ") << endl;
    }
}

