#include "sequenceParser.h"
#include "charUtil.h"
#include "myException.h"
#include "commonEnum.h"
#include "keywordList.h"
#include "parserOption.h"
#include "stringNumber.h"
#include <cassert>
using namespace std;

// for float chacacter  limit
/*static*/ const int   SequenceParser::S_FLOAT_DOT_UPPERLIMIT      = 1;  // 1 .
/*static*/ const int   SequenceParser::S_FLOAT_eE_UPPERLIMIT       = 1;  // 1 e/E
//static   const int   SequenceParser::S_FLOAT_POSITIVE_UPPERLIMIT = 1;  // 1 '+'    */
//static   const int   SequenceParser::S_FLOAT_NEGATIVE_UPPERLIMIT = 1;  // 1 '-'    */
/*static*/ const int   SequenceParser::S_FLOAT_fF_lL_SUFFIX_UPPERLIMIT = 1; // 1  f/F


SequenceParser::SequenceParser()
    : ParserBase()
    , m_guessType(SequenceParser::E_GUESS_UNKNOWN)
    /////////////////////////////////////////////////////
    , m_numberCnt(0)
    /////////////////////////////////////////////////////
    , m_float_dotCnt(0)
    , m_float_eECnt(0)
    , m_float_ch_eE(0)
    , m_float_positiveSignCnt(0)
    , m_float_negativeSignCnt(0)
    , m_float_fFSuffixCnt(0)
    , m_float_lLSuffixCnt(0)
    , m_float_reachSuffix_fF_lL_flag(false)
    /////////////////////////////////////////////////////
    , m_oct_broken_flag(false)
    , m_int_suffix_string("")
{
    m_type = E_PARSER_TYPE::E_SEQUENCE;
    m_tokenType = E_TOKEN_EXPRESSION;
}
    
// virtual 
SequenceParser::~SequenceParser()
{
}



// virtual 
ParserBase::E_PARSER_TYPE SequenceParser::appendChar(const ChInfo& rChInfo,  E_ParserAction& afterAction) // override;
{
    using namespace charutil;
    // m_endChPos = rChInfo;

    auto retType = m_type;

    switch( m_guessType )
    {
    case E_GUESS_DEC:
        retType = appendForType_int(rChInfo,afterAction);
        break;
    case E_GUESS_OCT:
        retType = appendForType_oct(rChInfo,afterAction);
        break;
    case E_GUESS_HEX:
        retType = appendForType_hex(rChInfo,afterAction);
        break;
    case E_GUESS_FLOAT:
        retType = appendForType_float(rChInfo,afterAction);
        break;
    case E_GUESS_VARIBLE:
        retType = appendForType_varible(rChInfo,afterAction);
        break;
    default:
        break;
    }

    if ( m_type == retType ) {
        m_endChPos = rChInfo;
    }

    return retType;
}


//virtual 
void SequenceParser::pushHeadChar(char ch, const ChInfo& rChInfo) // override
{
    using namespace charutil;

    //  push the 1st character
    ParserBase::pushHeadChar(ch, rChInfo);

    if ( isVaribleChar1st(ch) ) {
        m_guessType = E_GUESS_VARIBLE;
    } else if ( isNumber(ch) ) {
        m_guessType = E_GUESS_DEC;
        ++m_numberCnt;
    } else if ( isDot(ch) ) {
        m_guessType = E_GUESS_FLOAT;
        m_float_dotCnt = 1;
    }
}








// virtual 
void SequenceParser::resetInternalState() // override; 
{
    m_parsedSeq = "";

    m_guessType = SequenceParser::E_GUESS_UNKNOWN;


    // number count
    m_numberCnt = 0;

    ///////////////////////////////////////////////////
    // float part
    m_float_dotCnt = 0;
    m_float_eECnt= 0;
    m_float_ch_eE = 0;
    m_float_positiveSignCnt = 0;
    m_float_negativeSignCnt = 0;
    m_float_fFSuffixCnt = 0;
    m_float_lLSuffixCnt = 0;
    m_float_reachSuffix_fF_lL_flag = false;

    ///////////////////////////////////////////////////
    // int part
    m_oct_broken_flag = false;
    m_int_suffix_string = "";

    m_warningCnt = 0;
    m_warningContent = "";
}





ParserBase::E_PARSER_TYPE SequenceParser::appendForType_int(const ChInfo& rChInfo, E_ParserAction& afterAction)
{
    using namespace charutil;

    char ch = rChInfo.ch;

    string throwSeq = m_parsedSeq;
    ParserBase::E_PARSER_TYPE retType = m_type;
    auto tp = getCharType(ch);

    if ( isUnderline(ch) ) {
        throw_Int_exception(rChInfo, (throwSeq + ch));
    }

    if ( tp == E_CHAR_BLANK || tp == E_CHAR_OPERATOR || tp == E_CHAR_UNKNOWN ) {
        retType = ParserBase::E_BASE;
        afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        return retType;
    }


    int  sz = static_cast<int>( m_parsedSeq.size() );
    if ( sz == 1 ) {
        char ch1st = m_parsedSeq.at(0);
        auto is1stZero = (ch1st == '0');

        switch( tp )
        {
        case E_CHAR_ALPHA:
            {
                // Special Letter :   e/E    f/F     l/L   u/U   x/X
                auto needThrow = false;
                if ( is_xX(ch) ) {
                    if ( is1stZero ) {
                        m_guessType = E_GUESS_HEX; // 0x  or  0X
                    } else {
                        needThrow = true; // [1-9][xX] is not allowed
                    }
                } else if ( is_eE(ch) ) {
                    m_guessType = E_GUESS_FLOAT; // [0-9][eE]
                    increase_eE_CntAndCheck(rChInfo);
                } else if ( is_fF(ch) ) {
                    needThrow = true; // [0-9][fF] is not allowed
                } else if ( is_uU(ch) || is_lL(ch) ) {
                    m_int_suffix_string += ch; // [0-9]u   or   [0-9]l
                } else {
                    needThrow = true;
                }


                if ( needThrow ) {
                    throw_Int_exception(rChInfo, (throwSeq + ch));
                } else {
                    m_parsedSeq += ch;
                }
            }
            break;
        case E_CHAR_NUMBER:
            {
                if ( is1stZero ) {
                    // treat as Oct  temporary
                    m_guessType = E_GUESS_OCT;
                    if ( !isOctCode(ch) ) {
                        m_oct_broken_flag = true;
                    } 
                }

                // else [1-9][0-9] is valid
                m_parsedSeq += ch;
                ++m_numberCnt;
            }
            break;
        case E_CHAR_MISC:
            {
                if ( isDot(ch) ) {
                    m_guessType = E_GUESS_FLOAT; // [0-9].
                    increase_dot_CntAndCheck(rChInfo);
                    m_parsedSeq += ch;
                } else { // ;
                    retType = ParserBase::E_BASE; // [0-9];
                    afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                } 
            }
            break;
        // case E_CHAR_UNKNOWN:
        //     retType = ParserBase::E_BASE;
        //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        //     break;
        default:
            break;
        }

    } else {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // sz >= 2
        //
        switch ( tp ) 
        {
        case E_CHAR_ALPHA:
            {
                // Special Letter :   e/E    f/F     l/L   u/U   x/X
                auto needThrow = false;
                if ( is_eE(ch) ) {
                    if ( m_int_suffix_string.empty() ) {
                        //  [1-9]+  [eE] 
                        m_guessType = E_GUESS_FLOAT; // e.g.  123e
                        increase_eE_CntAndCheck(rChInfo);
                    } else {
                        needThrow = true; // e.g.   123ue or 123le is not valid
                    }
                } else if ( is_fF(ch) ) {
                    // [0-9][uU][lL]+  [fF] , as a float type , 
                    //      it must contain  '.'/'e|E'                  e.g.   3.14    3e2
                    //              or both  '.' && 'eE'                e.g.   3.14e2
                    needThrow = true;
                } else if ( is_uU(ch) || is_lL(ch) ) {
                    m_int_suffix_string += ch;
                    auto isValid = isIntSuffixValid();
                    if ( !isValid ) {
                        needThrow = true;
                    }
                } else {
                    needThrow = true; // Other alpha
                }


                if ( needThrow ) {
                    throw_Int_exception(rChInfo, (throwSeq + ch));
                } else {
                    m_parsedSeq += ch;
                }
            }
            break;
        case E_CHAR_NUMBER:
            {
                if ( !m_int_suffix_string.empty() ) {
                    throw_Int_exception( rChInfo, (throwSeq + ch));
                } else {
                    m_parsedSeq += ch;
                    ++m_numberCnt;
                }
            }
            break;
        // case E_CHAR_OPERATOR:
        // case E_CHAR_BLANK:
        //     //       [0-9]+
        //     //       [0-9]<Space>
        //     retType = ParserBase::E_BASE;
        //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        //     break;
        case E_CHAR_MISC:
            {
                if ( isDot(ch) ) {
                    if ( m_int_suffix_string.empty()  ) {
                        m_guessType = E_GUESS_FLOAT; // [0-9]+.
                        m_parsedSeq += ch;
                        increase_dot_CntAndCheck(rChInfo);
                    } else {
                        throw_Int_exception(rChInfo, (throwSeq + ch));
                    }
                } else { // ;
                    // [0-9]+[ul];
                    retType = ParserBase::E_BASE;
                    afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                } 
            }
            break;
        // case E_CHAR_UNKNOWN:
        //     retType = ParserBase::E_BASE;
        //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        //     break;
        default:
            break;
        }

    }

    return retType;
}

ParserBase::E_PARSER_TYPE SequenceParser::SequenceParser::appendForType_oct(const ChInfo& rChInfo, E_ParserAction& afterAction)
{
    using namespace charutil;

    ParserBase::E_PARSER_TYPE retType = m_type;

    string throwSeq = m_parsedSeq;
    char ch = rChInfo.ch;
    auto tp = getCharType(ch);

    if ( isUnderline(ch) ) {
        throw_Oct_exception(rChInfo, (throwSeq + ch));
    }


    if ( tp == E_CHAR_BLANK || tp == E_CHAR_OPERATOR || tp == E_CHAR_UNKNOWN  ) {
        retType = ParserBase::E_BASE;
        afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        return retType;
    }
    

    if ( isOctCode(ch) ) {
        // 0 ~ 7
        if ( m_int_suffix_string.empty() ) {
            m_parsedSeq += ch;
            ++m_numberCnt;
        } else {
            throw_Oct_exception(rChInfo, (throwSeq + ch));
        }
    } else {
        switch( tp )
        {
        case E_CHAR_ALPHA:
            {
                //
                // Special Letter :   e/E    f/F     l/L   u/U   x/X
                //
                auto throwFlag = false;
                if ( is_uU(ch) || is_lL(ch) ) {
                    m_int_suffix_string += ch;
                    auto isValid = isIntSuffixValid();
                    if ( !isValid ) {
                        throwFlag = true;
                    }
                } else if ( is_eE(ch) ) {
                    if ( !m_int_suffix_string.empty() ) {
                        throwFlag = true;
                    } else {
                        m_guessType = E_GUESS_FLOAT;
                        increase_eE_CntAndCheck(rChInfo);
                    }
                } else {
                    throwFlag = true;
                }

                if ( throwFlag ) {
                    throw_Oct_exception(rChInfo, (throwSeq + ch));
                } else {
                    m_parsedSeq += ch;
                }

            }
            break;
        case E_CHAR_NUMBER:
            {
                // the number is either 8 or 9
                // 0123.3  or  0123e4  is valid
                if ( m_int_suffix_string.empty() ) {
                    m_oct_broken_flag = true;
                    m_parsedSeq += ch;
                    ++m_numberCnt;
                } else {
                    throw_Oct_exception(rChInfo, (throwSeq + ch));
                }
            }
            break;
        // case E_CHAR_OPERATOR:
        // case E_CHAR_BLANK:
        //     //       [0-9]+
        //     //       [0-9]<Space>
        //     retType = ParserBase::E_BASE;
        //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        //     break;
        case E_CHAR_MISC:
            {
                if ( isDot(ch) ) {
                    if ( m_int_suffix_string.empty() ) {
                        m_guessType = E_GUESS_FLOAT;
                        increase_dot_CntAndCheck(rChInfo);
                    } else {
                        throw_Oct_exception(rChInfo, (throwSeq + ch));
                    }
                } else { // ;
                    // [0-9]+;
                    retType = ParserBase::E_BASE;
                    afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                } 
            }
            break;
        // case E_CHAR_UNKNOWN:
        //     retType = ParserBase::E_BASE;
        //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        //     break;
        default:
            break;
        }

    }

    return retType;
}

ParserBase::E_PARSER_TYPE SequenceParser::SequenceParser::appendForType_hex(const ChInfo& rChInfo, E_ParserAction& afterAction)
{
    using namespace charutil;

    ParserBase::E_PARSER_TYPE retType = m_type;

    string throwSeq = m_parsedSeq;
    char ch = rChInfo.ch;
    auto tp = getCharType(ch);

    if ( isUnderline(ch) ) {
        throw_Hex_exception(rChInfo, (throwSeq + ch));
    }

    if ( tp == E_CHAR_BLANK || tp == E_CHAR_OPERATOR || tp == E_CHAR_UNKNOWN  ) {
        retType = ParserBase::E_BASE;
        afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        return retType;
    }

    auto isOnly0xPrefix = (m_parsedSeq == "0x" || m_parsedSeq == "0X");

    if ( isHexCode(ch) ) {
        if ( m_int_suffix_string.empty() ) {
            m_parsedSeq += ch;
            if ( isNumber(ch) ) {
                ++m_numberCnt;
            }
        } else {
            throw_Hex_exception(rChInfo, (throwSeq + ch));
        }
    } else {

        switch( tp )
        {
        case E_CHAR_ALPHA:
            {
                // Special Letter :   only l/L or u/U is Valid
                auto throwFlag = false;
                if ( is_uU(ch) || is_lL(ch) ) {
                    if ( isOnly0xPrefix ) {
                        throwFlag = true;
                    } else {
                        m_int_suffix_string += ch;
                        auto isValid = isIntSuffixValid();
                        if ( !isValid ) {
                            throwFlag = true;
                        }
                    }
                } else {
                    throwFlag = true;
                }

                if ( throwFlag ) {
                    throw_Hex_exception(rChInfo, (throwSeq + ch));
                } else {
                    m_parsedSeq += ch;
                }
            }
            break;
        /////////////////////////////////////////////////////////////////////////
        // case E_CHAR_NUMBER:
        //     {
        //         // already checked in if ( isHexCode(ch) )
        //         break;
        //     }
        //     break;
        /////////////////////////////////////////////////////////////////////////
        // case E_CHAR_OPERATOR:
        // case E_CHAR_BLANK:
        //     //       [0-9]+
        //     //       [0-9]<Space>
        //     if ( isOnly0xPrefix ) {
        //         //   0x;   or  0x<Tab>
        //         throw_Hex_exception(rChInfo, (throwSeq + ch));
        //     } else {
        //         retType = ParserBase::E_BASE;
        //         afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        //     }
        //     break;
        case E_CHAR_MISC:
            {
                auto throwFlag = false;
                if ( isDot(ch) ) {
                    // 0x. or  0xA12B.
                    throwFlag = true;
                } else { // ; 
                    // 0x[hexcode]* ;
                    if ( isOnly0xPrefix ) {
                        throwFlag = true;
                    }
                    retType = ParserBase::E_BASE;
                    afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                } 

                if ( throwFlag ) {
                    throw_Hex_exception(rChInfo, (throwSeq + ch));
                }
            }
            break;
        // case E_CHAR_UNKNOWN:
        //     retType = ParserBase::E_BASE;
        //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        //     break;
        default:
            break;
        }

    }

    return retType;
}





ParserBase::E_PARSER_TYPE SequenceParser::appendForType_float(const ChInfo& rChInfo, E_ParserAction& afterAction)
{
    using namespace charutil;

    ParserBase::E_PARSER_TYPE retType = m_type;
    string throwSeq = m_parsedSeq;
    char ch = rChInfo.ch;
    auto tp = getCharType(ch);

    if ( isUnderline(ch) ) {
        throw_Float_exception(rChInfo, (throwSeq + ch));
    }

    auto lastCh = m_parsedSeq.back();
    if ( !is_eE(lastCh) ) {
        if ( tp == E_CHAR_BLANK || tp == E_CHAR_OPERATOR || tp == E_CHAR_UNKNOWN  ) {
            retType = ParserBase::E_BASE;
            afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
            return retType;
        }
    }



    if ( m_float_reachSuffix_fF_lL_flag ) {
        switch(tp)
        {
        case E_CHAR_ALPHA:
        case E_CHAR_NUMBER:
            throw_Float_exception(rChInfo, (throwSeq + ch));
            break;
        // case E_CHAR_OPERATOR:
        // case E_CHAR_BLANK:
        //     retType = ParserBase::E_BASE;
        //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        //     break;
        case E_CHAR_MISC:
            if ( isSemicolon(ch) ) { // ;
                retType = ParserBase::E_BASE;
                afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
            } else { //  '.'
                throw_Float_exception(rChInfo, (throwSeq + ch));
            }
            break;
        // case E_CHAR_UNKNOWN:
        //      retType = ParserBase::E_BASE;
        //      afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
        //     break;
        default:
            break;
        }

    } else {
        if ( isDot(lastCh) ) {
            // last is '.'
            switch(tp)
            {
            case E_CHAR_ALPHA:
                {
                    auto throwFlag = false;
                    if ( is_eE(ch) ) {
                        // e.g  12.e  or   12.E
                        if ( hasNumberBeforeOrAfterDot(true) ) {
                            increase_eE_CntAndCheck(rChInfo);
                        } else {
                            // ".e"   is not allowed
                            throwFlag = true;
                        }
                    } else if ( is_fF(ch) ) {
                        // .f , check the content before  . has number
                        increase_float_suffix_fF_CntAndCheck(rChInfo);
                        if ( !hasNumberBeforeOrAfterDot(true) ) {
                            throwFlag = true;
                        }
                    } else if ( is_lL(ch) ) {
                        // .l  check the content before  . has number
                        increase_float_suffix_lL_CntAndCheck(rChInfo);
                        if ( !hasNumberBeforeOrAfterDot(true) ) {
                            throwFlag = true;
                        }
                    } else {
                        throwFlag = true;
                    }


                    if ( throwFlag ) {
                        throw_Float_exception(rChInfo, (throwSeq + ch));
                    } else {
                        m_parsedSeq += ch;
                    }
                }
                break;
            case E_CHAR_NUMBER:
                m_parsedSeq += ch;
                ++m_numberCnt;
                break;
            // case E_CHAR_OPERATOR:
            // case E_CHAR_BLANK:
            //     retType = ParserBase::E_BASE;
            //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
            //     break;
            case E_CHAR_MISC:
                {
                    if ( isSemicolon(ch) ) { // ;
                        retType = ParserBase::E_BASE;
                        afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                    } else { // '.'
                        //  ".." must throw exception inside  increase_dot_CntAndCheck
                        m_parsedSeq += ch;
                        increase_dot_CntAndCheck(rChInfo);
                    }
                }
                break;
            // case E_CHAR_UNKNOWN:
            //     retType = ParserBase::E_BASE;
            //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
            //     break;
            default:
                break;
            }
            //
            // after process the last character is "."   statement
            //
        } else if ( is_eE(lastCh) ) {
            // last is 'e/E'
            auto switchThrowFlag = false;
            switch(tp)
            {
            case E_CHAR_NUMBER:
                m_parsedSeq += ch;
                ++m_numberCnt;
                break;
            case E_CHAR_OPERATOR:
                {
                    if ( ch == '+'  ) {
                        //  e.g. 1e+   or   1e-
                        m_float_positiveSignCnt = 1;
                        m_parsedSeq += ch;
                    } else if ( ch == '-' ) {
                        m_float_negativeSignCnt = 1;
                        m_parsedSeq += ch;
                    } else {
                        // switchThrowFlag = true;
                        // check logic inside     create  token
                        retType = ParserBase::E_BASE;
                        afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                    }
                }
                break;
            case E_CHAR_ALPHA:
            case E_CHAR_BLANK:
            case E_CHAR_MISC:
                switchThrowFlag = true;
                break;
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Don't Comment  E_CHAR_UNKNOWN Part
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            case E_CHAR_UNKNOWN:
                retType = ParserBase::E_BASE;
                afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                break;
            default:
                break;
            }

            if ( switchThrowFlag ) {
                throw_Float_exception(rChInfo, (throwSeq + ch));
            }
        } else {
            //
            // last is neither    '.'    nor    'e/E'
            //
            if ( m_float_eECnt == 1 ) {
                // e ...
                auto switchThrowFlag = false;
                switch(tp)
                {
                case E_CHAR_NUMBER:
                    m_parsedSeq += ch;
                    ++m_numberCnt;
                    break;
                case E_CHAR_ALPHA:
                    {
                        auto isCh_fF = false;
                        if ( (isCh_fF=is_fF(ch) ) || is_lL(ch) ) {
                            int numCnt = getNumberAfter_eE();
                            if ( numCnt > 0 ) {
                                if ( isCh_fF ) {
                                    increase_float_suffix_fF_CntAndCheck(rChInfo); // is f/F
                                } else {
                                    increase_float_suffix_lL_CntAndCheck(rChInfo); // is l/L
                                }
                                m_parsedSeq += ch;

                                m_float_reachSuffix_fF_lL_flag = true;
                                retType = ParserBase::E_BASE;
                                afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_STEP_NEXT;
                            } else {
                                switchThrowFlag = true; // u/U or x/X are neither allowed
                            }
                        } else {
                            switchThrowFlag = true;
                        }
                    }
                    break;
                // case E_CHAR_OPERATOR:
                // case E_CHAR_BLANK:
                //     retType = ParserBase::E_BASE;
                //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                //     break;
                case E_CHAR_MISC:
                    if ( isSemicolon(ch) ) {
                        retType = ParserBase::E_BASE;
                        afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                    } else {
                        // '.' can't be existed after  e/E
                        switchThrowFlag = true;
                    }
                    break;
                // case E_CHAR_UNKNOWN:
                //     retType = ParserBase::E_BASE;
                //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                //     break;
                default:
                    break;
                }   


                if ( switchThrowFlag ) {
                    throw_Float_exception(rChInfo,  (throwSeq + ch ));
                } 
            } else {
                // 0 e/E  , Only  '.'
                auto switchThrowFlag = false;
                switch(tp)
                {
                case E_CHAR_NUMBER:
                    m_parsedSeq += ch;
                    ++m_numberCnt;
                    break;
                case E_CHAR_ALPHA:
                    {
                        auto isCh_fF = false;
                        if ( is_eE(ch) ) {
                            m_parsedSeq += ch;
                            increase_eE_CntAndCheck(rChInfo);
                        } else if ( (isCh_fF=is_fF(ch) ) || is_lL(ch) ) {
                            if ( m_numberCnt > 0 ) {
                                if ( isCh_fF ) {
                                    increase_float_suffix_fF_CntAndCheck(rChInfo);
                                } else {
                                    increase_float_suffix_lL_CntAndCheck(rChInfo);
                                }

                                m_parsedSeq += ch;

                                m_float_reachSuffix_fF_lL_flag = true;
                                retType = ParserBase::E_BASE;
                                afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_STEP_NEXT;
                            } else {
                                switchThrowFlag = true;
                            }
                        } else {
                            switchThrowFlag = true;
                        }
                    }
                    break;
                // case E_CHAR_OPERATOR:
                // case E_CHAR_BLANK:
                //     retType = ParserBase::E_BASE;
                //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                //     break;
                case E_CHAR_MISC:
                    if ( isSemicolon(ch) ) {
                        retType = ParserBase::E_BASE;
                        afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                    } else {
                        // '.' // more than 1 '.' is not allowed
                        switchThrowFlag = true;
                    }
                    break;
                // case E_CHAR_UNKNOWN:
                //     retType = ParserBase::E_BASE;
                //     afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
                //     break;
                default:
                    break;
                }


                if ( switchThrowFlag ) {
                    throw_Float_exception(rChInfo, (throwSeq + ch ));
                }
            }

        }

    }

    return retType;
}




ParserBase::E_PARSER_TYPE SequenceParser::appendForType_varible(const ChInfo& rChInfo, E_ParserAction& afterAction)
{
    using namespace charutil;

    string throwSeq = m_parsedSeq;
    char ch = rChInfo.ch;

    ParserBase::E_PARSER_TYPE retType = m_type;
    if ( isVaribleChar(ch) ) {
        m_parsedSeq += ch;
    } else {
        auto tp = getCharType(ch);
        switch( tp )
        {
        case E_CHAR_OPERATOR:
        case E_CHAR_BLANK:
        case E_CHAR_UNKNOWN:
            retType = ParserBase::E_BASE;
            afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
            break;
        case E_CHAR_MISC:
            if ( isSemicolon(ch) ) {
                retType = ParserBase::E_BASE;
                afterAction = E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR;
            } else {
                // '.'   e.g.    obj.
                throw_Varible_exception( rChInfo, (throwSeq + ch));
            }
            break;
        default:
            break;
        }
    }

    return retType;
}


bool SequenceParser::isIntSuffixValid(int* pUCnt /* = nullptr*/, int* pLCnt /* = nullptr*/)
{
    static const SuffixInfo avaliableAry[] = {
        // 1 char
        { "u", 1, 0 },
        { "U", 1, 0 },
        { "l", 0, 1 },
        { "L", 0, 1 },

        //
        // 2 chars
        //
        { "ul", 1, 1 },
        { "uL", 1, 1 },
        { "Ul", 1, 1 },
        { "UL", 1, 1 },

        { "lu", 1, 1 },
        { "lU", 1, 1 },
        { "Lu", 1, 1 },
        { "LU", 1, 1 },

        { "ll", 0, 2 },
        { "LL", 0, 2 },

        //
        // 3 chars
        //
        { "ull", 1, 2 },
        { "uLL", 1, 2 },
        { "Ull", 1, 2 },
        { "ULL", 1, 2 },

        { "llu", 1, 2 },
        { "llU", 1, 2 },
        { "LLu", 1, 2 },
        { "LLu", 1, 2 }
    };

    if ( m_int_suffix_string.empty() ) {
        if ( pUCnt != nullptr ) { *pUCnt = 0; }
        if ( pLCnt != nullptr ) { *pLCnt = 0; }
        return false;
    }

    auto bInside = false;
    for( int idx = 0; idx < static_cast<int>( sizeof(avaliableAry)/sizeof(avaliableAry[0]) ); ++idx )
    {
        if ( m_int_suffix_string == avaliableAry[idx].suffix ) {
            if ( pUCnt != nullptr ) { *pUCnt = avaliableAry[idx].uUCnt; }
            if ( pUCnt != nullptr ) { *pLCnt = avaliableAry[idx].lLCnt; }

            bInside = true;
            break;
        }

    }

    return bInside;
}


void SequenceParser::increase_dot_CntAndCheck(const ChInfo& rChInfo)
{
    ++m_float_dotCnt;

    if ( m_float_dotCnt > SequenceParser::S_FLOAT_DOT_UPPERLIMIT ) {
        throw_Float_exception(rChInfo, (m_parsedSeq + rChInfo.ch) );
    }
}

void SequenceParser::increase_eE_CntAndCheck(const ChInfo& rChInfo)
{
    ++m_float_eECnt;
    m_float_ch_eE = rChInfo.ch;
    if ( m_float_eECnt  > SequenceParser::S_FLOAT_eE_UPPERLIMIT ) {
        throw_Float_exception(rChInfo, (m_parsedSeq + rChInfo.ch));
    }
}



void SequenceParser::increase_float_suffix_fF_CntAndCheck(const ChInfo& rChInfo)
{
    // e.g.  3.14f   ==> float
    if ( m_float_reachSuffix_fF_lL_flag ) {
        throw_Float_exception(rChInfo, (m_parsedSeq + rChInfo.ch));
    }

    ++m_float_fFSuffixCnt;
    m_float_reachSuffix_fF_lL_flag = true;
    if ( m_float_fFSuffixCnt > S_FLOAT_fF_lL_SUFFIX_UPPERLIMIT ) {
        throw_Float_exception(rChInfo, (m_parsedSeq + rChInfo.ch));
    }
}


void SequenceParser::increase_float_suffix_lL_CntAndCheck(const ChInfo& rChInfo)
{
    // does not support   long double    e.g.    3.14l is not allowed
    throw_Float_exception(rChInfo, (m_parsedSeq + rChInfo.ch));

    /*
    // e.g.   3.14l   ==>   long double type
    if ( m_float_reachSuffix_fF_lL_flag ) {
        throw_Float_exception(rChInfo, (m_parsedSeq + rChInfo.ch));
    }

    ++m_float_lLSuffixCnt;
    m_float_reachSuffix_fF_lL_flag = true;

    if ( m_float_lLSuffixCnt > S_FLOAT_fF_lL_SUFFIX_UPPERLIMIT ) {
        throw_Float_exception(rChInfo, (m_parsedSeq + rChInfo.ch));
    }
    */
}


bool SequenceParser::hasNumberBeforeOrAfterDot(bool beforeDot)
{
    using namespace charutil;

    string strPrefixBeforeDot;

    auto pos = m_parsedSeq.find('.');
    if ( pos == string::npos ) {
        return false;
    }

    auto foundNumber = false;
    if ( beforeDot ) {
        for( int i = 0; i < static_cast<int>(pos); ++i )
        {
            auto ch = m_parsedSeq.at(i);
            if ( isNumber(ch) ) {
                foundNumber = true;
                break;
            }

        }
    } else {
        //  after '.'
        int sz = static_cast<int>( m_parsedSeq.size() );
        for( int i = static_cast<int>(pos+1); i < sz; ++i )
        {
            auto ch = m_parsedSeq.at(i);
            if ( isNumber(ch) ) {
                foundNumber = true;
                break;
            }

        }
    }

    return foundNumber;
}

int  SequenceParser::getNumberAfter_eE()
{
    using namespace charutil;

    auto pos_e = m_parsedSeq.rfind('e');
    auto pos_E = m_parsedSeq.rfind('E');
    
    assert( pos_e != string::npos || pos_E != string::npos );
    int sz = static_cast<int>( m_parsedSeq.size() );
    int numberCnt = 0;

    int beg = static_cast<int>( (pos_e != string::npos ? pos_e : pos_E) + 1 );
    for( int i = beg; i < sz; ++i ) {
        if ( isNumber( m_parsedSeq.at(i) ) ) {
            ++numberCnt;
            break;
        }
    }

    return numberCnt;
}



bool SequenceParser::isOctValid()
{
    using namespace charutil;

    string stripSuffix = m_parsedSeq;
    if ( !m_int_suffix_string.empty() ) {
        int sz = static_cast<int>( m_parsedSeq.size() );
        stripSuffix = m_parsedSeq.substr(0, sz - m_int_suffix_string.size() );
    } 

    auto isAllOctCode = true;
    int stripSz = static_cast<int>( stripSuffix.size() );
    for( auto i = 0; i < stripSz; ++i ) {
        if ( !isOctCode( stripSuffix.at(i) ) ) {
            isAllOctCode = false;
            break;
        }
    }

    return isAllOctCode;
}

std::string SequenceParser::surroundDoubleQuoto(const std::string str)
{
    using namespace charutil;

    string retstr = DOUBLE_QUOTO + str + DOUBLE_QUOTO;
    return retstr;
}

void  SequenceParser::throw_Int_exception(const ChInfo& rChInfo, const string& detail)
{
    MyException e(E_THROW_INVALID_CHAR_IN_DECIMAL_SEQ, rChInfo);
    e.setDetail(detail);
    throw e;
}

void  SequenceParser::throw_Oct_exception(const ChInfo& rChInfo, const string& detail)
{
    MyException e(E_THROW_INVALID_CHAR_IN_OCTAL_SEQ, rChInfo);
    e.setDetail(detail);
    throw e;
}

void  SequenceParser::throw_Hex_exception(const ChInfo& rChInfo, const string& detail)
{
    MyException e(E_THROW_INVALID_CHAR_IN_HEX_SEQ, rChInfo);
    e.setDetail(detail);
    throw e;
}

void  SequenceParser::throw_Float_exception(const ChInfo& rChInfo, const string& detail)
{
    MyException e(E_THROW_INVALID_CHAR_IN_FLOAT_SEQ, rChInfo);
    e.setDetail( detail );
    throw e;
}

void  SequenceParser::throw_Varible_exception(const ChInfo& rChInfo, const string& detail)
{
    MyException e(E_THROW_INVALID_CHAR_IN_VARIBLE_SEQ,  rChInfo);
    e.setDetail(detail);
    throw e;
}

bool SequenceParser::isValid_Decimal()
{
    using namespace charutil;

    string parsedStr = m_parsedSeq;
    if ( !m_int_suffix_string.empty() ){
        int suffixLength = static_cast<int>( m_int_suffix_string.size() );
        int sz = static_cast<int>( parsedStr.size() );
        parsedStr = parsedStr.substr(0, sz - suffixLength);
    }

    auto isAllNumber = true;
    int parseLen = static_cast<int>( parsedStr.size() );
    for ( int i = 0; i < parseLen; ++i )
    {
        if ( !isNumber( parsedStr.at(i) ) ) {
            isAllNumber = false;
            break;
        }
    }

    return isAllNumber;
}

bool SequenceParser::isValid_Octal()
{
    using namespace charutil;

    string parsedStr = m_parsedSeq;
    if ( !m_int_suffix_string.empty() ){
        int suffixLength = static_cast<int>( m_int_suffix_string.size() );
        int sz = static_cast<int>( parsedStr.size() );
        parsedStr = parsedStr.substr(0, sz - suffixLength);
    }

    auto isAllOctNumber = true;
    int parseLen = static_cast<int>( parsedStr.size() );
    for ( int i = 0; i < parseLen; ++i )
    {
        if ( !isOctCode( parsedStr.at(i) ) ) {
            isAllOctNumber = false;
            break;
        }
    }

    return isAllOctNumber;
}

bool SequenceParser::isValid_Hex()
{
    using namespace charutil;

    string parsedStr = m_parsedSeq;
    int sz = static_cast<int>( parsedStr.size() );
    if ( sz <= 2 ) {
        return false;
    }
    
    // sz >=3
    string prefix2 = parsedStr.substr(0,2);
    if ( prefix2 != "0x" && prefix2 != "0X" ) {
        return false;
    }
    // strip prefix 0x / 0X
    parsedStr = parsedStr.substr(2);

    if ( !m_int_suffix_string.empty() ){
        int suffixLength = static_cast<int>( m_int_suffix_string.size() );
        sz = static_cast<int>( parsedStr.size() );
        // strip u/l
        parsedStr = parsedStr.substr(0, sz - suffixLength);
    }

    auto isAllHexNumber = true;
    int parseLen = static_cast<int>( parsedStr.size() );
    for ( int i = 0; i < parseLen; ++i )
    {
        if ( !isHexCode( parsedStr.at(i) ) ) {
            isAllHexNumber = false;
            break;
        }
    }

    return isAllHexNumber;
}

bool SequenceParser::isValid_Float()
{
    using namespace charutil;

    string parsedStr = m_parsedSeq;
    auto suffixCnt = m_float_fFSuffixCnt + m_float_lLSuffixCnt;
    if ( suffixCnt >= 2 ) {
        return false;   
    }

    int sz = static_cast<int>( parsedStr.size() );
    // suffixCnt <= 1
    if ( suffixCnt > 0 ) {
        // strip suffix f/F  or  l/L
        parsedStr = parsedStr.substr(0, (sz - suffixCnt) );
    }


    // float literal must contain either  '.' or  'e/E'
    if ( !(m_float_eECnt > 0 || m_float_dotCnt > 0) ) {
        return false;
    }


    if ( m_float_eECnt > 0 ) {
        auto eEpos = parsedStr.find(m_float_ch_eE);
        if ( eEpos == string::npos ) {
            return false;
        }

        string strBefore_E = parsedStr.substr(0,eEpos);
        string strAfter_E  = parsedStr.substr(eEpos + 1);

        if ( m_float_dotCnt > 0 ) {
            int numBeforeE = 0;
            int dotCnt     = 0;
            int otherMiscCnt = 0;
            int szBefore_E = static_cast<int>( strBefore_E.size() );
            for( int i = 0; i < szBefore_E; ++i ) {
                char ch = strBefore_E.at(i);
                if ( isNumber(ch) ) {
                    ++numBeforeE;
                } else if ( isDot(ch) ) {
                    ++dotCnt;
                } else {
                    ++otherMiscCnt;
                }
            }

            if ( m_float_dotCnt == 1 && numBeforeE > 0 && otherMiscCnt == 0 ) {
                if ( strAfter_E.empty() ) {
                    return false;
                }

                char chAfter_eE = strAfter_E.at(0);
                auto isSign = (chAfter_eE == '+' || chAfter_eE == '-');
                string nextstr = strAfter_E;
                if ( isSign ) {
                    nextstr = nextstr.substr(1);
                }

                int tmpSz = static_cast<int>( nextstr.size() );
                int numAfterE = 0;
                int otherMiscAfterECnt = 0;
                for( int i = 0; i < tmpSz; ++i ) {
                    char ch =  nextstr.at(i);
                    if ( isNumber(ch) ) {
                        ++numAfterE;
                    } else {
                        ++otherMiscAfterECnt;
                    }
                }

                return (numAfterE > 0 && otherMiscAfterECnt == 0);
            } else {
                return false;
            }

        } else {
            // only   1 e/E  without  '.'
            // strBefore_E; 
            // strAfter_E;  
            int numBeforeE = 0;
            int otherCntBeforeE = 0;   
            int szBeforeE = static_cast<int>( strBefore_E.size() );
            for( int i = 0; i < szBeforeE; ++i )
            {
                char ch = strBefore_E.at(0);
                if ( isNumber(ch) ) {
                    ++numBeforeE;
                } else {
                    ++otherCntBeforeE;
                }
            }

            if ( numBeforeE > 0 && otherCntBeforeE == 0 ) {
                string restStr = strAfter_E;
                if ( restStr.empty() ) {
                    return false;
                }

                char signBit = restStr.at(0);
                if ( signBit == '+' || signBit == '-' ) {
                    restStr = restStr.substr(1);
                }

                int restSz = static_cast<int>( restStr.size() );
                int numAfterECnt   = 0;
                int otherAfterECnt = 0;
                for( int i = 0; i < restSz; ++i )
                {
                    char ch = restStr.at(i);
                    if ( isNumber(ch) ) {
                        ++numAfterECnt;
                    } else {
                        ++otherAfterECnt;
                    }
                }

                return numAfterECnt>0 && otherAfterECnt==0;
            } else {
                return false;
            }
        }
    } else {
        // only 1 '.'
        auto dotpos = parsedStr.find('.');
        if ( dotpos == string::npos ) {
            return false;
        }

        string strBeforeDot = parsedStr.substr(0,dotpos);
        int szBefore = static_cast<int>( strBeforeDot.size() );
        string strAfterDot  = parsedStr.substr(dotpos+1);
        int szAfter = static_cast<int>( strAfterDot.size() );

        int numCnt = 0;
        for( int i = 0; i < szBefore; ++i ) {
            if ( isNumber( strBeforeDot.at(i) ) ) {
                ++numCnt;
            }
        }

        if ( numCnt > 0 ) {
            return true;
        } else {
            for( int i = 0; i < szAfter; ++i ) {
                if ( isNumber( strAfterDot.at(i) ) ) {
                    ++numCnt;
                }
            }

            return numCnt > 0;
        }
    }
}

bool SequenceParser::isValid_Varible()
{
    using namespace charutil;

    if ( m_parsedSeq.empty() ) {
        return false;
    }

    
    string parsedStr = m_parsedSeq;
    auto isAllVaribleCh = true;
    int parseLen = static_cast<int>( parsedStr.size() );
    for ( int i = 0; i < parseLen; ++i )
    {
        auto ch = parsedStr.at(i);
        if ( i == 0 ) {
            if ( !isVaribleChar1st(ch ) ) {
                isAllVaribleCh = false;
                break;
            }
        } else {
            if ( !isVaribleChar( ch ) ) {
                isAllVaribleCh = false;
                break;
            }
        }
    }

    return isAllVaribleCh;
}


// virtual
bool SequenceParser::isParsedSeqValid(string& errorMSg)
{
    auto isSeqvalid = false;

    switch( m_guessType )
    {
    case SequenceParser::E_GUESS_DEC:
        {
            isSeqvalid = isValid_Decimal();
            if ( !isSeqvalid ) {
                errorMSg = "invalid decimal format ";
            }
        }
        break;
    case SequenceParser::E_GUESS_OCT:
        {
            isSeqvalid = isValid_Octal();
            if ( !isSeqvalid ) {
                errorMSg = "invalid octal format ";
            }
        }
        break;
    case SequenceParser::E_GUESS_HEX:
        {
            isSeqvalid = isValid_Hex();
            if ( !isSeqvalid ) {
                errorMSg = "invalid hex format ";
            }
        }
        break;
    case SequenceParser::E_GUESS_FLOAT:
        {
            isSeqvalid = isValid_Float();
            if ( !isSeqvalid ) {
                errorMSg = "invalid float format ";
            }
        }
        break;
    case SequenceParser::E_GUESS_VARIBLE:
        {
            isSeqvalid = isValid_Varible();
            if ( !isSeqvalid ) {
                errorMSg = "invalid varible format ";
            }
        }
        break;
    default:
        break;
    }


    return isSeqvalid;
}


// virtual 
TokenBase* SequenceParser::generateToken() // override;
{
    TokenBase* pGenToken = nullptr;
    switch ( m_guessType )
    {
    case SequenceParser::E_GUESS_DEC:
        pGenToken = new TokenBase( calcFixedLiteralDataType() );
        pGenToken->setBaseAsFixedLiteral( 10 );
        break;
    case SequenceParser::E_GUESS_OCT:
        pGenToken = new TokenBase( calcFixedLiteralDataType() );
        pGenToken->setBaseAsFixedLiteral( 8 );
        break;
    case SequenceParser::E_GUESS_HEX:
        pGenToken = new TokenBase( calcFixedLiteralDataType() );
        pGenToken->setBaseAsFixedLiteral( 16 );
        break;
    case SequenceParser::E_GUESS_FLOAT:
        pGenToken = new TokenBase( calcFixedLiteralDataType() );
        // treat 2 as float type fixed literal
        pGenToken->setBaseAsFixedLiteral( 2 );
        break;
    case SequenceParser::E_GUESS_VARIBLE:
        {
            pGenToken = new TokenBase( E_TP_UNKNOWN );
            if ( KeyWordList::isKeyWord( m_parsedSeq) ) {
                if ( !KeyWordList::isTypeKeyWord( m_parsedSeq ) ) { 
                    MyException e(E_THROW_VARIBLE_CANNOT_BE_KEYWORD );
                    e.setDetail( surroundDoubleQuoto(m_parsedSeq) + string(" @") + m_beginChPos.getPos(0) );
                    throw e;
                } 

                // is TypeKeyWord
                pGenToken->setAsKeyword();
            } else {
                // make sure the datatype of the varible from variblePool  or from   varible definition 
                // maybe statement :     int a;  , varible named 'a' will be set as int type in  tokenMgr
                pGenToken->setAsVarible();
            }
        }
        break;
    default:
        break;
    }



    if ( pGenToken != nullptr ) {
        pGenToken->setBeginPos( m_beginChPos );
        pGenToken->setEndPos(   m_endChPos );

        string strWithoutSuffix;
        if ( pGenToken->isFixedLiteral() ) {
            if ( m_guessType == SequenceParser::E_GUESS_FLOAT ) {
                if ( m_float_reachSuffix_fF_lL_flag ) {
                    // m_float_fFSuffixCnt == 1    or   m_float_lLSuffixCnt == 1
                    strWithoutSuffix = m_parsedSeq.substr(0, static_cast<int>( m_parsedSeq.size() - 1) );
                } else {
                    strWithoutSuffix = m_parsedSeq;
                }
            } else {
                // Dec / Oct / Hex
                int uCnt,lCnt;
                uCnt = lCnt = 0;
                isIntSuffixValid(&uCnt, &lCnt);
                auto totalSuffixCnt = uCnt + lCnt;
                if ( totalSuffixCnt != 0 ) {
                    // strip suffix
                    strWithoutSuffix = m_parsedSeq.substr(0, static_cast<int>( m_parsedSeq.size() - totalSuffixCnt) );
                } else {
                    strWithoutSuffix = m_parsedSeq;
                }
            }

            pGenToken->setTokenContentWithoutSuffix( m_parsedSeq, strWithoutSuffix );
        } else {
            pGenToken->setTokenContent( m_parsedSeq );
        }

        if ( m_warningCnt > 0 ) {
            pGenToken->setWarningContent( m_warningContent );
        }
    }

    return pGenToken;
}


E_DataType SequenceParser::calcFixedLiteralDataType()
{
    using namespace charutil;

    E_DataType dt = E_TP_UNKNOWN;

    if ( m_guessType == E_GUESS_FLOAT ) {
        if( m_float_reachSuffix_fF_lL_flag ) {
            dt = E_TP_FLOAT; // suffix is 'f/F'

            // if( m_float_fFSuffixCnt ) {
            //     dt = E_TP_FLOAT; // suffix is 'f/F'
            // } else {
            //     dt = E_TP_LONG_DOUBLE; // suffix is 'l/L'
            // }

        } else {
            dt = E_TP_DOUBLE;
        }
    } else {
        // decimal / octal / hex
        if ( !m_int_suffix_string.empty() ) {
            int uCnt,lCnt;
            uCnt = lCnt = 0;
            isIntSuffixValid(&uCnt, &lCnt);

            string stripSuffixULL = m_parsedSeq.substr(0, static_cast<int>( m_parsedSeq.size() - (uCnt + lCnt) ) );

            int compareType = 1;
            if ( lCnt == 0 ) { // only 1 'u'
                // dt = E_TP_U_INT;

                switch ( m_guessType )
                {
                case SequenceParser::E_GUESS_DEC:
                    {
                        StringNumber currentNumber10(stripSuffixULL, 10);
                        if (          currentNumber10 <=  StringNumber::s_unsignedIntMax10 ) {
                            compareType = 1;
                        } else if (   currentNumber10 <=  StringNumber::s_unsignedLongMax10 ) {
                            compareType = 2;
                        } else if (   currentNumber10 <=  StringNumber::s_unsignedLongLongMax10 ) {
                            compareType = 3;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (10) is out of max int : " + StringNumber::s_unsignedLongLongMax10.getStrNumber() + SPACE_1 + "@" + m_beginChPos.getPos(0);
                        }
                    }
                    break;
                case SequenceParser::E_GUESS_OCT:
                    {
                        StringNumber currentNumber8(stripSuffixULL, 8);
                        if (          currentNumber8 <=  StringNumber::s_unsignedIntMax8 ) {
                            compareType = 1;
                        } else if (   currentNumber8 <=  StringNumber::s_unsignedLongMax8 ) {
                            compareType = 2;
                        } else if (   currentNumber8 <=  StringNumber::s_unsignedLongLongMax8 ) {
                            compareType = 3;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (8) is out of max int : " + StringNumber::s_unsignedLongLongMax8.getStrNumber() + SPACE_1 + "@" + m_beginChPos.getPos(0);
                        }
                    }
                    break;
                case SequenceParser::E_GUESS_HEX:
                    {
                        StringNumber currentNumber16(stripSuffixULL, 16);
                        if (          currentNumber16 <=  StringNumber::s_unsignedIntMax16 ) {
                            compareType = 1;
                        } else if (   currentNumber16 <=  StringNumber::s_unsignedLongMax16 ) {
                            compareType = 2;
                        } else if (   currentNumber16 <=  StringNumber::s_unsignedLongLongMax16 ) {
                            compareType = 3;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (16) is out of max int : " + StringNumber::s_unsignedLongLongMax16.getStrNumber() + SPACE_1 + "@" + m_beginChPos.getPos(0);
                        }
                    }
                    break;
                default:
                    break;
                }


                if ( compareType == 1 ) {
                    dt = E_TP_U_INT;
                } else if ( compareType == 2 ) {
                    dt = E_TP_U_LONG;
                } else if ( compareType == 3 ) {
                    dt = E_TP_U_LONG_LONG;
                } else {
                    dt = E_TP_U_INT;
                }

            } else if ( lCnt == 1 ) {
                // 1 'l' , 0/1 'u'
                // dt = (uCnt>0 ? E_TP_U_LONG : E_TP_S_LONG);

                int compareType = 2;

                switch ( m_guessType )
                {
                case SequenceParser::E_GUESS_DEC:
                    {
                        StringNumber currentNumber10(stripSuffixULL, 10);
                        if (         (uCnt>0) ?  (currentNumber10 <=  StringNumber::s_unsignedLongMax10)     : (currentNumber10 <=  StringNumber::s_signedLongMax10) ) {        
                            compareType = 2;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (10) is out of max int : ";
                            if ( uCnt > 0 ) {
                                m_warningContent += (StringNumber::s_unsignedLongMax10.getStrNumber());
                            } else {
                                m_warningContent += (StringNumber::s_signedLongMax10.getStrNumber());
                            }
                            m_warningContent += (SPACE_1 + "@" + m_beginChPos.getPos(0) );
                        }

                        /*
                        else if (  (uCnt>0) ?  (currentNumber10 <=  StringNumber::s_unsignedLongLongMax10) : (currentNumber10 <= StringNumber::s_signedLongLongMax10)  ) {
                            compareType = 3;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (10) is out of max int : ";
                            if ( uCnt > 0 ) {
                                m_warningContent += StringNumber::s_unsignedLongLongMax10.getStrNumber();
                            } else {
                                m_warningContent += StringNumber::s_signedLongLongMax10.getStrNumber();
                            }
                        }
                        */
                    }
                    break;
                case SequenceParser::E_GUESS_OCT:
                    {
                        StringNumber currentNumber8(stripSuffixULL, 8);
                        if (         (uCnt>0) ?  (currentNumber8 <=  StringNumber::s_unsignedLongMax8)     : (currentNumber8 <=  StringNumber::s_signedLongMax8) ) {        
                            compareType = 2;
                        } else {
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (8) is out of max int : ";
                            if ( uCnt > 0 ) {
                                m_warningContent += StringNumber::s_unsignedLongMax8.getStrNumber();
                            } else {
                                m_warningContent += StringNumber::s_signedLongMax8.getStrNumber();
                            }
                            m_warningContent += (SPACE_1 + "@" + m_beginChPos.getPos(0) );
                        }

                        /* 
                        
                        else if (  (uCnt>0) ?  (currentNumber8 <=  StringNumber::s_unsignedLongLongMax8) : (currentNumber8 <= StringNumber::s_signedLongLongMax8)  ) {
                            compareType = 3;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (8) is out of max int : ";
                            if ( uCnt > 0 ) {
                                m_warningContent += StringNumber::s_unsignedLongLongMax8.getStrNumber();
                            } else {
                                m_warningContent += StringNumber::s_signedLongLongMax8.getStrNumber();
                            }
                        }
                        */
                    }
                    break;
                case SequenceParser::E_GUESS_HEX:
                    {
                        StringNumber currentNumber16(stripSuffixULL, 16);
                        if (         (uCnt>0) ?  (currentNumber16 <=  StringNumber::s_unsignedLongMax16)     : (currentNumber16 <= StringNumber::s_signedLongMax16) ) {        
                            compareType = 2;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (16) is out of max int : ";
                            if ( uCnt > 0 ) {
                                m_warningContent += StringNumber::s_unsignedLongMax16.getStrNumber();
                            } else {
                                m_warningContent += StringNumber::s_signedLongMax16.getStrNumber();
                            }
                            m_warningContent += (SPACE_1 + "@" + m_beginChPos.getPos(0) );
                        }

                        /*
                        else if (  (uCnt>0) ?  (currentNumber16 <=  StringNumber::s_unsignedLongLongMax16) : (currentNumber16 <= StringNumber::s_signedLongLongMax16)  ) {
                            compareType = 3;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (16) is out of max int : ";
                            if ( uCnt > 0 ) {
                                m_warningContent += StringNumber::s_unsignedLongLongMax16.getStrNumber();
                            } else {
                                m_warningContent += StringNumber::s_signedLongLongMax16.getStrNumber();
                            }
                        }
                        */
                    }
                    break;
                default:
                    break;
                }

                (void)compareType;
                dt = (uCnt>0 ? E_TP_U_LONG : E_TP_S_LONG);

                // if ( compareType == 2 ) {
                //     dt = (uCnt>0 ? E_TP_U_LONG : E_TP_S_LONG);
                // } else if ( compareType == 3 ) {
                //     dt = (uCnt>0 ? E_TP_U_LONG_LONG : E_TP_S_LONG_LONG);
                // } else {
                //     dt = (uCnt>0 ? E_TP_U_LONG : E_TP_S_LONG);
                // }

            } else { // lCnt == 2
                // 2 'l' , 0/1 'u'
                // dt = (uCnt>0 ? E_TP_U_LONG_LONG : E_TP_S_LONG_LONG);

                int compareType = 3;
                switch ( m_guessType )
                {
                case SequenceParser::E_GUESS_DEC:
                    {
                        StringNumber currentNumber10(stripSuffixULL, 10);
                        if ( (uCnt>0) ?   (currentNumber10 <=  StringNumber::s_unsignedLongLongMax10) : (currentNumber10 <=  StringNumber::s_signedLongLongMax10) ) {
                            compareType = 3;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (10) is out of max int : ";
                            if ( uCnt > 0 ) {
                                m_warningContent += StringNumber::s_unsignedLongLongMax10.getStrNumber();
                            } else {
                                m_warningContent += StringNumber::s_signedLongLongMax10.getStrNumber();
                            }
                            m_warningContent += (SPACE_1 + "@" + m_beginChPos.getPos(0) );
                        }
                    }
                    break;
                case SequenceParser::E_GUESS_OCT:
                    {
                        StringNumber currentNumber8(stripSuffixULL, 8);
                        if ( (uCnt>0)  ?   (currentNumber8 <=  StringNumber::s_unsignedLongLongMax8) : (currentNumber8 <=  StringNumber::s_signedLongLongMax8) ) {
                            compareType = 3;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (8) is out of max int : ";
                            if ( uCnt > 0 ) {
                                m_warningContent += StringNumber::s_unsignedLongLongMax8.getStrNumber();
                            } else {
                                m_warningContent += StringNumber::s_signedLongLongMax8.getStrNumber();
                            }
                            m_warningContent += (SPACE_1 + "@" + m_beginChPos.getPos(0) );
                        }
                    }
                    break;
                case SequenceParser::E_GUESS_HEX:
                    {
                        StringNumber currentNumber16(stripSuffixULL, 16);
                        if ( (uCnt>0)  ?   (currentNumber16 <=  StringNumber::s_unsignedLongLongMax16) : (currentNumber16 <=  StringNumber::s_signedLongLongMax16) ) {
                            compareType = 3;
                        } else {
                            // larger than max number
                            ++m_warningCnt;
                            m_warningContent = m_parsedSeq + " (16) is out of max int : ";
                            if ( uCnt > 0 ) {
                                m_warningContent += StringNumber::s_unsignedLongLongMax16.getStrNumber();
                            } else {
                                m_warningContent += StringNumber::s_signedLongLongMax16.getStrNumber();
                            }
                            m_warningContent += (SPACE_1 + "@" + m_beginChPos.getPos(0) );
                        }
                    }
                    break;
                default:
                    break;
                }
                (void)compareType;
                dt = (uCnt>0 ? E_TP_U_LONG_LONG : E_TP_S_LONG_LONG);


                // if ( compareType == 3 ) {
                //     dt = (uCnt>0 ? E_TP_U_LONG_LONG : E_TP_S_LONG_LONG);
                // } else {
                //     dt = (uCnt>0 ? E_TP_U_LONG_LONG : E_TP_S_LONG_LONG);
                // }

            }
        } else {
            // No Suffix   "ull"
            // 1: int     2: long      3:long long
            int compareType = 1;
            switch ( m_guessType )
            {
            case SequenceParser::E_GUESS_DEC:
                {
                    StringNumber currentNumber10(m_parsedSeq, 10);
                    if (          currentNumber10 <=  StringNumber::s_signedIntMax10 ) {
                        compareType = 1;
                    } else if (   currentNumber10 <=  StringNumber::s_signedLongMax10 ) {
                        compareType = 2;
                    } else if (   currentNumber10 <=  StringNumber::s_signedLongLongMax10 ) {
                        compareType = 3;
                    } else {
                        // larger than max number
                        ++m_warningCnt;
                        m_warningContent = m_parsedSeq + " (10) is out of max int : " + StringNumber::s_signedLongLongMax10.getStrNumber();
                        m_warningContent += (SPACE_1 + "@" + m_beginChPos.getPos(0) );
                    }
                }
                break;
            case SequenceParser::E_GUESS_OCT:
                {
                    StringNumber currentNumber8(m_parsedSeq, 8);
                    if (          currentNumber8 <=  StringNumber::s_signedIntMax8 ) {
                        compareType = 1;
                    } else if (   currentNumber8 <=  StringNumber::s_signedLongMax8 ) {
                        compareType = 2;
                    } else if (   currentNumber8 <=  StringNumber::s_signedLongLongMax8 ) {
                        compareType = 3;
                    } else {
                        // larger than max number
                        ++m_warningCnt;
                        m_warningContent = m_parsedSeq + " (8) is out of max int : " + StringNumber::s_signedLongLongMax8.getStrNumber();
                        m_warningContent += (SPACE_1 + "@" + m_beginChPos.getPos(0) );
                    }
                }
                break;
            case SequenceParser::E_GUESS_HEX:
                {
                    StringNumber currentNumber16(m_parsedSeq, 16);
                    if (          currentNumber16 <=  StringNumber::s_signedIntMax16 ) {
                        compareType = 1;
                    } else if (   currentNumber16 <=  StringNumber::s_signedLongMax16 ) {
                        compareType = 2;
                    } else if (   currentNumber16 <=  StringNumber::s_signedLongLongMax16 ) {
                        compareType = 3;
                    } else {
                        // larger than max number
                        ++m_warningCnt;
                        m_warningContent = m_parsedSeq + " (16) is out of max int : " + StringNumber::s_signedLongLongMax16.getStrNumber();
                        m_warningContent += (SPACE_1 + "@" + m_beginChPos.getPos(0) );
                    }
                }
                break;
            default:
                break;
            }


            if ( compareType == 1) {
                dt = E_TP_S_INT;
            } else if ( compareType == 2 ) {
                dt = E_TP_S_LONG;
            } else if ( compareType == 3 ) {
                dt = E_TP_S_LONG_LONG;
            } else {
                dt = E_TP_S_INT;
            }
        }
    }

    return dt;
}


