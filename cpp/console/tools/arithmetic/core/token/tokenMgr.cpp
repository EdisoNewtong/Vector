#include "tokenMgr.h"
#include "myException.h"
#include "enumUtil.h"
#include "opUtil.h"
#include "cmdOptions.h"
#include "charUtil.h"
#include <iterator>
#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////
//
//   Static Member Data && Functions
//
//////////////////////////////////////////////////////////////////////

TokenMgr* TokenMgr::s_gInstance = nullptr;


const TokenMgr::OpPairCfg TokenMgr::s_OpPairCfgTable[s_TABLE_SIZE] = {
    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      +
    { E_ADD,                E_ADD,       false,         true  },   // ++ is not avaliable ,    + + is avaliable
    { E_MINUS,              E_ADD,       true,          true  },
    { E_MULTIPLY,           E_ADD,       true,          true  },
    { E_DIVIDE,             E_ADD,       true,          true  },
    { E_MOD,                E_ADD,       true,          true  },
    { E_BIT_AND,            E_ADD,       true,          true  },
    { E_BIT_OR,             E_ADD,       true,          true  },
    { E_BIT_XOR,            E_ADD,       true,          true  },
    { E_BIT_NOT,            E_ADD,       true,          true  },
    { E_BIT_LEFT_SHIFT,     E_ADD,       true,          true  },
    { E_BIT_RIGHT_SHIFT,    E_ADD,       true,          true  },
    { E_OPEN_PARENTHESES,   E_ADD,       true,          true  },
    { E_CLOSE_PARENTHESES,  E_ADD,       true,          true  },
    { E_ASSIGNMENT,         E_ADD,       true,          true  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      -
    { E_ADD,                E_MINUS,     true,          true  },
    { E_MINUS,              E_MINUS,     false,         true  }, // -- is not allowed ,   - - is  avaliable
    { E_MULTIPLY,           E_MINUS,     true,          true  },
    { E_DIVIDE,             E_MINUS,     true,          true  },
    { E_MOD,                E_MINUS,     true,          true  },
    { E_BIT_AND,            E_MINUS,     true,          true  },
    { E_BIT_OR,             E_MINUS,     true,          true  },
    { E_BIT_XOR,            E_MINUS,     true,          true  },
    { E_BIT_NOT,            E_MINUS,     true,          true  },
    { E_BIT_LEFT_SHIFT,     E_MINUS,     true,          true  },
    { E_BIT_RIGHT_SHIFT,    E_MINUS,     true,          true  },
    { E_OPEN_PARENTHESES,   E_MINUS,     true,          true  },
    { E_CLOSE_PARENTHESES,  E_MINUS,     true,          true  },
    { E_ASSIGNMENT,         E_MINUS,     true,          true  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      *
    { E_ADD,                E_MULTIPLY,  false,         false  },
    { E_MINUS,              E_MULTIPLY,  false,         false  }, 
    { E_MULTIPLY,           E_MULTIPLY,  false,         false  },
    { E_DIVIDE,             E_MULTIPLY,  false,         false  },
    { E_MOD,                E_MULTIPLY,  false,         false  },
    { E_BIT_AND,            E_MULTIPLY,  false,         false  },
    { E_BIT_OR,             E_MULTIPLY,  false,         false  },
    { E_BIT_XOR,            E_MULTIPLY,  false,         false  },
    { E_BIT_NOT,            E_MULTIPLY,  false,         false  },
    { E_BIT_LEFT_SHIFT,     E_MULTIPLY,  false,         false  },
    { E_BIT_RIGHT_SHIFT,    E_MULTIPLY,  false,         false  },
    { E_OPEN_PARENTHESES,   E_MULTIPLY,  false,         false  },
    { E_CLOSE_PARENTHESES,  E_MULTIPLY,   true,          true  },  // (...)*   or  (...)   *
    { E_ASSIGNMENT,         E_MULTIPLY,  false,         false  },


    //
    //   Don't worry about the following statement          
    //                                +//
    //                             or + //
    //                             or +/* ... */
    //                             or + /* ... */
    //                             or ? //
    //                             or ? /* ... */
    //==========================================================================================================================================
    //
    // Because while push token , the token is parsed none by parser explicitly and be assigned as a token type
    //
    //==========================================================================================================================================
    //
    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      /
    { E_ADD,                E_DIVIDE,    false,         false  },
    { E_MINUS,              E_DIVIDE,    false,         false  }, 
    { E_MULTIPLY,           E_DIVIDE,    false,         false  },
    { E_DIVIDE,             E_DIVIDE,    false,         false  },
    { E_MOD,                E_DIVIDE,    false,         false  },
    { E_BIT_AND,            E_DIVIDE,    false,         false  },
    { E_BIT_OR,             E_DIVIDE,    false,         false  },
    { E_BIT_XOR,            E_DIVIDE,    false,         false  },
    { E_BIT_NOT,            E_DIVIDE,    false,         false  },
    { E_BIT_LEFT_SHIFT,     E_DIVIDE,    false,         false  },
    { E_BIT_RIGHT_SHIFT,    E_DIVIDE,    false,         false  },
    { E_OPEN_PARENTHESES,   E_DIVIDE,    false,         false  },
    { E_CLOSE_PARENTHESES,  E_DIVIDE,     true,          true  },  // (...)/   or  (...)   /
    { E_ASSIGNMENT,         E_DIVIDE,    false,         false  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      %
    { E_ADD,                E_MOD,        false,        false  },
    { E_MINUS,              E_MOD,        false,        false  }, 
    { E_MULTIPLY,           E_MOD,        false,        false  },
    { E_DIVIDE,             E_MOD,        false,        false  },
    { E_MOD,                E_MOD,        false,        false  },
    { E_BIT_AND,            E_MOD,        false,        false  },
    { E_BIT_OR,             E_MOD,        false,        false  },
    { E_BIT_XOR,            E_MOD,        false,        false  },
    { E_BIT_NOT,            E_MOD,        false,        false  },
    { E_BIT_LEFT_SHIFT,     E_MOD,        false,        false  },
    { E_BIT_RIGHT_SHIFT,    E_MOD,        false,        false  },
    { E_OPEN_PARENTHESES,   E_MOD,        false,        false  },
    { E_CLOSE_PARENTHESES,  E_MOD,         true,         true  },  // (...)%   or  (...)   %
    { E_ASSIGNMENT,         E_MOD,        false,        false  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      &
    { E_ADD,                E_BIT_AND,    false,        false  },
    { E_MINUS,              E_BIT_AND,    false,        false  }, 
    { E_MULTIPLY,           E_BIT_AND,    false,        false  },
    { E_DIVIDE,             E_BIT_AND,    false,        false  },
    { E_MOD,                E_BIT_AND,    false,        false  },
    { E_BIT_AND,            E_BIT_AND,    false,        false  }, // && or  & &    are neither allowed 
    { E_BIT_OR,             E_BIT_AND,    false,        false  },
    { E_BIT_XOR,            E_BIT_AND,    false,        false  },
    { E_BIT_NOT,            E_BIT_AND,    false,        false  },
    { E_BIT_LEFT_SHIFT,     E_BIT_AND,    false,        false  },
    { E_BIT_RIGHT_SHIFT,    E_BIT_AND,    false,        false  },
    { E_OPEN_PARENTHESES,   E_BIT_AND,    false,        false  },
    { E_CLOSE_PARENTHESES,  E_BIT_AND,     true,         true  },  // (...)&   or  (...)   &
    { E_ASSIGNMENT,         E_BIT_AND,    false,        false  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      |
    { E_ADD,                E_BIT_OR,     false,        false  },
    { E_MINUS,              E_BIT_OR,     false,        false  }, 
    { E_MULTIPLY,           E_BIT_OR,     false,        false  },
    { E_DIVIDE,             E_BIT_OR,     false,        false  },
    { E_MOD,                E_BIT_OR,     false,        false  },
    { E_BIT_AND,            E_BIT_OR,     false,        false  }, 
    { E_BIT_OR,             E_BIT_OR,     false,        false  }, // || or | | are neither allowed
    { E_BIT_XOR,            E_BIT_OR,     false,        false  },
    { E_BIT_NOT,            E_BIT_OR,     false,        false  },
    { E_BIT_LEFT_SHIFT,     E_BIT_OR,     false,        false  },
    { E_BIT_RIGHT_SHIFT,    E_BIT_OR,     false,        false  },
    { E_OPEN_PARENTHESES,   E_BIT_OR,     false,        false  },
    { E_CLOSE_PARENTHESES,  E_BIT_OR,      true,         true  },  // (...)|   or  (...)   |
    { E_ASSIGNMENT,         E_BIT_OR,     false,        false  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      ^
    { E_ADD,                E_BIT_XOR,    false,        false  },
    { E_MINUS,              E_BIT_XOR,    false,        false  }, 
    { E_MULTIPLY,           E_BIT_XOR,    false,        false  },
    { E_DIVIDE,             E_BIT_XOR,    false,        false  },
    { E_MOD,                E_BIT_XOR,    false,        false  },
    { E_BIT_AND,            E_BIT_XOR,    false,        false  }, 
    { E_BIT_OR,             E_BIT_XOR,    false,        false  }, 
    { E_BIT_XOR,            E_BIT_XOR,    false,        false  }, 
    { E_BIT_NOT,            E_BIT_XOR,    false,        false  },
    { E_BIT_LEFT_SHIFT,     E_BIT_XOR,    false,        false  },
    { E_BIT_RIGHT_SHIFT,    E_BIT_XOR,    false,        false  },
    { E_OPEN_PARENTHESES,   E_BIT_XOR,    false,        false  },
    { E_CLOSE_PARENTHESES,  E_BIT_XOR,     true,         true  },  // (...)^   or  (...)   ^
    { E_ASSIGNMENT,         E_BIT_XOR,    false,        false  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      ~
    { E_ADD,                E_BIT_NOT,    true,         true  },
    { E_MINUS,              E_BIT_NOT,    true,         true  }, 
    { E_MULTIPLY,           E_BIT_NOT,    true,         true  },
    { E_DIVIDE,             E_BIT_NOT,    true,         true  },
    { E_MOD,                E_BIT_NOT,    true,         true  },
    { E_BIT_AND,            E_BIT_NOT,    true,         true  }, 
    { E_BIT_OR,             E_BIT_NOT,    true,         true  }, 
    { E_BIT_XOR,            E_BIT_NOT,    true,         true  }, 
    { E_BIT_NOT,            E_BIT_NOT,    true,         true  },
    { E_BIT_LEFT_SHIFT,     E_BIT_NOT,    true,         true  },
    { E_BIT_RIGHT_SHIFT,    E_BIT_NOT,    true,         true  },  
    { E_OPEN_PARENTHESES,   E_BIT_NOT,    true,         true  },
    { E_CLOSE_PARENTHESES,  E_BIT_NOT,    false,        false },  // (...)~    or (...) ~  are neither allowed
    { E_ASSIGNMENT,         E_BIT_NOT,    true,         true  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      <<
    { E_ADD,                E_BIT_LEFT_SHIFT, false,     false  },
    { E_MINUS,              E_BIT_LEFT_SHIFT, false,     false  }, 
    { E_MULTIPLY,           E_BIT_LEFT_SHIFT, false,     false  },
    { E_DIVIDE,             E_BIT_LEFT_SHIFT, false,     false  },
    { E_MOD,                E_BIT_LEFT_SHIFT, false,     false  },
    { E_BIT_AND,            E_BIT_LEFT_SHIFT, false,     false  }, 
    { E_BIT_OR,             E_BIT_LEFT_SHIFT, false,     false  }, 
    { E_BIT_XOR,            E_BIT_LEFT_SHIFT, false,     false  }, 
    { E_BIT_NOT,            E_BIT_LEFT_SHIFT, false,     false  },
    { E_BIT_LEFT_SHIFT,     E_BIT_LEFT_SHIFT, false,     false  },
    { E_BIT_RIGHT_SHIFT,    E_BIT_LEFT_SHIFT, false,     false  },  
    { E_OPEN_PARENTHESES,   E_BIT_LEFT_SHIFT, false,     false  },
    { E_CLOSE_PARENTHESES,  E_BIT_LEFT_SHIFT, true,      true   },  // (...)<<    or (...) <<  are neither allowed
    { E_ASSIGNMENT,         E_BIT_LEFT_SHIFT, false,     false  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                        >> 
    { E_ADD,                E_BIT_RIGHT_SHIFT, false,    false  },
    { E_MINUS,              E_BIT_RIGHT_SHIFT, false,    false  }, 
    { E_MULTIPLY,           E_BIT_RIGHT_SHIFT, false,    false  },
    { E_DIVIDE,             E_BIT_RIGHT_SHIFT, false,    false  },
    { E_MOD,                E_BIT_RIGHT_SHIFT, false,    false  },
    { E_BIT_AND,            E_BIT_RIGHT_SHIFT, false,    false  }, 
    { E_BIT_OR,             E_BIT_RIGHT_SHIFT, false,    false  }, 
    { E_BIT_XOR,            E_BIT_RIGHT_SHIFT, false,    false  }, 
    { E_BIT_NOT,            E_BIT_RIGHT_SHIFT, false,    false  },
    { E_BIT_LEFT_SHIFT,     E_BIT_RIGHT_SHIFT, false,    false  },
    { E_BIT_RIGHT_SHIFT,    E_BIT_RIGHT_SHIFT, false,    false  },  
    { E_OPEN_PARENTHESES,   E_BIT_RIGHT_SHIFT, false,    false  },
    { E_CLOSE_PARENTHESES,  E_BIT_RIGHT_SHIFT, true,     true    },  // (...)>>    or (...) >>  are neither allowed
    { E_ASSIGNMENT,         E_BIT_RIGHT_SHIFT, false,    false  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      (
    { E_ADD,                E_OPEN_PARENTHESES, true,     true  },
    { E_MINUS,              E_OPEN_PARENTHESES, true,     true  }, 
    { E_MULTIPLY,           E_OPEN_PARENTHESES, true,     true  },
    { E_DIVIDE,             E_OPEN_PARENTHESES, true,     true  },
    { E_MOD,                E_OPEN_PARENTHESES, true,     true  },
    { E_BIT_AND,            E_OPEN_PARENTHESES, true,     true  }, 
    { E_BIT_OR,             E_OPEN_PARENTHESES, true,     true  }, 
    { E_BIT_XOR,            E_OPEN_PARENTHESES, true,     true  }, 
    { E_BIT_NOT,            E_OPEN_PARENTHESES, true,     true  },
    { E_BIT_LEFT_SHIFT,     E_OPEN_PARENTHESES, true,     true  },
    { E_BIT_RIGHT_SHIFT,    E_OPEN_PARENTHESES, true,     true  },  
    { E_OPEN_PARENTHESES,   E_OPEN_PARENTHESES, true,     true  },
    { E_CLOSE_PARENTHESES,  E_OPEN_PARENTHESES, false,    false },  // )(  or     )  (  are neither allowed
    { E_ASSIGNMENT,         E_OPEN_PARENTHESES, true,     true  },

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      )
    { E_ADD,                E_CLOSE_PARENTHESES, false,   false  },
    { E_MINUS,              E_CLOSE_PARENTHESES, false,   false  }, 
    { E_MULTIPLY,           E_CLOSE_PARENTHESES, false,   false  },
    { E_DIVIDE,             E_CLOSE_PARENTHESES, false,   false  },
    { E_MOD,                E_CLOSE_PARENTHESES, false,   false  },
    { E_BIT_AND,            E_CLOSE_PARENTHESES, false,   false  }, 
    { E_BIT_OR,             E_CLOSE_PARENTHESES, false,   false  }, 
    { E_BIT_XOR,            E_CLOSE_PARENTHESES, false,   false  }, 
    { E_BIT_NOT,            E_CLOSE_PARENTHESES, false,   false  },
    { E_BIT_LEFT_SHIFT,     E_CLOSE_PARENTHESES, false,   false  },
    { E_BIT_RIGHT_SHIFT,    E_CLOSE_PARENTHESES, false,   false  },  
    { E_OPEN_PARENTHESES,   E_CLOSE_PARENTHESES, false,   false  },  // () or  (    )   Empty content inside  () is not allowed
    { E_CLOSE_PARENTHESES,  E_CLOSE_PARENTHESES, true,    true   },    // )) or  ) ) are both avaliable
    { E_ASSIGNMENT,         E_CLOSE_PARENTHESES, false,   false  },  // =) = ) are neither allowed

    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      =
    { E_ADD,                E_ASSIGNMENT,        false,   false  }, // +=  or +  =
    { E_MINUS,              E_ASSIGNMENT,        false,   false  }, // -=
    { E_MULTIPLY,           E_ASSIGNMENT,        false,   false  }, // *=
    { E_DIVIDE,             E_ASSIGNMENT,        false,   false  }, // /=
    { E_MOD,                E_ASSIGNMENT,        false,   false  }, // %=
    { E_BIT_AND,            E_ASSIGNMENT,        false,   false  }, // &=
    { E_BIT_OR,             E_ASSIGNMENT,        false,   false  }, // |=
    { E_BIT_XOR,            E_ASSIGNMENT,        false,   false  }, // ^=
    { E_BIT_NOT,            E_ASSIGNMENT,        false,   false  }, // ~=
    { E_BIT_LEFT_SHIFT,     E_ASSIGNMENT,        false,   false  }, // <<= 
    { E_BIT_RIGHT_SHIFT,    E_ASSIGNMENT,        true,    true   }, // >>=
    { E_OPEN_PARENTHESES,   E_ASSIGNMENT,        false,   false  },  // (=
    { E_CLOSE_PARENTHESES,  E_ASSIGNMENT,        false,   false  },  // // )=     or   ) =     e.g.    (a)=3;
    { E_ASSIGNMENT,         E_ASSIGNMENT,        false,   false  }   // ==     = = are neither allowed

};

// static 
const int TokenMgr::s_MAX_KEYWORDS_CNT = 4;

// static 
const unordered_map<string, E_DataType> TokenMgr::s_keywordsDataTypeMap{
    // 1 word
    { "char",                   E_TP_CHAR         },
    { "short",                  E_TP_S_SHORT      },
    { "int",                    E_TP_S_INT        },
    { "long",                   E_TP_S_LONG       },
    { "float",                  E_TP_FLOAT        },
    { "double",                 E_TP_DOUBLE       },
    { "signed",                 E_TP_S_INT        },
    { "unsigned",               E_TP_U_INT        },
    // 2 words
    { "unsigned char",          E_TP_U_CHAR       },
    { "signed char",            E_TP_S_CHAR       },
    { "unsigned short",         E_TP_U_SHORT      },
    { "signed short",           E_TP_S_SHORT      },
    { "unsigned int",           E_TP_U_INT        },
    { "signed int",             E_TP_S_INT        },
    { "unsigned long",          E_TP_U_LONG       },
    { "signed long",            E_TP_S_LONG       },
    { "long int",               E_TP_S_LONG       },
    { "long long",              E_TP_S_LONG_LONG  },
    // 3 words
    { "unsigned long int",      E_TP_U_LONG       },
    { "signed long int",        E_TP_S_LONG       },
    { "unsigned long long",     E_TP_U_LONG_LONG  },
    { "signed long long",       E_TP_S_LONG_LONG  },
    { "long long int",          E_TP_S_LONG_LONG  },
    // 4 words
    { "unsigned long long int", E_TP_U_LONG_LONG  },
    { "signed long long int",   E_TP_S_LONG_LONG  }
};


// static 
std::list<TokenBase*> TokenMgr::s_generatedTmpTokenPool{};


void TokenMgr::init()
{
    if ( s_gInstance != nullptr ) { return; }
    s_gInstance = new TokenMgr();
}

void TokenMgr::release()
{
    clearTmpGenTokenPool();
    INNER_SAFE_DELETE(s_gInstance);
}

TokenMgr* TokenMgr::getInstance()
{
    return s_gInstance;
}




// static
bool TokenMgr::isCommentType(E_TokenType tp)
{
    return tp == E_TOKEN_SINGLE_LINE_COMMENT || tp == E_TOKEN_MULTI_LINE_COMMENT;
}


// static 
bool TokenMgr::isBlankType(E_TokenType tp)
{
    return tp == E_TOKEN_BLANK;
}

// static 
bool TokenMgr::isIgnoredType(E_TokenType tp)
{
    return TokenMgr::isBlankType(tp) || TokenMgr::isCommentType(tp);
}



TokenMgr::TokenMgr()
    : m_allTokenList()
    , m_validTokenList()
    , m_oneSentence()
    , m_opertorStack()
    , m_suffixExpression()
    , m_execodeIdx(0)
{
    m_allTokenList.clear();
    m_validTokenList.clear();

    m_oneSentence.clear();
    m_opertorStack.clear();
    m_suffixExpression.clear();
}

// virtual
TokenMgr::~TokenMgr()
{
    for( auto it = m_allTokenList.begin(); it != m_allTokenList.end(); ++it )
    {
        auto pToken = *it;
        if ( pToken != nullptr ) {
            delete pToken;
            pToken = nullptr;
        }
    }
    m_allTokenList.clear();

    m_validTokenList.clear();

    m_oneSentence.clear();
    m_opertorStack.clear();
    m_suffixExpression.clear();
}





void TokenMgr::pushToken(TokenBase* pToken)
{
    using namespace charutil;

    if ( pToken == nullptr ) {
        MyException e(E_THROW_NULL_PTR);

        string detail( __FILE__ );
        detail += (string(" TokenMgr::pushToken( nullptr ) @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    auto tokenType = pToken->getTokenType();
    auto pr = checkTokenValidFromPreviousRelationship(pToken);
    auto isValid = pr.first;
    if ( !isValid ) {
        auto previousToken = pr.second;
        MyException e(E_THROW_INVALID_TOKEN_RELATIONSHIP);

        string detailstr;
        if ( previousToken == nullptr ) {
            detailstr += "nullptr";
            detailstr += SPACE_2;
            detailstr += "After '";
            detailstr += (pToken->getTokenContent() + SPACE_1 + pToken->getBeginPos().getPos(0) + SINGLE_QUOTO);
        } else {
            //  previousToken != nullptr
            // auto preTp = previousToken->getTokenType();

            auto leftContent = previousToken->getTokenContent();

            //if ( preTp  == E_TOKEN_OPERATOR ) {
            //    detailstr += EnumUtil::enumName( previousToken->getOperatorType() );
            //} else {
            //    detailstr += EnumUtil::enumName( preTp );
            //}
            detailstr += (SPACE_1 + SINGLE_QUOTO + leftContent + SINGLE_QUOTO);
            detailstr += " @";
            detailstr += previousToken->getBeginPos().getPos(0);


            auto rightContent = pToken->getTokenContent();
            detailstr += SPACE_1;
            //if ( tokenType == E_TOKEN_OPERATOR ) {
            //    detailstr += EnumUtil::enumName( pToken->getOperatorType() );
            //} else {
            //    detailstr += EnumUtil::enumName( tokenType );
            //}
            detailstr += (SPACE_1 + SINGLE_QUOTO + rightContent + SINGLE_QUOTO);
            detailstr += " @";
            detailstr += pToken->getBeginPos().getPos(0);
        }
        detailstr += " is not allowed";
        e.setDetail(detailstr);
        throw e;
    }

    //
    // Always Push to All-Token-List
    //
    m_allTokenList.push_back(pToken);
    tracePushedTokenWarning(pToken);


    //      Skip :  Blank / Single-Comment / Multi-Comment   <===   such types can be ignored
    //      Operator or Sequence or    ;
    if ( !( TokenMgr::isIgnoredType(tokenType) ) ) {
        // int pushedIdx = static_cast<int>( m_validTokenList.size() );
        m_validTokenList.push_back( pToken );

        if ( tokenType == E_TOKEN_SEMICOLON ) {
            // Core Core Core
            executeCode();
            ++m_execodeIdx;
        } else {
            m_oneSentence.push_back( pToken );
        }
    }

}


pair<bool,string>
TokenMgr::isLastValidTokenSemicolonOrEmpty()
{
    if ( m_validTokenList.empty() ) {
        return make_pair(true,"");
    }

    auto lastElement = m_validTokenList.back();
    auto tp = lastElement->getTokenType();
    auto content = lastElement->getTokenContent();
    auto isValid = (tp == E_TOKEN_SEMICOLON);
    return make_pair(isValid, content);
}



void TokenMgr::executeCode()
{
    //////////////////////////////////////////////////////////////////////
    //
    // Core Core Core
    //
    //////////////////////////////////////////////////////////////////////
    printOperatorStack();
    m_opertorStack.clear();
    printSuffixExpression(1);
    m_suffixExpression.clear();

    if ( m_oneSentence.empty() ) {
        traceBlankStatement();
        return;
    }
    
    int sentenceType = -1;

    // Sequence / operator   Only
    int vecSz = static_cast<int>( m_oneSentence.size() );

    // if ( vecSz < 2 ) {
    //     MyException e(E_THROW_SENTENCE_TOO_LESS_TOKEN, m_oneSentence.front()->getBeginPos() );
    //     // e.setDetail( m_oneSentence.front()->getBeginPos().getPos() );
    //     throw e;
    // } 

    // vecSz >=2
    int equal1stIdx = -1;
    int equal2ndIdx = -1;
    int equalCnt = 0; // '=' count
    for( auto idx = 0; idx < vecSz;  ++idx )
    {
        TokenBase* pToken = m_oneSentence.at(idx);
        auto tokenType = pToken->getTokenType();
        // tokenVec.push_back( tokenType );

        if ( tokenType == E_TOKEN_OPERATOR   &&   pToken->getOperatorType() == E_ASSIGNMENT ) {
            ++equalCnt;
            if ( equalCnt == 1 ) {
                equal1stIdx = idx;
            } else if ( equalCnt == 2 ) {
                equal2ndIdx = idx;
            }
        }
    }

    if ( equalCnt >= 2 ) {
        MyException e(E_THROW_SENTENCE_TOO_MORE_ASSIGNMENT, m_oneSentence.at(equal2ndIdx)->getBeginPos() );
        // e.setDetail( m_oneSentence.at(equal2ndIdx)->getBeginPos().getPos() );
        throw e;
    }

    E_DataType defDt = E_TP_UNKNOWN;
    string varname;
    auto varIdx = 0;
    auto hasTokenEqual = (equalCnt != 0);
    if ( equalCnt == 0 ) {
        if ( is1stTokenKeyWord() ) {
            // 1. int a;
            varIdx = (vecSz - 1);
            defDt = checkPrefixKeyWordsAndGetDataType(varIdx, varname, hasTokenEqual);
            sentenceType = 1;
        } else {
            varIdx = 0;
            sentenceType = 4; // all token is either expression ( can't not be keyword ) or operator
        }
    } else {
        // 1 '='
        // make sure  there must be at least 1 content before and after  '='
        if ( equal1stIdx == 0 ) {
            MyException e(E_THROW_SENTENCE_NO_EXPR_BEFORE_ASSIGNMENT, m_oneSentence.front()->getBeginPos() );
            // e.setDetail( m_oneSentence.front()->getBeginPos().getPos() );
            throw e;
        } else if ( equal1stIdx == (vecSz -1) ) {
            MyException e(E_THROW_SENTENCE_NO_EXPR_AFTER_ASSIGNMENT , m_oneSentence.back()->getBeginPos() );
            // e.setDetail( m_oneSentence.back()->getBeginPos().getPos() );
            throw e;
        }

        varIdx = (equal1stIdx - 1);
        // equal1stIdx >=1
        if ( equal1stIdx > 1 ) {
            defDt  = checkPrefixKeyWordsAndGetDataType(varIdx, varname, hasTokenEqual);
            // 2. int a = 3;
            sentenceType = 2;
        } else {
            // equal1stIdx == 1  =>     varname = expression
            auto varElement = m_oneSentence.at(varIdx);
            if ( !(varElement->getTokenType() == E_TOKEN_EXPRESSION &&    varElement->isVarible()) ) {
                MyException e(E_THROW_SENTENCE_DEFINITION_SUFFIX_IS_NOT_VARIBLE , varElement->getBeginPos() );
                // e.setDetail( varElement->getBeginPos().getPos() );
                throw e;
            }
            varname = varElement->getTokenContent();
            // 3. a = 3;
            sentenceType = 3;
        }
    }

    /*
    --------------------------------------------------
        Only 3 kinds of format are all allowed
    --------------------------------------------------

        1. DataType    varible                   (;)    type-id = 1
        2. DataType    varible = expression      (;)    type=id = 2
        3.             varible = expression      (;)    type-id = 3
        4. Tmp expression                        (;)    such as     a + b * c;

        ///////////////////////////////////////////////////////////////////////
        //
        // Core Core Core : build suffix-expression after '='
        //
        ///////////////////////////////////////////////////////////////////////

    */


    auto sentenceVarElement = m_oneSentence.at(varIdx);
    VaribleInfo* pVaribleInfo = nullptr;
    if ( sentenceType == 1 ) {
        // 1. e.g.  unsigned int a;
        pVaribleInfo = VariblePool::getPool()->create_a_new_varible(defDt, varname, sentenceVarElement->getBeginPos().line );
        sentenceVarElement->setRealValue( pVaribleInfo->dataVal );
    } else if ( sentenceType == 2 ) {
        // 2. e.g.  unsigned int           a = 3;
        pVaribleInfo = VariblePool::getPool()->create_a_new_varible(defDt, varname, sentenceVarElement->getBeginPos().line );
        sentenceVarElement->setRealValue( pVaribleInfo->dataVal );
    } else if ( sentenceType == 3 ) {
        // 3. e.g.               a   = 3;
        pVaribleInfo = VariblePool::getPool()->getVaribleByName(varname);
        if ( pVaribleInfo == nullptr ) {
            MyException e(E_THROW_VARIBLE_NOT_DEFINED, sentenceVarElement->getBeginPos() );
            e.setDetail( varname );
            throw e;
        }
    } 


    if ( sentenceType != 1 ) {
        //                              == 4 && set need process tmp expression as    true
        if ( sentenceType != 4   ||   CmdOptions::needProcessTmpExpressionWithoutAssignment() ) {
            buildSuffixExpression(sentenceType, varIdx);
            checkSuffixExpressionValid();
            popAllOperatorStack();
            printSuffixExpression(2);

            evaluateSuffixExpression();
            printSuffixExpression(3);
            m_suffixExpression.clear(); // clear the only 1 element
        }

    }


    // After Execute , clear
    m_oneSentence.clear();
    clearTmpGenTokenPool();
}





void TokenMgr::buildSuffixExpression(int sentenceType, int varibleIdx)
{
    int sententSz = static_cast<int>( m_oneSentence.size() );

    for( int idx = varibleIdx; idx < sententSz; ++idx )
    {
        auto pToken = m_oneSentence.at(idx);
        auto tokenType = pToken->getTokenType();
        if ( tokenType == E_TOKEN_EXPRESSION ) {
            // fixed literal / varible / KeyWord
            if ( pToken->isKeyword() ) {
                MyException e(E_THROW_CANNOT_PUSH_TOKEN_KEYWORD, pToken->getBeginPos() );
                throw e;
            } else if ( pToken->isVarible() ) {
                string pVisitVarName = pToken->getTokenContent();

                VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( pVisitVarName );
                if ( pVisitedVaribleInfo == nullptr ) {
                    MyException e(E_THROW_VARIBLE_NOT_DEFINED , pToken->getBeginPos());
                    e.setDetail( pVisitVarName );
                    throw e;
                }

                auto indexCheck = false;
                if ( sentenceType == 4 ) {
                    indexCheck = true;
                } else {
                    indexCheck = (idx > varibleIdx);
                }

                if ( indexCheck &&  !(pVisitedVaribleInfo->isInitialed) ) {
                    if ( CmdOptions::needTreatUninitializedVaribleAsError()  ) {
                        MyException e(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED, pToken->getBeginPos() );
                        throw e;
                    } else {
                        traceUnInitializedVarWhenUsed(pToken);
                    }

                }

                pToken->setRealValue( pVisitedVaribleInfo->dataVal );
            }

            m_suffixExpression.push_back( pToken );
            traceSuffixExpression( pToken , true);
        } else {
            // must be a type of operator
            TokenBase* previousToken;
            auto previousIdx = (idx - 1);
            if ( previousIdx < varibleIdx ) {
                previousToken = nullptr;
            } else {
                previousToken = m_oneSentence.at(previousIdx);
            }
            processOperatorStack(previousToken, pToken);
        }
    }
}


void TokenMgr::checkSuffixExpressionValid()
{
    TokenBase* errorToken = nullptr;
    for( auto it = m_suffixExpression.begin(); it != m_suffixExpression.end(); ++it )
    {
        auto pToken = *it;
        auto tp = pToken->getTokenType();

        if ( tp == E_TOKEN_EXPRESSION ) {
            if ( !(pToken->isVarible() || pToken->isFixedLiteral()) ) {
                errorToken = pToken;
                break;
            }
        } else if ( tp == E_TOKEN_OPERATOR ) {
            auto opTp = pToken->getOperatorType();
            if ( opTp == E_OPERATOR_UNKNOWN || opTp == E_OPEN_PARENTHESES || opTp == E_CLOSE_PARENTHESES ) {
                errorToken = pToken;
                break;
            }
        } else {
            errorToken = pToken;
            break;
        }
    }


    if ( errorToken != nullptr ) {
        // auto errorTp = errorToken->getTokenType();
        MyException e(E_THROW_INVALID_SUFFIX_EXPRESSION, errorToken->getBeginPos() );
        // e.setDetail( EnumUtil::enumName(errorTp)  + string(" , ") +  errorToken->getBeginPos().getPos() );
        throw e;
    }
}


bool TokenMgr::is1stTokenKeyWord()
{
    auto headtoken = m_oneSentence.front();
    return headtoken->getTokenType() == E_TOKEN_EXPRESSION  &&   headtoken->isKeyword();
}







void TokenMgr::processOperatorStack(TokenBase* previousToken, TokenBase* pToken)
{
    // change operator if original operator is '+' / '-'
    // '+' Addition    -->     '+' Positive
    // '-' Minus       -->     '-' Negative
    auto opType = pToken->getOperatorType();
    if ( opType == E_ADD   || opType == E_MINUS ) {
        E_OperatorType changedTp = opType;

        auto isAdd = (opType == E_ADD);
        if ( previousToken == nullptr ) {
            // Positive  or Negative
            changedTp = (isAdd ? E_POSITIVE : E_NEGATIVE);
            pToken->setOpType(changedTp);

            tracePositiveNegativeFlag( previousToken, opType );

            opType = changedTp;

        } else {
            auto previousTp = previousToken->getTokenType();
            if ( previousTp == E_TOKEN_OPERATOR   &&   previousToken->getOperatorType() != E_CLOSE_PARENTHESES ) {
                changedTp = (isAdd ? E_POSITIVE : E_NEGATIVE);

                tracePositiveNegativeFlag( previousToken, opType );

                pToken->setOpType(changedTp);


                opType = changedTp;
            }
        }
    }



    // Core Core Core
    //    Generate Suffix Expression
    if ( opType == E_CLOSE_PARENTHESES ) {
        if ( !hasPreviousExistOpenParentheses() ) {
            MyException e(E_THROW_NO_MATCHED_OPEN_PARENTHESES, pToken->getBeginPos() );
            // e.setDetail( pToken->getBeginPos().getPos() );
            throw e;
        } else {
            //  toPushed is ')' , 
            //    Do not push ')'      
            //    Pop previous operator till   '('
            popUntilOpenParentheses();
        }
    } else {

        // operator is not ')'
        if ( m_opertorStack.empty() ) { // stack is empty
            m_opertorStack.push_back( pToken );
            traceOperatorStack( pToken, true);
            return;
        }

        // else :
        //      list is not empty
        auto toPushedOpMeta    = OpUtil::getOpInfo(opType);
        auto toPushed_p = toPushedOpMeta.getPriority();
        // auto current_rightAss = toPushedOpMeta.isRight2Left();
        auto forwardIt = m_opertorStack.begin();

        auto pushedFlag = false;
        for( auto rit = m_opertorStack.rbegin(); rit != m_opertorStack.rend(); )
        {
            auto pVisitToken = *rit;
            auto visitOpType = pVisitToken->getOperatorType();
            auto cmpOpMeta = OpUtil::getOpInfo( visitOpType );
            auto cmp_p = cmpOpMeta.getPriority();
            auto cmp_rightAss = toPushedOpMeta.isRight2Left();

            if ( toPushed_p < cmp_p ) {
                m_opertorStack.insert( rit.base(), pToken );
                traceOperatorStack( pToken, true );

                pushedFlag = true;
            } else if ( toPushed_p == cmp_p ) {
                if ( opType == E_OPEN_PARENTHESES ) { // '(' is right-to-Left Associativity
                    m_opertorStack.insert( rit.base(), pToken );
                    traceOperatorStack( pToken, true );
                } else {
                    if ( cmp_rightAss ) {
                        // Associativity is   right 2 left
                        forwardIt = m_opertorStack.insert( rit.base(), pToken );
                        traceOperatorStack( pToken, true );
                        rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                    } else {
                        // Associativity is   left 2 right , 
                        //   1. copy current op into  m_suffixExpression
                        //   2. pop only one : the current
                        //   3. insert the same priority operator
                        m_suffixExpression.push_back( pVisitToken ); // 1
                        traceSuffixExpression( pVisitToken, true );

                        forwardIt = m_opertorStack.erase( (++rit).base() ); // 2
                        rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                        traceOperatorStack( pVisitToken, false );

                        m_opertorStack.insert( forwardIt, pToken ); // 3
                        rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                        traceOperatorStack( pToken, true );
                    }
                }

                pushedFlag = true;
            } else {
                // toPushed_p > cmp_p
                if ( visitOpType == E_OPEN_PARENTHESES ) {
                    forwardIt = m_opertorStack.insert( rit.base(), pToken );
                    rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                    traceOperatorStack( pToken, true );

                    pushedFlag = true;
                } else {
                    forwardIt = m_opertorStack.erase( (++rit).base() );
                    rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                    traceOperatorStack( pVisitToken, false );

                    m_suffixExpression.push_back( pVisitToken );
                    traceSuffixExpression( pVisitToken, true );
                }
            }

            if ( pushedFlag ) {
                break;
            }
        }


        if ( !pushedFlag ) {
            m_opertorStack.push_back( pToken );
            traceOperatorStack( pToken, true );
        }
    }
}


void TokenMgr::evaluateSuffixExpression()
{
    int operatorProcessCnt = 0; // count for do binaryOp/UnaryOp Cnt
    int previousExpCnt = 0;
    for( auto it = m_suffixExpression.begin(); it != m_suffixExpression.end(); ) 
    {
        auto currentElement = *it;
        auto currentTokenType = currentElement->getTokenType();
        if ( currentTokenType != E_TOKEN_OPERATOR ) {
            ++it;
            ++previousExpCnt;
        } else {
            // is an operator
            auto opType = currentElement->getOperatorType();
            if ( opType == E_OPERATOR_UNKNOWN || opType == E_OPEN_PARENTHESES || opType == E_CLOSE_PARENTHESES ) {
                MyException e(E_THROW_INVALID_OPERATOR , currentElement->getBeginPos() );
                string detailstr = "inside suffix expression : ";
                detailstr += EnumUtil::enumName( opType );
                detailstr += (string(", ") + currentElement->getBeginPos().getPos() );
                e.setDetail( detailstr );
                throw e;
            }

            auto opMeta = OpUtil::getOpInfo( opType );
            auto isbinaryOp = !opMeta.isUnaryOp();
            if ( isbinaryOp ) {
                if ( previousExpCnt < 2 ) {
                    MyException e(E_THROW_SUFFIXEXPR_BINARY_OP_MISS_TWO_OPERANDS, currentElement->getBeginPos() );
                    // e.setDetail( currentElement->getBeginPos().getPos() );
                    throw e;
                }

                auto nextIt = next(it);

                TokenBase* leftOperand  = nullptr;
                TokenBase* rightOperand = nullptr;

                it = prev(it,1);
                rightOperand = *it;

                it = prev(it,1);
                leftOperand = *it;

                auto genTmpExp = doBinaryOp(leftOperand, currentElement, rightOperand);
                ++operatorProcessCnt;

                it = m_suffixExpression.erase(it, nextIt);
                it = m_suffixExpression.insert(it, genTmpExp);

                // previousExpCnt -= 3;
                // previousExpCnt += 1;
                previousExpCnt -= 2;
            } else {
                if ( previousExpCnt < 1 ) {
                    MyException e(E_THROW_SUFFIXEXPR_UNARY_OP_MISS_ONE_OPERAND, currentElement->getBeginPos());
                    // e.setDetail( currentElement->getBeginPos().getPos() );
                    throw e;
                }

                auto nextIt = next(it);
                TokenBase* rightOperand = nullptr;

                it = prev(it,1);
                rightOperand = *it;

                auto genTmpExp = doUnaryOp(currentElement, rightOperand);
                ++operatorProcessCnt;

                it = m_suffixExpression.erase(it, nextIt);
                it = m_suffixExpression.insert(it, genTmpExp);

                // previousExpCnt -= 2;
                // previousExpCnt += 1;
                --previousExpCnt;
            }

        }
    }

    // special condition if tmp expression has no operator
    if (  operatorProcessCnt == 0 ) {
        for( auto it = m_suffixExpression.begin(); it != m_suffixExpression.end(); ++it ) 
        {
            auto currentElement = *it;
            auto currentTokenType = currentElement->getTokenType();
            if ( currentTokenType == E_TOKEN_EXPRESSION  ) {
                auto content = currentElement->getTokenContent();
                auto dataVal = currentElement->getRealValue();
                traceTmpOpResult(content , dataVal);
            } else {
                MyException e(E_THROW_CODE_CANNOT_REACH_HERE, currentElement->getBeginPos()  );
                e.setDetail(" When operatorStack is empty or binary/unary operator's count == 0 ");
                throw e;
            }
        }
    }
}


TokenBase* TokenMgr::doBinaryOp(TokenBase* left, TokenBase* op, TokenBase* right)
{
    TokenBase* genTmpExp = nullptr;

    switch( op->getOperatorType() )
    {
    case E_ADD:
        genTmpExp = doAdd(left,right);
        break;
    case E_MINUS:
        genTmpExp = doMinus(left,right);
        break;
    case E_MULTIPLY:
        genTmpExp = doMultiply(left,right);
        break;
    case E_DIVIDE:
        genTmpExp = doDivide(left,right);
        break;
    case E_MOD:
        genTmpExp = doMod(left,right);
        break;
    case E_BIT_AND:
        genTmpExp = doBitAnd(left,right);
        break;
    case E_BIT_OR:
        genTmpExp = doBitOr(left,right);
        break;
    case E_BIT_XOR:
        genTmpExp = doBitXor(left,right);
        break;
    case E_BIT_LEFT_SHIFT:
        genTmpExp = doBitLeftShift(left,right);
        break;
    case E_BIT_RIGHT_SHIFT:
        genTmpExp = doBitRightShift(left,right);
        break;
    case E_ASSIGNMENT:
        genTmpExp = doAssignment(left,right);
        break;
    default:
        break;
    }

    return genTmpExp;
}

TokenBase* TokenMgr::doUnaryOp(TokenBase* op, TokenBase* right)
{
    TokenBase* genTmpExp = nullptr;
    switch( op->getOperatorType() )
    {
    case E_POSITIVE:
        genTmpExp = doPositive(op,right);
        break;
    case E_NEGATIVE:
        genTmpExp = doNegative(op,right);
        break;
    case E_BIT_NOT:
        genTmpExp = doBitNot(op,right);
        break;
    default:
        break;
    }

    return genTmpExp;
}



void TokenMgr::popUntilOpenParentheses()
{
    for( auto rit = m_opertorStack.rbegin(); rit != m_opertorStack.rend(); )
    {
        TokenBase* pElement = *rit;
        auto isopenParentheses = (pElement->getOperatorType() == E_OPEN_PARENTHESES);

        traceOperatorStack( pElement, false );
        auto fit = m_opertorStack.erase( (++rit).base() );
        rit = reverse_iterator< decltype(fit) >( fit );

        if ( isopenParentheses ) {
            // meet the matched    '('
            break;
        } else {
            m_suffixExpression.push_back( pElement );
            traceSuffixExpression( pElement , true);
        }
    }

}





E_DataType TokenMgr::checkPrefixKeyWordsAndGetDataType(int varIdx, string& varname, bool hasTokenEqual )
{
    using namespace charutil;

    E_DataType dt = E_TP_UNKNOWN;

    auto isValid = true;
    // auto errorIdx = -1;
    // auto lastIdx = vecSz - 1;
    auto keywordsCnt = 0;
    string keywordsSeq;
    for( auto idx = 0; idx < varIdx; ++idx )
    {
        auto pToken = m_oneSentence.at(idx);
        auto content = pToken->getTokenContent();

        if ( !(pToken->getTokenType() == E_TOKEN_EXPRESSION  && pToken->isKeyword() ) ) {
            // errorIdx = idx;
            isValid = false;
            break;
        } else {
            ++keywordsCnt;
            keywordsSeq += content;
            if ( idx != (varIdx-1) ) {
                keywordsSeq += SPACE_1;
            }
        }
    }

    if ( !isValid ) {
        string strWithVarible;
        for( auto idx = 0; idx <= varIdx; ++idx ) { strWithVarible += m_oneSentence.at(idx)->getTokenContent(); }

        E_ExceptionType errorCode = (hasTokenEqual  ? E_THROW_SENTENCE_PREFIX_TOKENS_BEFORE_EQUAL_ARE_INVALID 
                                                    : E_THROW_SENTENCE_PREFIX_TOKENS_ARE_INVALID);
        MyException e(errorCode);
        string errorMsg = ( SINGLE_QUOTO + strWithVarible + SINGLE_QUOTO + " @line: " + to_string( m_oneSentence.at(varIdx)->getBeginPos().line ) ); 
        e.setDetail(errorMsg);
        throw e;
    }

    if ( keywordsCnt > s_MAX_KEYWORDS_CNT ) {
        MyException e(E_THROW_SENTENCE_DEFINITION_TOO_MANY_KEYWORDS, m_oneSentence.at(0)->getBeginPos() );
        throw e;
    }

    auto varElement = m_oneSentence.at(varIdx);
    if ( !(varElement->getTokenType() == E_TOKEN_EXPRESSION    &&   varElement->isVarible()) ) {
        MyException e(E_THROW_SENTENCE_DEFINITION_SUFFIX_IS_NOT_VARIBLE , varElement->getBeginPos() );
        throw e;
    }
    varname = varElement->getTokenContent();

    auto it = s_keywordsDataTypeMap.find(keywordsSeq);
    if ( it == s_keywordsDataTypeMap.end() ) {
        MyException e(E_THROW_SENTENCE_UNKNOWN_DATA_TYPE);
        e.setDetail( keywordsSeq );
        throw e;
    }

    dt = it->second;
    return dt;
}


bool TokenMgr::hasPreviousExistOpenParentheses()
{
    auto foundLeft = false;
    
    for( auto rit = m_opertorStack.rbegin(); rit != m_opertorStack.rend(); ++rit )
    {
        TokenBase* pElement = *rit;
        if ( pElement->getOperatorType() == E_OPEN_PARENTHESES )  {
            foundLeft = true;
            break;
        }
    }

    return foundLeft;
}


//
// *** Core Core Core *** Function
//   Key Logic
pair<bool,TokenBase*> TokenMgr::checkTokenValidFromPreviousRelationship(TokenBase* toBePushed)
{
    auto avaliable = false;
    auto needProcess = false;
    TokenBase* previousToken = nullptr;

    auto pr = getPreviousToken();
    if ( pr.first == nullptr ) {
        avaliable = true;
        return make_pair(true, previousToken);
    }

    auto toBePushedTp = toBePushed->getTokenType();
    switch ( toBePushedTp )
    {
    case E_TOKEN_BLANK:
    case E_TOKEN_SINGLE_LINE_COMMENT:
    case E_TOKEN_MULTI_LINE_COMMENT:
        {
            needProcess = false;
            avaliable = true;
            previousToken = pr.first;
        }
        break;
    case E_TOKEN_EXPRESSION:
    case E_TOKEN_OPERATOR:
    case E_TOKEN_SEMICOLON:
        needProcess = true;
        break;
    default:
        break;
    }


    if ( !needProcess ) {
        return make_pair(avaliable, previousToken);
    }
    //
    // else : Expression , Operator , Semicolon 
    //     => Need process in the following statement
    //

    if ( toBePushedTp == E_TOKEN_SEMICOLON ) {
        previousToken = pr.first;
        if ( previousToken != nullptr ) {
            if ( previousToken->getTokenType() == E_TOKEN_OPERATOR ) {
                // Operator type
                if (  previousToken->getOperatorType() == E_CLOSE_PARENTHESES ) {
                    avaliable = true;
                } else {
                    avaliable = false;
                }
            } else {
                // None operator type , e.g.  1. ;;   2. aaa;
                avaliable = true;
            }
        } else {
            // previousToken == nullptr , 
            // ';' is the 1st token to be pushed
            avaliable = true;
        }

        return make_pair(avaliable, previousToken);
    }


    //  toBePushedTp is an Sequence or Operator
    if ( toBePushedTp == E_TOKEN_EXPRESSION ) {
        previousToken = pr.first;
        if ( previousToken != nullptr ) {
            auto previousTp = previousToken->getTokenType();
            if ( previousTp == E_TOKEN_SEMICOLON ) {
                avaliable = true;
            } else if ( previousTp == E_TOKEN_EXPRESSION ) {
                // avaliable = false; // "a b"     is not avaliable
                if ( previousToken->isKeyword() ) {
                    if ( toBePushed->isKeyword() ) {
                        avaliable = true;
                    } else if ( toBePushed->isVarible() ) {
                        avaliable = true;
                    } else {
                        avaliable = false; // fixed literal  
                    }
                } else  {
                    // previous is not keyword , may be varible or fixed literanl
                    avaliable = false;
                }
            } else {
                // previousTp is an operator type
                if ( previousToken->getOperatorType() == E_CLOSE_PARENTHESES ) {
                    avaliable = false;
                } else {
                    // e.g.     +;   -;  *; /; %;    ....
                    avaliable = true;
                }
            }
        } else {
            avaliable = true; // previousToken is nullptr , sequence is the 1st token to be pushed
        }
    } else {
        // toBePushedTp is an operator
        auto curOpType = toBePushed->getOperatorType();
        TokenBase* previousValidToken   = pr.first;
        TokenBase* previousClosestToken = pr.second;
        if ( previousValidToken == nullptr ) {
            previousToken = previousValidToken;
            avaliable = false;
        } else {
            // previousValidToken != nullptr
            auto preTp = previousValidToken->getTokenType();
            if ( preTp == E_TOKEN_SEMICOLON ) {
                previousToken = previousValidToken;
                if (      curOpType == E_OPEN_PARENTHESES 
                     ||  (curOpType == E_ADD   || curOpType == E_POSITIVE) 
                     ||  (curOpType == E_MINUS || curOpType == E_NEGATIVE)  )
                {
                    avaliable = true; // ;(
                } else {
                    avaliable = false;
                }
            } else if ( preTp == E_TOKEN_EXPRESSION ) {
                previousToken = previousValidToken;
                if ( curOpType == E_BIT_NOT || curOpType == E_OPEN_PARENTHESES ) {
                    // e.g.     idx~  or   idx(    is  invalid
                    avaliable = false;
                } else { // e.g.  idx+ , idx- ... , idx) , idx=
                    avaliable = true;
                }
            } else {
                // previous is an operator too ,     leftOp   rightOp
                auto foundMatched = false;
                auto closeFlag = false;
                auto sepFlag   = false;
                for( int idx = 0; idx < TokenMgr::s_TABLE_SIZE; ++idx )
                {
                    if(     TokenMgr::s_OpPairCfgTable[idx].left  == previousValidToken->getOperatorType()  
                        &&  TokenMgr::s_OpPairCfgTable[idx].right == curOpType ) 
                    {
                        closeFlag = TokenMgr::s_OpPairCfgTable[idx].closeAvaliable;
                        sepFlag   = TokenMgr::s_OpPairCfgTable[idx].seperateAvaliable;
                        foundMatched = true;
                        break;
                    }
                }

                previousToken = previousValidToken;
                if ( foundMatched ) {
                    if ( closeFlag && sepFlag ) {
                        avaliable = true;
                    } else if ( !closeFlag && !sepFlag ) {
                        avaliable = false;
                    } else {
                        if ( !closeFlag && sepFlag ) {
                            if ( previousClosestToken == previousValidToken  ) {
                                avaliable = false;
                            } else {
                                avaliable = true;
                            }
                        } else {
                            // never execute code to here
                        }
                    }
                } else {
                    avaliable = false;
                }
            }
        }
    }

    return make_pair(avaliable, previousToken);
}





// first  : skip space && comment , valid one
// second : the closest one
pair<TokenBase*,TokenBase*> TokenMgr::getPreviousToken()
{
    if ( m_allTokenList.empty() ) {
        return make_pair(nullptr,nullptr);
    }

    // closest

    auto hasbeenSet = false;
    TokenBase* pTheClosestToken = nullptr;
    TokenBase* pNoneBlankCommentToken = nullptr;
    for( auto rit = m_allTokenList.rbegin(); rit != m_allTokenList.rend(); ++rit )
    {
        auto pToken = *rit;
        if ( !hasbeenSet ) {
            pTheClosestToken = pToken;
            hasbeenSet = true;
        }

        if ( pToken!=nullptr  &&  (!TokenMgr::isIgnoredType(  pToken->getTokenType() ) ) ) {
            pNoneBlankCommentToken = pToken;
            break;
        }
    }

    return make_pair(pNoneBlankCommentToken, pTheClosestToken);
}













//####################################################################################################
//
//      Binary Op
TokenBase* TokenMgr::doAdd(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_ADD + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    E_DataType retDt = leftVal.type;
    DataValue sumRet = leftVal + rightVal;
    traceTmpOpResult(finalExpr, sumRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( sumRet );

    return ret;
}

TokenBase* TokenMgr::doMinus(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_MINUS + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    E_DataType retDt = leftVal.type;
    DataValue substractRet = leftVal - rightVal;
    traceTmpOpResult(finalExpr, substractRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setBeginPos( left->getBeginPos() );
    ret->setEndPos(   right->getBeginPos() );
    ret->setRealValue( substractRet );

    return ret;
}

TokenBase* TokenMgr::doMultiply(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_MULTIPLY  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    E_DataType retDt = leftVal.type;
    DataValue multiRet = leftVal * rightVal;
    traceTmpOpResult(finalExpr, multiRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( multiRet  );

    return ret;
}

TokenBase* TokenMgr::doDivide(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_DIVIDE  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);


    E_DataType retDt = leftVal.type;

    // calc 
    DataValue divideRet;
    try {
        divideRet  = (leftVal / rightVal);
    } catch ( MyException& e ) {
        // auto detailstr = e.getDetail();
        // if ( detailstr.empty() ) {
        //     e.setDetail( rightContent + " is zero Value ( in Divide ) @"  +  right->getBeginPos().getPos() );
        // } 
        e.setDetail( rightContent + " is zero Value ( in Divide ) @"  +  right->getBeginPos().getPos(0) );
        throw;
    }
    traceTmpOpResult(finalExpr, divideRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( divideRet );

    return ret;
}

TokenBase* TokenMgr::doMod(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_MOD  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_MODULO_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_MODULO_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }


    E_DataType retDt = leftVal.type;

    // calc 
    DataValue modRet;
    try {
        modRet  = (leftVal % rightVal);
    } catch ( MyException& e ) {
        if ( e.getDetail().empty() ) {
            e.setDetail( rightContent + " is zero Value ( in Modulo ) @" + right->getBeginPos().getPos(0) );
        }
        throw;
    }

    traceTmpOpResult(finalExpr, modRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( modRet );

    return ret;
}

TokenBase* TokenMgr::doBitAnd(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_AND  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITAND_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITAND_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    E_DataType retDt = leftVal.type;

    // calc 
    DataValue bitAndRet = (leftVal & rightVal);
    traceTmpOpResult(finalExpr, bitAndRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( bitAndRet );

    return ret;

}

TokenBase* TokenMgr::doBitOr(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_OR  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITOR_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITOR_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }


    E_DataType retDt = leftVal.type;

    // calc 
    DataValue bitOrRet = (leftVal | rightVal);
    traceTmpOpResult(finalExpr, bitOrRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( bitOrRet );

    return ret;
}

TokenBase* TokenMgr::doBitXor(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_XOR  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITXOR_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITXOR_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    

    E_DataType retDt = leftVal.type;

    // calc 
    DataValue bitXOrRet = (leftVal ^ rightVal);
    traceTmpOpResult(finalExpr, bitXOrRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( bitXOrRet );

    return ret;
}

TokenBase* TokenMgr::doBitLeftShift(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_LEFT_SHIFT  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();

    E_DataType leftPromotionDt = operatorPrepairDataTypeConversion2(&leftVal, &rightVal);


    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_LEFTSHIFT_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_LEFTSHIFT_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    tracebitShiftWarning(true, left, right);


    // E_DataType retDt = leftVal->type;

    // calc 
    DataValue bitLeftShiftRet = (leftVal << rightVal);
    bitLeftShiftRet.downerCast( leftPromotionDt );

    traceTmpOpResult(finalExpr, bitLeftShiftRet);

    TokenBase* ret = generateTmpExpression(leftPromotionDt, finalExpr, left, right);
    ret->setRealValue( bitLeftShiftRet );

    return ret;
}

TokenBase* TokenMgr::doBitRightShift(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_RIGHT_SHIFT   + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();

    E_DataType leftPromotionDt = operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_RIGHTSHIFT_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_RIGHTSHIFT_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    tracebitShiftWarning(false, left, right);
    

    // calc 
    DataValue bitRightShiftRet = (leftVal >> rightVal);
    bitRightShiftRet.downerCast( leftPromotionDt );
    traceTmpOpResult(finalExpr, bitRightShiftRet);

    TokenBase* ret = generateTmpExpression(leftPromotionDt, finalExpr, left, right);
    ret->setRealValue( bitRightShiftRet );

    return ret;
}

TokenBase* TokenMgr::doAssignment(TokenBase* left, TokenBase* right)
{
    using namespace charutil;

    VaribleInfo* toBeAssignmentVar = nullptr;
    auto content = left->getTokenContent();
    if (    !( left->isVarible() ) 
        ||  ( ( toBeAssignmentVar = VariblePool::getPool()->getVaribleByName(content)) == nullptr )  ) {
        MyException e(E_THROW_VARIBLE_NOT_DEFINED, left->getBeginPos() );
        e.setDetail( content );
        throw e;
    } 


    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_ASSIGNMENT + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();

    //
    // Core Core Core :
    // Can't use  assignment : Because the left operand and the right operand has different data type
    //
    //    leftVal = rightVal;
    //
    leftVal.doAssignment(rightVal);
    left->setRealValue( leftVal );

    // update varible' value
    toBeAssignmentVar->dataVal.doAssignment( leftVal );
    toBeAssignmentVar->isInitialed = true; // set as initialed

    traceTmpOpResult(finalExpr, leftVal);

    TokenBase* ret = generateTmpExpression( leftVal.type , finalExpr, left, right);
    ret->setRealValue( leftVal );

    return ret;
}


//####################################################################################################
//
//      Unary Op
TokenBase* TokenMgr::doPositive(TokenBase* op, TokenBase* right)
{
    using namespace charutil;

    string rightContent = right->getExpressionContent();
    string finalExpr = SC_OP_POSITIVE_BEGIN + rightContent;

    DataValue rightVal = right->getRealValue();
    E_DataType rightPromotionDt = operatorPrepairDataTypeConversion1(&rightVal);

    // calc 
    DataValue positiveRet = +rightVal;
    traceTmpOpResult(finalExpr, positiveRet);

    TokenBase* ret = generateTmpExpression(rightPromotionDt, finalExpr, op, right);
    ret->setRealValue( positiveRet );

    return ret;
}

TokenBase* TokenMgr::doNegative(TokenBase* op, TokenBase* right)
{
    using namespace charutil;

    string rightContent = right->getExpressionContent();
    string finalExpr = SC_OP_NEGATIVE_BEGIN + rightContent;

    DataValue rightVal = right->getRealValue(); 
    traceNegativeOperation(right);

    E_DataType rightPromotionDt = operatorPrepairDataTypeConversion1(&rightVal);

    // calc 
    DataValue negativeRet = -(rightVal);
    traceTmpOpResult(finalExpr, negativeRet);

    TokenBase* ret = generateTmpExpression(rightPromotionDt, finalExpr, op, right);
    ret->setRealValue( negativeRet  );

    return ret;

}

TokenBase* TokenMgr::doBitNot(TokenBase* op, TokenBase* right)
{
    using namespace charutil;

    string rightContent = right->getExpressionContent();
    string finalExpr = SC_OP_BIT_NOT_BEGIN + rightContent;

    DataValue rightVal = right->getRealValue();
    E_DataType rightPromotionDt = operatorPrepairDataTypeConversion1(&rightVal);

    if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITNOT_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    } 


    // calc 
    DataValue bitNotRet   =    ~rightVal;
    traceTmpOpResult(finalExpr, bitNotRet);

    TokenBase* ret = generateTmpExpression(rightPromotionDt, finalExpr, op, right);
    ret->setRealValue( bitNotRet );

    return ret;
}


// static 
TokenBase* TokenMgr::generateTmpExpression(E_DataType dt, const string& expression, TokenBase* begtoken, TokenBase* endtoken)
{
    TokenBase* pTmpExpression = new TokenBase(dt);
    pTmpExpression->setAsTmpExpression();
    pTmpExpression->setTokenContent( expression );
    pTmpExpression->setBeginPos( begtoken->getBeginPos() );
    pTmpExpression->setEndPos( endtoken->getEndPos() );

    s_generatedTmpTokenPool.push_back( pTmpExpression );
    return pTmpExpression;
}


// static 
void TokenMgr::clearTmpGenTokenPool()
{
    for( auto it = s_generatedTmpTokenPool.begin(); it != s_generatedTmpTokenPool.end(); ++it )
    {
        if ( *it != nullptr ) {
            delete *it;
            *it = nullptr;
        }
    }
    s_generatedTmpTokenPool.clear();
}


E_DataType TokenMgr::operatorPrepairDataTypeConversion1(DataValue* pRightVal)
{
    auto retType = pRightVal->type;
    auto rightpr = DataTypeUtil::needDoIntegerPromotion( pRightVal->type );
    if ( rightpr.first ) {
        pRightVal->doIntergerPromotion( rightpr.second );
        // set new data Type after finishing interger Promotion 
        retType = rightpr.second;
    } 


    return retType;

}


E_DataType TokenMgr::operatorPrepairDataTypeConversion2(DataValue* pLeftVal, DataValue* pRightVal)
{
    auto setFlag = false;
    auto retType = pLeftVal->type;
    auto leftpr = DataTypeUtil::needDoIntegerPromotion( pLeftVal->type );
    if ( leftpr.first ) {
        pLeftVal->doIntergerPromotion( leftpr.second );
        // set new data Type after finishing interger Promotion 
        setFlag = true;
        retType = leftpr.second;
    }

    auto rightpr = DataTypeUtil::needDoIntegerPromotion( pRightVal->type );
    if ( rightpr.first ) {
        pRightVal->doIntergerPromotion( rightpr.second );
    }

    auto cvtPrpr = DataTypeUtil::needDoConvertion( pLeftVal->type, pRightVal->type );
    auto retDt = cvtPrpr.first.second;
    if ( cvtPrpr.first.first ) {
        pLeftVal->doConvertion( retDt );
    }

    if ( cvtPrpr.second.first ) {
        pRightVal->doConvertion( retDt );
    }

    if ( setFlag ) {
        retType = retDt;
    }

    return retType;

}

void TokenMgr::printOperatorStack()
{
    if ( CmdOptions::needPrintOperatorStackAll()  ) {
        if ( m_opertorStack.empty() ) {
            cerr << m_execodeIdx << ". Operator Stack  : <Empty> " << endl;
        } else {
            cerr << m_execodeIdx <<". Operator Stack  : " <<  m_opertorStack.size() << " Element(s)" << endl;

            auto idx = 0;
            auto it = m_opertorStack.begin();
            while ( it != m_opertorStack.end() ) {
                auto pToken = *it;
                auto content = pToken->getTokenContent();
                cerr << idx << ". " << content << endl;

                ++it;
                ++idx;
            }
            cerr << endl;
        }
    }
}




void TokenMgr::printSuffixExpression(int tag)
{
    auto printFlag = false;
    if ( tag == 1 ) {
        printFlag = CmdOptions::needPrintSuffixExpressionBefore();
    } else if ( tag == 2 )  {
        printFlag = CmdOptions::needPrintSuffixExpressionAfterBuild();
    } else if ( tag == 3 ) {
        printFlag = CmdOptions::needPrintSuffixExpressionAfterEvaluate();
    }

    if ( printFlag ) {
        if ( m_suffixExpression.empty() ) {
            cerr << m_execodeIdx << "-" << tag << " . Suffix Expression List  : <Empty> " << endl;
        } else {
            cerr << m_execodeIdx << "-" << tag << ". Suffix Expression List  : " <<  m_suffixExpression.size() << " Element(s)" << endl;

            auto idx = 0;
            auto it = m_suffixExpression.begin();
            while ( it != m_suffixExpression.end() ) {
                auto pToken = *it;
                auto content = pToken->getTokenContent();
                cerr << idx << ". " << content << endl;

                ++it;
                ++idx;
            }
            cerr << endl;
        }

    }


}



void TokenMgr::popAllOperatorStack()
{

    for( auto rit = m_opertorStack.rbegin(); rit != m_opertorStack.rend(); ++rit )
    {
        auto pToken = *rit;

        traceOperatorStack(pToken, false);

        m_suffixExpression.push_back( pToken );
        traceSuffixExpression(pToken, true);
    }
    m_opertorStack.clear();
}




void TokenMgr::traceOperatorStack(TokenBase* pToken, bool push)
{
    if ( CmdOptions::needTraceOperatorStackChange()  ) {
        if ( push ) {
            cerr << "OpStack->Push \"" <<  pToken->getTokenContent() << "\"" << endl;
        } else {
            cerr << "OpStack->Pop \""  <<  pToken->getTokenContent() << "\"" << endl;
        }

    }

}

void TokenMgr::traceSuffixExpression(TokenBase* pToken, bool push)
{
    if ( CmdOptions::needTraceSuffixExpressionChange()  ) {
        if ( push ) {
            cerr << "SuffixExpr->Push \"" <<  pToken->getTokenContent() << "\"" << endl;
        } else {
            cerr << "SuffixExpr->Pop \""  <<  pToken->getTokenContent() << "\"" << endl;
        }

    }
}


void TokenMgr::tracePositiveNegativeFlag(TokenBase* pToken, E_OperatorType op)
{
    if ( CmdOptions::needTracePositiveNegativePropertyChange() ) {
        auto isAdd = (op == E_ADD);
        if ( pToken == nullptr ) {
            cerr << "chOp : nullptr , so " << (isAdd ? " add -> positive" : " minus -> negative ") << endl;
        } else {
            auto content = pToken->getTokenContent();
            cerr << "ChOp :  \"" << content << "\" , so  " << (isAdd ? " add -> positive" : " minus -> negative ") << endl;
        }
    }

}



void TokenMgr::tracePushedTokenWarning(TokenBase* pToken)
{
    using namespace charutil;

    auto strWarning = pToken->getWarningContent();
    if (  CmdOptions::needPrintParseRuntimeWarning()    &&   !strWarning.empty() ) {
        auto tp = pToken->getTokenType();
        cerr << SC_WARNING_TITLE;
        if ( !(tp == E_TOKEN_SINGLE_LINE_COMMENT || tp == E_TOKEN_MULTI_LINE_COMMENT ) ) {
            auto content = pToken->getTokenContent();
            cerr << "\"" << content << "\"" << endl;
        }

        cerr << strWarning 
             << endl
             << endl;
    }
}


void TokenMgr::tracebitShiftWarning(bool isLeftBitShift, TokenBase* left,  TokenBase* right)
{
    using namespace charutil;

    // Core Core Core : config this flag
    static const bool SC_B_SHOULD_ALWAYS_CHECK_RIGHT_VALUE = true;

    if (  CmdOptions::needPrintParseRuntimeWarning()  ) {
        auto hasprintFlag = false;
        auto hasLeftWarningFlag = false;
        auto hasRightWarningFlag = false;

        TypeBaseInfo leftTpInfo(  left->getRealValue().type );
        TypeBaseInfo rightTpInfo( right->getRealValue().type );
        DataValue rightVal = right->getRealValue();

        auto leftusFlag = leftTpInfo.isUnsignedType();
        if ( CmdOptions::needTreatSignedIntergerBitShiftAsWarning() &&  left->isVarible() &&  !leftusFlag ) {
            cerr << SC_WARNING_TITLE;
            cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
            cerr << "leftOperand is not unsigned type : "  << EnumUtil::enumName(leftTpInfo.getType()) << " : " << left->getTokenContent() << SPACE_1 << "@" << left->getBeginPos().getPos(0) << endl;
            hasprintFlag = true;
            hasLeftWarningFlag = true;
        }

        auto checkRightFlag =  SC_B_SHOULD_ALWAYS_CHECK_RIGHT_VALUE || right->isVarible();

        if ( !checkRightFlag )  {
            return;
        }

        int leftallbits = leftTpInfo.getBits();
        auto rightusFlag = rightTpInfo.isUnsignedType();
        if ( !rightusFlag ) {
            // with  +/-
            if ( rightVal.isNegative() ) {
                if ( !hasprintFlag ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
                    hasprintFlag = true;
                }

                cerr << "rightOperand's value <= 0 , type = "  << EnumUtil::enumName(rightTpInfo.getType()) << " : " << right->getTokenContent() << " = " << rightVal.getPrintValue(0) << endl;
                hasRightWarningFlag = true;
            } else {
                // >=0
                if ( rightVal.isGreaterEqualBitsWidth(leftallbits) ) {
                    if ( !hasprintFlag ) {
                        cerr << SC_WARNING_TITLE;
                        cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
                        hasprintFlag = true;
                    }

                    cerr << "rightOperand's value >= "  << leftallbits << " (Bits-Width) , " << right->getTokenContent() << " = " << rightVal.getPrintValue(0) << endl;
                    hasRightWarningFlag = true;
                }
            }
        } else {
            // >=0
            if ( rightVal.isGreaterEqualBitsWidth(leftallbits) ) {
                if ( !hasprintFlag ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
                    hasprintFlag = true;
                }

                cerr << "rightOperand's value > "  << leftallbits << " (Bits-Width) , " << right->getTokenContent() << " = " << rightVal.getPrintValue(0) << endl;
                hasRightWarningFlag = true;
            }
        }


        if ( hasLeftWarningFlag || hasRightWarningFlag ) {
            cerr << endl;
        }
    }
}


void TokenMgr::traceNegativeOperation(TokenBase* right)
{
    using namespace charutil;

    if ( CmdOptions::needPrintParseRuntimeWarning()  ) {
        if ( right->isVarible()  ) {
            DataValue rightVal = right->getRealValue();

            TypeBaseInfo tpInfo( rightVal.type );
            if ( tpInfo.isIntegerFamily() ) {

                if ( tpInfo.isUnsignedType() ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " operator - (negative) on a unsigned varible \""<< right->getTokenContent() << "\"  doesn't take effect on a number whose value is always >= 0 " << endl;
                } else if ( rightVal.isMinimumNegativeNumber() ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " operator - (negative) on signed varible \""<< right->getTokenContent() << "\"  doesn't take effect on a number ( minimum negative number ) to making a result as a +number > 0" << endl;
                }
            }
        }
    }

}


void TokenMgr::traceTmpOpResult(const std::string& expr, DataValue& retValue)
{
    if ( CmdOptions::needTraceTmpExpressionProcess() ) {
        TypeBaseInfo retTpInfo(retValue.type);
        cerr << "[INFO] : " << expr << " => " << EnumUtil::enumName( retTpInfo.getType() ) << " , value = " << retValue.getPrintValue(0) << endl;
    }
}





void TokenMgr::traceUnInitializedVarWhenUsed(TokenBase* pToken)
{

    using namespace charutil;

    if ( CmdOptions::needPrintParseRuntimeWarning() ) {
        cerr << SC_WARNING_TITLE;
        cerr << " varible named " << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << " is not initialized before used " << endl;
    }

}



void TokenMgr::traceBlankStatement()
{
    using namespace charutil;

    if ( CmdOptions::needPrintParseRuntimeWarning() &&  CmdOptions::needTreatBlankStatementAsWarning()  ) {
        cerr << SC_WARNING_TITLE;
        cerr << " Blank Statement is skipped ! . such as    ; ; " << endl;
    }

}



