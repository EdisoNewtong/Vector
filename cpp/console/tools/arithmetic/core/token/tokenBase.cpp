#include "tokenBase.h"
#include "keywordList.h"
#include "myException.h"
#include "charUtil.h"
#include "stringNumber.h"
#include <cstdlib>
using namespace std;




TokenBase::TokenBase(E_TokenType tp)
    : m_tokenType(tp)
    , m_opType(E_OPERATOR_UNKNOWN)
    , m_dataType(E_TP_UNKNOWN)
    , m_expTypeFlag(0U)
	, m_opFlag( E_OP_FLAG_UNKNOWN )
    , m_token_content()
    , m_beginPos()
    , m_endPos()
    , m_dataValue()
    , m_warningContent()
{
    m_dataValue.type = E_TP_UNKNOWN;
}


TokenBase::TokenBase(E_DataType dt)
    : m_tokenType( E_TOKEN_EXPRESSION ) // set as expression flag
    , m_opType(E_OPERATOR_UNKNOWN)
    , m_dataType(dt)
    , m_expTypeFlag(0U)
	, m_opFlag( E_OP_FLAG_UNKNOWN )
    , m_token_content()
    , m_beginPos()
    , m_endPos()
    , m_dataValue()
{
    m_dataValue.type = dt;
}


//virtual 
TokenBase::~TokenBase()
{
}

E_TokenType TokenBase::getTokenType()
{
    return m_tokenType;
}





//////////////////////////////////////////////////////////////////////
//
// Operator get/set
//
E_OperatorType TokenBase::getOperatorType()
{
    return m_opType;
}


void TokenBase::setOpType(E_OperatorType opTp)
{
    m_opType = opTp;
}

//////////////////////////////////////////////////////////////////////
//
// DataType get/set
//
E_DataType TokenBase::getDataType()
{
    return m_dataType;
}


void TokenBase::setDataType(E_DataType dt)
{
    m_dataType = dt;
}


//////////////////////////////////////////////////////////////////////
//
// Token Content  get/set
//
string TokenBase::getTokenContent()
{
    return m_token_content;
}

void TokenBase::setTokenContent(const string& content)
{
    m_token_content = content;
}


void TokenBase::setTokenContentWithoutSuffix(const std::string& content, const std::string& noSuffix)
{
    m_token_content = content;
    // noSuffix
    if ( isFixedLiteral() ) {
        int base = getFixedLiteralBase();
        if ( base == 0 ) {
            MyException e(E_THROW_INVALID_NUMBER_BASE,  this->m_beginPos );
            e.setDetail( content );
            throw e;
        }

        char* str_end = nullptr;
        if ( base == 2 ) {
            // float / double 
            switch ( m_dataValue.type )
            {
            case E_TP_FLOAT:
                {
                    float f = strtof(noSuffix.c_str(), &str_end);
                    if ( !( str_end == nullptr || ( str_end != nullptr && (*str_end) == '\0' ) ) ) {
                        MyException e(E_THROW_INVALID_FLOAT_NUMBER_WHEN_CONVERTING , this->m_beginPos );
                        e.setDetail( noSuffix );
                        throw e;
                    } 

                    m_dataValue.value.float_val = f;
                }
                break;
            case E_TP_DOUBLE:
                {
                    double d = strtod(noSuffix.c_str(), &str_end);
                    if ( !( str_end == nullptr || ( str_end != nullptr && (*str_end) == '\0' ) ) ) {
                        MyException e(E_THROW_INVALID_DOUBLE_NUMBER_WHEN_CONVERTING, this->m_beginPos);
                        e.setDetail( noSuffix );
                        throw e;
                    } 

                    m_dataValue.value.double_val = d;
                }
                break;
            /*
            case E_TP_LONG_DOUBLE:
                {   
                    long double ld = strtold(noSuffix.c_str(), &str_end);
                    if ( !( str_end == nullptr || ( str_end != nullptr && (*str_end) == '\0') ) ) {
                        MyException e(E_THROW_INVALID_LONG_DOUBLE_NUMBER_WHEN_CONVERTING , this->m_beginPos);
                        e.setDetail( noSuffix );
                        throw e;
                    } 

                    m_dataValue.value.longdouble_val = ld;
                }
                break;
            */
            default:
                break;
            }

        } else {

            switch ( m_dataValue.type )
            {
            case E_TP_S_INT:
            case E_TP_S_LONG:
                {
					/***************************************************
					 
						strtol("0x80000000", &str_end, 16);   => the api yield the incorrect result number =>  0x7FFFFFFF
						strtol("0xFEDCBA98", &str_end, 16);   => the api yield the incorrect result number =>  0x7FFFFFFF


						Because the leader sign bit is '1'   when process a signed number 

					 ***************************************************/

                    // signed long l_num = strtol(noSuffix.c_str(), &str_end, base);
                    // if ( !( str_end == nullptr || ( str_end != nullptr && (*str_end) == '\0' ) ) ) {
                    //     MyException e(E_THROW_INVALID_LONG_NUMBER_WHEN_CONVERTING , this->m_beginPos );
                    //     e.setDetail( noSuffix );
                    //     throw e;
                    // }

					StringNumber strNum(noSuffix, base);
					size_t cvtBitSz = strNum.convert2Bin();
					size_t targetDataTypeBitSz = ( (m_dataValue.type == E_TP_S_INT) ?  (sizeof(int) * 8) : (sizeof(long) * 8) );

					if ( cvtBitSz > targetDataTypeBitSz ) {
						// trunc the cvtBitSz
						strNum.truncateToTargetSize( targetDataTypeBitSz );
					} 

					unsigned long l_num = strNum.getULongValue();
                    if ( m_dataType == E_TP_S_INT ) {
                        m_dataValue.value.sint_val = static_cast<int>( l_num );
                    } else {
                        m_dataValue.value.slong_val = static_cast<signed long>( l_num );
                    }
                }
                break;
            case E_TP_U_INT:
            case E_TP_U_LONG:
                {
                    // unsigned long ul_num = strtoul(noSuffix.c_str(), &str_end, base);
                    // if ( !( str_end == nullptr || ( str_end != nullptr && (*str_end) == '\0' ) ) ) {
                    //     MyException e(E_THROW_INVALID_UNSIGNED_LONG_NUMBER_WHEN_CONVERTING , this->m_beginPos );
                    //     e.setDetail( noSuffix );
                    //     throw e;
                    // }

					StringNumber strNum(noSuffix, base);
					size_t cvtBitSz = strNum.convert2Bin();
					size_t targetDataTypeBitSz = ( (m_dataValue.type == E_TP_U_INT) ?  (sizeof(unsigned int) * 8) : (sizeof(unsigned long) * 8) );

					if ( cvtBitSz > targetDataTypeBitSz ) {
						// trunc the cvtBitSz
						strNum.truncateToTargetSize( targetDataTypeBitSz );
					} 

					unsigned long ul_num = strNum.getULongValue();

                    if ( m_dataType == E_TP_U_INT  ) {
                        m_dataValue.value.uint_val = static_cast<unsigned int>( ul_num  );
                    } else {
                        m_dataValue.value.ulong_val = ul_num;
                    }
                }
                break;
            case E_TP_S_LONG_LONG:
                {
                    // signed long long ll_num = strtoll(noSuffix.c_str(), &str_end, base);
                    // if ( !( str_end == nullptr || ( str_end != nullptr && (*str_end) == '\0' ) ) ) {
                    //     MyException e(E_THROW_INVALID_LONG_LONG_NUMBER_WHEN_CONVERTING, this->m_beginPos);
                    //     e.setDetail( noSuffix );
                    //     throw e;
                    // } 

					StringNumber strNum(noSuffix, base);
					size_t cvtBitSz = strNum.convert2Bin();
					size_t targetDataTypeBitSz = ( sizeof(signed long long) * 8 );

					if ( cvtBitSz > targetDataTypeBitSz ) {
						// trunc the cvtBitSz
						strNum.truncateToTargetSize( targetDataTypeBitSz );
					}

					unsigned long long ll_num = strNum.getULongLongValue();
                    m_dataValue.value.slonglong_val = static_cast<signed long long>( ll_num ) ;
                }
                break;
            case E_TP_U_LONG_LONG:
                {
                    // unsigned long long ull_num = strtoull(noSuffix.c_str(), &str_end, base);
                    // if ( !( str_end == nullptr || ( str_end != nullptr && (*str_end) == '\0'  ) ) ) {
                    //     MyException e(E_THROW_INVALID_UNSIGNED_LONG_LONG_NUMBER_WHEN_CONVERTING, this->m_beginPos);
                    //     e.setDetail( noSuffix );
                    //     throw e;
                    // }

					StringNumber strNum(noSuffix, base);
					size_t cvtBitSz = strNum.convert2Bin();
					size_t targetDataTypeBitSz = ( sizeof(unsigned long long) * 8 );

					if ( cvtBitSz > targetDataTypeBitSz ) {
						// trunc the cvtBitSz
						strNum.truncateToTargetSize( targetDataTypeBitSz );
					} 

					unsigned long long ull_num = strNum.getULongLongValue();
                    m_dataValue.value.ulonglong_val = ull_num;
                }
                break;
            default:
                break;
            }
        }
    }
}


void  TokenBase::setAsKeyword()
{
    m_dataType = E_TP_UNKNOWN;
    m_expTypeFlag = 0x40U;
}


//////////////////////////////////////////////////
// get/set   Begin Pos
ChInfo  TokenBase::getBeginPos()
{
    return m_beginPos;
}

void  TokenBase::setBeginPos(const ChInfo& beg)
{
    m_beginPos = beg;
}


//////////////////////////////////////////////////
// get/set   End Pos
ChInfo  TokenBase::getEndPos()
{
    return m_endPos;
}

void  TokenBase::setEndPos(const ChInfo& end)
{
    m_endPos = end;
}

bool   TokenBase::isKeyword()
{
    return m_expTypeFlag == 0x40U;
}

bool   TokenBase::isVarible()
{
	// m_expTypeFlag = 0x30U ??
    return ( ((m_expTypeFlag >> 4) & 0x3U) == 0x3U );
}

void   TokenBase::setAsVarible()
{
    //              | 0011 | 0000 |
    m_expTypeFlag = 0x30U;
}


void TokenBase::setBaseAsFixedLiteral(int base)
{
    if ( base == 10 ) {
        //              | 0010 | 0001 |
        m_expTypeFlag = 0x21U;
    } else if ( base == 8 ) {
        //              | 0010 | 0010 |
        m_expTypeFlag = 0x22U;
    } else if ( base == 16 ) {
        //              | 0010 | 0100 |
        m_expTypeFlag = 0x24U;
    } else if ( base == 2 ) {
        // treat 2 as float fixed literal
        //              | 0010 | 1000 |
        m_expTypeFlag = 0x28U;
    }
}

int TokenBase::getFixedLiteralBase()
{
    int retBase = 0;
    unsigned char meta = static_cast<unsigned char>( m_expTypeFlag & 0xFU );
    if ( meta == 0x1U ) {
        retBase = 10;
    } else if ( meta == 0x2U ) {
        retBase = 8;
    } else if ( meta == 0x4U ) {
        retBase = 16;
    } else if ( meta == 0x8U ) {
        // treat 2 as float fixed literal
        retBase = 2;
    } else {
        retBase = 0;
    }

    return retBase;
}



bool TokenBase::isFixedLiteral()
{
    unsigned char meta = static_cast<unsigned char>( m_expTypeFlag & 0xFU );
    return (meta != 0U);
}




bool TokenBase::isTmpExpression()
{
    return (m_expTypeFlag == 0x20U);
}



void  TokenBase::setAsTmpExpression()
{
    m_expTypeFlag = 0x20U;
}


DataValue TokenBase::getRealValue()
{
    return m_dataValue;
}

void TokenBase::setRealValue(const DataValue& value)
{
    m_dataValue = value;
}


string    TokenBase::getExpressionContent()
{
    using namespace charUtil;

    string retExpr = getTokenContent();

    if ( isTmpExpression() ) {
        int sz = static_cast<int>( retExpr.size() );
        if ( sz >= 2 ) {
            if (    !(   retExpr.front() == STR_OPEN_PARENTHESIS.at(0)
                      && retExpr.back()  == STR_CLOSE_PARENTHESIS.at(0) ) )
            {
                retExpr = (STR_OPEN_PARENTHESIS + SPACE_1
                           + retExpr 
                           + SPACE_1 + STR_CLOSE_PARENTHESIS);
            }
        } 
    } 

    return retExpr;
}



void   TokenBase::setWarningContent( const std::string& content)
{
    m_warningContent = content;
}



string    TokenBase::getWarningContent()
{
    return m_warningContent;
}



void  TokenBase::setContextRoleForOp(const E_OpAnotherFlag& flag)
{
	m_opFlag = flag;
}

E_OpAnotherFlag  TokenBase::getContextRoleForOp()
{
	return m_opFlag;
}


bool	TokenBase::isFunction()
{
	return m_expTypeFlag == 0x80;
}

void    TokenBase::setAsFunction()
{
	m_expTypeFlag = 0x80;
}

