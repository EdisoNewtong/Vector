#ifndef  SEQUENCE_PARSER_H
#define  SEQUENCE_PARSER_H

#include "parserBase.h"
#include <string>

class SequenceParser : public ParserBase
{
public:
    enum E_GUESS_TYPE {
        E_GUESS_UNKNOWN = 0,

        E_GUESS_DEC,
        E_GUESS_OCT,
        E_GUESS_HEX,

        E_GUESS_FLOAT,

        E_GUESS_VARIBLE,
    };


protected:
    struct SuffixInfo
    {
        std::string suffix;
        int    uUCnt;
        int    lLCnt;
    };

public:
    SequenceParser();
    virtual ~SequenceParser() override;

    virtual ParserBase::E_PARSER_TYPE appendChar(const ChInfo& rChInfo, E_ParserAction& afterAction) override;

    virtual void pushHeadChar(char ch , const ChInfo& rChInfo) override;

    virtual void resetInternalState() override; 
    virtual bool isParsedSeqValid(std::string& errorMsg) override;

    virtual TokenBase* generateToken() override;
protected:
    ParserBase::E_PARSER_TYPE appendForType_int(const ChInfo& rChInfo, E_ParserAction& afterAction);
    ParserBase::E_PARSER_TYPE appendForType_oct(const ChInfo& rChInfo, E_ParserAction& afterAction);
    ParserBase::E_PARSER_TYPE appendForType_hex(const ChInfo& rChInfo, E_ParserAction& afterAction);

    ParserBase::E_PARSER_TYPE appendForType_float(const ChInfo& rChInfo, E_ParserAction& afterAction);
    ParserBase::E_PARSER_TYPE appendForType_varible(const ChInfo& rChInfo, E_ParserAction& afterAction);

    void throw_Int_exception(const ChInfo& rChInfo, const std::string& detail);
    void throw_Oct_exception(const ChInfo& rChInfo, const std::string& detail);
    void throw_Hex_exception(const ChInfo& rChInfo, const std::string& detail);
    void throw_Float_exception(const ChInfo& rChInfo, const std::string& detail);
    void throw_Varible_exception(const ChInfo& rChInfo, const std::string& detail);

    void increase_dot_CntAndCheck(const ChInfo& rChInfo);
    void increase_eE_CntAndCheck(const ChInfo& rChInfo);
    void increase_float_suffix_fF_CntAndCheck(const ChInfo& rChInfo);
    void increase_float_suffix_lL_CntAndCheck(const ChInfo& rChInfo);

    bool isIntSuffixValid(int* pUCnt = nullptr,int* pLCnt = nullptr);  // to accumulate suffix string "ull"
    bool hasNumberBeforeOrAfterDot(bool beforeDot);

    // int  getNumberAfter_float_sep(char ch); 
    int  getNumberAfter_eE(); 
    bool isOctValid();

    /////////////////////////////////////////////
    //
    // Util Function : to check one kine of type is valid or not  
    //
    /////////////////////////////////////////////
    bool isValid_Decimal();
    bool isValid_Octal();
    bool isValid_Hex();
    bool isValid_Float();
    bool isValid_Varible();

    std::string surroundDoubleQuoto(const std::string str);

    E_DataType calcFixedLiteralDataType();
protected:
    E_GUESS_TYPE m_guessType;
    int          m_numberCnt;

    ///////////////////////////////////////////////////////////////////////////
    //
    // for float literal presentation used
    //
    ///////////////////////////////////////////////////////////////////////////
    int          m_float_dotCnt;            // e.g.   at most 1 '.'
    int          m_float_eECnt;             // e.g.   3.14e2  or  3.14E2 , at most 1
    char         m_float_ch_eE;             // 'e' or 'E'
    // either '+' or '-'
    int          m_float_positiveSignCnt;   // e.g.   3.14e+2  , at most 1 '+'
    int          m_float_negativeSignCnt;   // e.g.   3.14e-2  , at most 1 '-'

    int          m_float_fFSuffixCnt;       // e.g.   3.14f   or 3.14F , at most 1 'f/F'
    int          m_float_lLSuffixCnt;       // e.g.   3.14l   or 3.14L , at most 1 'l/L'
    bool         m_float_reachSuffix_fF_lL_flag;   // reach  f/F  or reach 'l/L' ( for long double )
    //
    ///////////////////////////////////////////////////////////////////////////


    static const int   S_FLOAT_DOT_UPPERLIMIT;       // 1 .
    static const int   S_FLOAT_eE_UPPERLIMIT;        // 1 e/E
    // static const int   S_FLOAT_POSITIVE_UPPERLIMIT;  // 1 '+'
    // static const int   S_FLOAT_NEGATIVE_UPPERLIMIT;  // 1 '-'
    static const int   S_FLOAT_fF_lL_SUFFIX_UPPERLIMIT; // 1  f/F

    //
    bool         m_oct_broken_flag;
    std::string  m_int_suffix_string; // ull or uL


};


#endif


