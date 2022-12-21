#ifndef TOKEN_BASE_H
#define TOKEN_BASE_H

#include "commonEnum.h"
#include "buff.h"
#include "dataValue.h"
#include <string>

class TokenBase
{
public:
    TokenBase(E_TokenType tp);
    TokenBase(E_DataType dt);
    virtual ~TokenBase();

    E_TokenType    getTokenType();

    E_OperatorType getOperatorType();
    void           setOpType(E_OperatorType opTp);

    E_DataType     getDataType();
    void           setDataType(E_DataType dt);

    void           setBaseAsFixedLiteral(int base);
    int            getFixedLiteralBase();
    bool           isFixedLiteral();

    bool           isTmpExpression();
    void           setAsTmpExpression();

    std::string    getTokenContent();
    void           setTokenContent(const std::string& content);
    void           setTokenContentWithoutSuffix(const std::string& content, const std::string& noSuffix);
    std::string    getExpressionContent();

    bool           isKeyword();
    void           setAsKeyword();

    bool           isVarible();
    void           setAsVarible();

    void           setBeginPos(const ChInfo& beg);
    ChInfo         getBeginPos();
    void           setEndPos(const ChInfo& end);
    ChInfo         getEndPos();

    DataValue      getRealValue();
    void           setRealValue(const DataValue& value);

    void           setWarningContent( const std::string& content);
    std::string    getWarningContent();
protected:
    E_TokenType    m_tokenType;

    E_OperatorType m_opType;
    E_DataType     m_dataType;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //-----------------------------------------------------------------------------------------------------
    //             |      7      6    5      |   4     3   2   1  |
    //-----------------------------------------------------------------------------------------------------
    // 8 bits      | x keywords exp varible  | float  hex oct dec |
    //----------------------------------------------------------------------------------------------------
    //
    //  if token is an intermediate tmp expression : val = 0x20
    //        e.g.  ( a + b ) =>  an innermediate tmp expression
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char  m_expTypeFlag;

    std::string    m_token_content;

    ChInfo         m_beginPos;
    ChInfo         m_endPos;

    DataValue      m_dataValue;

    std::string    m_warningContent;
};

#endif