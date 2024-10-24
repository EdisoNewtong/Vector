#include "myException.h"
#include "enumUtil.h"
#include "charUtil.h"
using namespace std;

// static 
unordered_map<MyException::ErrorType,string>    MyException::s_exceptionMap{
    { enumCvt(E_THROW_UNKNOWN_CHAR),                            "Unknown character "},
    { enumCvt(E_THROW_CODE_CANNOT_REACH_HERE),                  "Strange ! code can't reach here. "},
    { enumCvt(E_THROW_NULL_PTR),                                "NULL pointer "},

    { enumCvt(E_THROW_UNSUPPORTED_FEATURE_LESS_THAN),           "Unsupported operator feature '<' , less than " },
    { enumCvt(E_THROW_UNSUPPORTED_FEATURE_GREATER_THAN),        "Unsupported operator feature '>' , greater than " },

    { enumCvt(E_THROW_UNSUPPORTED_FEATURE_NO_GREATER_THAN),        "Unsupported operator feature '<=' ( No Greater Than ) "},
    { enumCvt(E_THROW_UNSUPPORTED_FEATURE_NO_LESS_THAN),           "Unsupported operator feature '>=' ( No Less Than ) "},

    { enumCvt(E_THROW_UNSUPPORTED_FEATURE_EQUAL_TO),           "Unsupported operator feature '==' , equal to "},
    

    { enumCvt(E_THROW_INVALID_PARSEDSTR_TO_GENERATE_TOKEN),     "Invalid parsed sequence "},
    { enumCvt(E_THROW_INVALID_CHAR_IN_DECIMAL_SEQ),             "Malformed decimal string "},
    { enumCvt(E_THROW_INVALID_CHAR_IN_OCTAL_SEQ),               "Malformed octal string "},
    { enumCvt(E_THROW_INVALID_CHAR_IN_HEX_SEQ),                 "Malformed hex string "},
    { enumCvt(E_THROW_INVALID_CHAR_IN_FLOAT_SEQ),               "Malformed float string "},
    { enumCvt(E_THROW_INVALID_CHAR_IN_VARIBLE_SEQ),             "Malformed varible string "},


    { enumCvt(E_THROW_INVALID_NUMBER_BASE),                     "Invalid number Base "},
    { enumCvt(E_THROW_DIFFERENT_NUMBER_BASE),                   "Compare different number base "},
    { enumCvt(E_THROW_INVALID_DECIMAL_NUMBER),                  "Invalid decimal number "},
    { enumCvt(E_THROW_INVALID_OCTAL_NUMBER),                    "Invalid octal number "},
    { enumCvt(E_THROW_INVALID_HEX_NUMBER),                      "Invalid hex number "},

    { enumCvt(E_THROW_INVALID_LONG_NUMBER_WHEN_CONVERTING),                "Invalid long number string when converting "},
    { enumCvt(E_THROW_INVALID_UNSIGNED_LONG_NUMBER_WHEN_CONVERTING),       "Invalid unsigned long number string when converting "},
    { enumCvt(E_THROW_INVALID_LONG_LONG_NUMBER_WHEN_CONVERTING),           "Invalid long long number string when converting "},
    { enumCvt(E_THROW_INVALID_UNSIGNED_LONG_LONG_NUMBER_WHEN_CONVERTING),  "Invalid unsigned long long number string when converting "},

    { enumCvt(E_THROW_INVALID_FLOAT_NUMBER_WHEN_CONVERTING),               "Invalid float number string when converting "},
    { enumCvt(E_THROW_INVALID_DOUBLE_NUMBER_WHEN_CONVERTING),              "Invalid double number string when converting "},
    { enumCvt(E_THROW_INVALID_LONG_DOUBLE_NUMBER_WHEN_CONVERTING),         "Invalid long double number string when converting "},

    //
    { enumCvt(E_THROW_INVALID_OPERATOR),                        "Invalid operator "},

    { enumCvt(E_THROW_INVALID_TOKEN_RELATIONSHIP),              "Invalid relationship between 2 tokens "},

    { enumCvt(E_THROW_VARIBLE_ALREADY_DEFINED),                 "Varible already defined before "},
    { enumCvt(E_THROW_VARIBLE_CANNOT_BE_KEYWORD),               "Varible can't be keyword "},
    { enumCvt(E_THROW_VARIBLE_NOT_DEFINED),                     "Varible is undefined " },
    { enumCvt(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED),     "Varible hasn't been initialized before used " },
    { enumCvt(E_THROW_VARIBLE_IS_MISSING),                      "Varible is missing" },

    { enumCvt(E_THROW_SENTENCE_LESS_TOKEN),                     "Sentence has too few tokens " },
    { enumCvt(E_THROW_SENTENCE_MORE_TOKEN),                     "Sentence has too more assignment " },

    { enumCvt(E_THROW_SENTENCE_PREFIX_TOKENS_ARE_INVALID),               "Sentence prefix tokens are too strange : " },
    { enumCvt(E_THROW_SENTENCE_PREFIX_TOKENS_BEFORE_EQUAL_ARE_INVALID),  "Sentence prefix tokens before '=' are too strange : " },
    { enumCvt(E_THROW_SENTENCE_DEFINITION_PREFIX_IS_NOT_VARIBLE),        "Sentence definition prefix is not a varible " },
    { enumCvt(E_THROW_SENTENCE_2ND_IS_NOT_A_VARIADIC_ASSIGNMENT),        "Sentence the 2nd token is not a variadic-assignment" },
    { enumCvt(E_THROW_SENTENCE_DEFINITION_TOO_MANY_KEYWORDS),            "Sentence definition has too many keywords " },
    { enumCvt(E_THROW_SENTENCE_NO_EXPR_BEFORE_ASSIGNMENT),               "Sentence no expression before assignment " },
    { enumCvt(E_THROW_SENTENCE_NO_EXPR_AFTER_ASSIGNMENT),                "Sentence no expression after assignment " },
    { enumCvt(E_THROW_SENTENCE_UNKNOWN_DATA_TYPE),                       "Sentence unknown data type " },
    { enumCvt(E_THROW_SENTENCE_TPYE_IS_UNDETERMINDED),                   "Sentence type is undeterminded " },

    { enumCvt(E_THROW_NO_MATCHED_OPEN_PARENTHESIS),                      "Sentence no matched '(' while pushing   ')' " },
    { enumCvt(E_THROW_NO_MATCHED_CLOSE_PARENTHESIS),                     "Sentence no matched ')' while building suffix expression. " },
    { enumCvt(E_THROW_CANNOT_PUSH_TOKEN_KEYWORD),                        "Keyword can't existed after '=' " },


    { enumCvt(E_THROW_SUFFIXEXPR_BINARY_OP_MISS_TWO_OPERANDS),           "Suffix-Expression miss 2 operands while processing binary operator " },
    { enumCvt(E_THROW_SUFFIXEXPR_UNARY_OP_MISS_ONE_OPERAND),             "Suffix-Expression miss 1 operand  while processing unary operator " },
    { enumCvt(E_THROW_SUFFIXEXPR_FINAL_EVALUATE_MUST_LEAVE_ONLY_ONE_ELEMENT),   "Suffix-Expression's final size must be only one. " },

    { enumCvt(E_THROW_DIVIDE_ZERO),                                      "Interger operator : divide by Zero " },
    { enumCvt(E_THROW_MODULO_ZERO),                                      "Interger operator : modulo by Zero " },

    { enumCvt(E_THROW_MODULO_CANNOT_APPLY_ON_FLOAT),                     "Operator modulo can't apply on float " },

    { enumCvt(E_THROW_BITAND_CANNOT_APPLY_ON_FLOAT),                     "Bit-And can't apply on float " },
    { enumCvt(E_THROW_BITOR_CANNOT_APPLY_ON_FLOAT),                      "Bit-Or  can't apply on float " },
    { enumCvt(E_THROW_BITXOR_CANNOT_APPLY_ON_FLOAT),                     "Bit-XOr can't apply on float " },
    { enumCvt(E_THROW_BITNOT_CANNOT_APPLY_ON_FLOAT),                    "Bit-Not can't apply on float " },

    { enumCvt(E_THROW_BIT_LEFTSHIFT_CANNOT_APPLY_ON_FLOAT),              "Bit-Left-Shift can't apply on float " },
    { enumCvt(E_THROW_BIT_RIGHTSHIFT_CANNOT_APPLY_ON_FLOAT),             "Bit-Right-Shift can't apply on float " },
    { enumCvt(E_THROW_INVALID_SUFFIX_EXPRESSION),                        "Invalid suffix expression " },
    { enumCvt(E_THROW_LAST_VALID_TOKEN_IS_NOT_SEMICOLON),                "Final valid token is not ';' " },

    { enumCvt(E_THROW_UNSUPPORTED_FEATURE_INCREASEMENTAL_1),             "++ ( self increasemental operator feature ) is not supported in version " },
    { enumCvt(E_THROW_UNSUPPORTED_FEATURE_DECREASEMENTAL_1),             "-- ( self decreasemental operator feature ) is not supported in version " },
    { enumCvt(E_THROW_UNSUPPORTED_FEATURE_LOGIC_AND),                    "&& ( logic and operator feature ) is not supported in version " },
    { enumCvt(E_THROW_UNSUPPORTED_FEATURE_LOGIC_OR),                     "|| ( logic or  operator feature ) is not supported in version " },

    { enumCvt(E_THROW_FIRST_TOKEN_IS_INVALID),                              "First token is not valid. " },

    { enumCvt(E_THROW_ALL_TOKENS_ARE_KEYWORD),                              "All sentence which is full of keywords is syntax error. " },
    { enumCvt(E_THROW_TOO_MANY_KEYWORDS),                                   "The keywords' count is exceeded the limit.  " },
    { enumCvt(E_THROW_SENTENCE_DEFINITION_HAS_MISSED_AN_ASSIGNMENT_OPERATOR),  "The sentence defination has missed an assignment operator. " },

    { enumCvt(E_THROW_FUNCTION_OBJECT_IS_NULLPTR),                            "Function object is a nullptr pointer. "    },
    { enumCvt(E_THROW_CALL_FUNCTION_TOO_MANY_ARGUMENTS),                      "No matched function prototype , too many arguments is given. "    },
    { enumCvt(E_THROW_CALL_FUNCTION_TOO_LITTLE_ARGUMENTS),                    "No matched function prototype , more arguments is required. " },
    { enumCvt(E_THROW_PARSE_FUNCTION_PUSH_TOO_MANY_ARGUMENTS),                "During token parsing , too many arugments will be pushed to arguments. " },
    { enumCvt(E_THROW_UNKNOWN_BUILT_IN_FUNCTION_NAME),                        "Unknown built-in function name. " },
    { enumCvt(E_THROW_EMPTY_CONTENT_INSIDE_PARENTHESIS_PAIR_IS_NOT_ALLOWED),  "empty content inside none function version '()' pair is not allowed.  " },
    { enumCvt(E_THROW_CANNOT_DEFINE_A_VARIBLE_WITH_THE_SAME_NAME_OF_BUILT_IN_FUNCTION), "Can't define a varible with the same name of built-in function. " },
    { enumCvt(E_THROW_FUNCTION_ARGUMENT_CANNOT_BE_EMPTY_WHEN_MEET_COMMA),               "one function argument expression can't be empty when meet   ',' . " },
    { enumCvt(E_THROW_FUNCTION_IS_NOT_VALID_BY_MISSING_CLOSE_PARENTHESIS),               "the function is not valid because of missing a ')' . " },

    // for force type cast    new feature
    { enumCvt(E_THROW_FORCE_TYPE_CAST_LIST_IS_EMPTY),               "the force type cast list is empty . " },

    { enumCvt(E_THROW_CALL_STACK_SHOULD_NEVER_BE_EMPTY) , "Call Stack should never be empty. " }
};



// static 
string MyException::s_retDetail = "";



MyException::MyException(E_ExceptionType errorCode)
    : exception()
    , m_errorCode(errorCode)
    , m_detail()
    , m_cursor()
    , m_hasSetCursor(false)
{

}


MyException::MyException(E_ExceptionType errorCode, ChInfo cur)
    : exception()
    , m_errorCode(errorCode)
    , m_detail()
    , m_cursor(cur)
    , m_hasSetCursor(true)
{
}


// MyException::MyException(const  MyException& r)
//     : m_errorCode(r.m_errorCode)
//     , m_detail(r.m_detail)
//     , m_cursor(r.m_cursor)
//     , m_hasSetCursor(r.m_hasSetCursor)
// {
// }



void MyException::setDetail(const std::string& detail)
{
    m_detail = detail;
}


string MyException::getDetail()
{
    return m_detail;
}

string MyException::getExceptionDetail() const
{
    using namespace charUtil;

    string detail;
    auto it = s_exceptionMap.find(m_errorCode);
    if ( it != s_exceptionMap.end() ) {
        detail = it->second;
    } else {
        detail = EnumUtil::enumName(m_errorCode); 
    }

    detail += SPACE_1;
    if ( m_hasSetCursor ) {
        detail += m_cursor.getPos(0);
    }

    if ( !m_detail.empty() ) {
        // detail += SPACE_1;
        detail += m_detail;
    }

    // MyException::s_retDetail = detail;

    // return MyException::s_retDetail.c_str();
    return detail.c_str();
}


E_ExceptionType MyException::getCode()
{
    return m_errorCode;
}


// virtual 
/*
const char* MyException::what() const noexcept // throw()
{
    using namespace charUtil;

    string detail;
    auto it = s_exceptionMap.find(m_errorCode);
    if ( it != s_exceptionMap.end() ) {
        detail = it->second;
    } else {
        detail = EnumUtil::enumName(m_errorCode); 
    }

    detail += SPACE_1;
    if ( m_hasSetCursor ) {
        detail += m_cursor.getPos();
    }

    if ( !m_detail.empty() ) {
        detail += SPACE_1;
        detail += (DOUBLE_QUOTO + m_detail + DOUBLE_QUOTO);
    }

    MyException::s_retDetail = detail;

    return MyException::s_retDetail.c_str();
    // return detail.c_str();
}
*/
