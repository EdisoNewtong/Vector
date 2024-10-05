#include "enumUtil.h"
#include "myException.h"
using namespace std;

#define INNER_ENUM_2_STRING_MAP(enumTp) case enumTp:\
    retstr = #enumTp;\
    break;

///////////////////////////////////////////////////////////////////////////
//
// static class member function
//
///////////////////////////////////////////////////////////////////////////

string EnumUtil::enumName(E_CharType tp)
{
    string retstr;
    switch( tp )
    {
    INNER_ENUM_2_STRING_MAP(E_CHAR_UNKNOWN)
    INNER_ENUM_2_STRING_MAP(E_CHAR_ALPHA)
    INNER_ENUM_2_STRING_MAP(E_CHAR_NUMBER)
    INNER_ENUM_2_STRING_MAP(E_CHAR_OPERATOR)
    INNER_ENUM_2_STRING_MAP(E_CHAR_BLANK)
    INNER_ENUM_2_STRING_MAP(E_CHAR_MISC)
    default:
         retstr = string("??? E_CharType ??? | value = ") + to_string( static_cast<int>(tp) );
         break;
    }

    return retstr;
}


// static 
string EnumUtil::enumName(E_OperatorType tp)
{
    string retstr;

    switch( tp )
    {
    INNER_ENUM_2_STRING_MAP(E_OPERATOR_UNKNOWN)
    INNER_ENUM_2_STRING_MAP(E_ADD)
    INNER_ENUM_2_STRING_MAP(E_POSITIVE)
    INNER_ENUM_2_STRING_MAP(E_MINUS)
    INNER_ENUM_2_STRING_MAP(E_NEGATIVE)
    INNER_ENUM_2_STRING_MAP(E_MULTIPLY)
    INNER_ENUM_2_STRING_MAP(E_DIVIDE)
    INNER_ENUM_2_STRING_MAP(E_MOD)
    INNER_ENUM_2_STRING_MAP(E_BIT_AND)
    INNER_ENUM_2_STRING_MAP(E_BIT_OR)
    INNER_ENUM_2_STRING_MAP(E_BIT_XOR)
    INNER_ENUM_2_STRING_MAP(E_BIT_NOT)
    INNER_ENUM_2_STRING_MAP(E_BIT_LEFT_SHIFT)
    INNER_ENUM_2_STRING_MAP(E_BIT_RIGHT_SHIFT)
    INNER_ENUM_2_STRING_MAP(E_OPEN_PARENTHESIS)
    INNER_ENUM_2_STRING_MAP(E_CLOSE_PARENTHESIS)
    INNER_ENUM_2_STRING_MAP(E_ASSIGNMENT)
    ///////////////////////////////////////////////////
    INNER_ENUM_2_STRING_MAP(E_ADD_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_MINUS_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_MULTIPLY_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_DIVIDE_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_MOD_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_BIT_AND_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_BIT_OR_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_BIT_XOR_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_BIT_LEFT_SHIFT_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_BIT_RIGHT_SHIFT_ASSIGNMENT)
	// New feature for function call
    INNER_ENUM_2_STRING_MAP(E_COMMA)
	// new feature for force type cast
    INNER_ENUM_2_STRING_MAP(E_FORCE_TYPE_CAST)
    default:
        retstr = string("??? E_OperatorType ??? | value = ") + to_string( static_cast<int>(tp) );
        break;
    }

    return retstr;
}


 
string EnumUtil::enumName(E_TokenType tp)
{
    string retstr;

    switch( tp )
    {
    INNER_ENUM_2_STRING_MAP(E_TOKEN_UNKNOWN)
    INNER_ENUM_2_STRING_MAP(E_TOKEN_BLANK)
    INNER_ENUM_2_STRING_MAP(E_TOKEN_SINGLE_LINE_COMMENT)
    INNER_ENUM_2_STRING_MAP(E_TOKEN_MULTI_LINE_COMMENT)
    INNER_ENUM_2_STRING_MAP(E_TOKEN_EXPRESSION)
    INNER_ENUM_2_STRING_MAP(E_TOKEN_OPERATOR)
    INNER_ENUM_2_STRING_MAP(E_TOKEN_SEMICOLON)
    default:
        retstr = string("??? E_TokenType ??? | value = ") + to_string( static_cast<int>(tp) );
        break;
    }
    return retstr;
}



string EnumUtil::enumName(E_DataType tp)
{
    string retstr;

    switch( tp )
    {
    case E_TP_UNKNOWN:
        retstr = "unknown type";
        break;
    case E_TP_CHAR:
        retstr = "char";
        break;
    case E_TP_U_CHAR:
        retstr = "unsigned char";
        break;
    case E_TP_S_CHAR:
        retstr = "signed char";
        break;
    case E_TP_U_SHORT:
        retstr = "unsigned short";
        break;
    case E_TP_S_SHORT:
        retstr = "short";
        break;
    case E_TP_U_INT:
        retstr = "unsigned int";
        break;
    case E_TP_S_INT:
        retstr = "int";
        break;
    case E_TP_U_LONG:
        retstr = "unsigned long";
        break;
    case E_TP_S_LONG:
        retstr = "long";
        break;
    case E_TP_U_LONG_LONG:
        retstr = "unsigned long long";
        break;
    case E_TP_S_LONG_LONG:
        retstr = "long long";
        break;
    case E_TP_FLOAT:
        retstr = "float";
        break;
    case E_TP_DOUBLE:
        retstr = "double";
        break;


    // INNER_ENUM_2_STRING_MAP(E_TP_UNKNOWN)
    // INNER_ENUM_2_STRING_MAP(E_TP_CHAR)
    // INNER_ENUM_2_STRING_MAP(E_TP_U_CHAR)
    // INNER_ENUM_2_STRING_MAP(E_TP_S_CHAR)
    // INNER_ENUM_2_STRING_MAP(E_TP_U_SHORT)
    // INNER_ENUM_2_STRING_MAP(E_TP_S_SHORT)
    // INNER_ENUM_2_STRING_MAP(E_TP_U_INT)
    // INNER_ENUM_2_STRING_MAP(E_TP_S_INT)
    // INNER_ENUM_2_STRING_MAP(E_TP_U_LONG)
    // INNER_ENUM_2_STRING_MAP(E_TP_S_LONG)
    // INNER_ENUM_2_STRING_MAP(E_TP_U_LONG_LONG)
    // INNER_ENUM_2_STRING_MAP(E_TP_S_LONG_LONG)
    // INNER_ENUM_2_STRING_MAP(E_TP_FLOAT)
    // INNER_ENUM_2_STRING_MAP(E_TP_DOUBLE)
    // INNER_ENUM_2_STRING_MAP(E_TP_LONG_DOUBLE)
    default:
        retstr = string("??? E_DataType ??? | value = ") + to_string( static_cast<int>(tp) );
        break;
    }

    return retstr;
}



string EnumUtil::enumName(E_ParserAction tp)
{
    string retstr;

    switch(tp)
    {
    INNER_ENUM_2_STRING_MAP(E_NO_ACTION)
    INNER_ENUM_2_STRING_MAP(E_SWITCH_FROM_BASE_TO_SPECIAL_PARSER)
    INNER_ENUM_2_STRING_MAP(E_TRANSFER_TO_OTHER)
    INNER_ENUM_2_STRING_MAP(E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR)
    default:
        retstr = string("??? E_ParserAction ??? | value = ") + to_string( static_cast<int>(tp) );
        break;
    }

    return retstr;
}

// static 
string EnumUtil::enumName(E_ExceptionType tp)
{
    string retstr;
    switch( tp )
    {
    INNER_ENUM_2_STRING_MAP(E_THROW_UNKNOWN_CHAR)
    INNER_ENUM_2_STRING_MAP(E_THROW_CODE_CANNOT_REACH_HERE)
    INNER_ENUM_2_STRING_MAP(E_THROW_NULL_PTR)

    INNER_ENUM_2_STRING_MAP(E_THROW_UNSUPPORTED_FEATURE_LESS_THAN)
    INNER_ENUM_2_STRING_MAP(E_THROW_UNSUPPORTED_FEATURE_GREATER_THAN)

    INNER_ENUM_2_STRING_MAP(E_THROW_UNSUPPORTED_FEATURE_NO_GREATER_THAN)
    INNER_ENUM_2_STRING_MAP(E_THROW_UNSUPPORTED_FEATURE_NO_LESS_THAN)

    INNER_ENUM_2_STRING_MAP(E_THROW_UNSUPPORTED_FEATURE_EQUAL_TO)

    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_PARSEDSTR_TO_GENERATE_TOKEN)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_CHAR_IN_DECIMAL_SEQ)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_CHAR_IN_OCTAL_SEQ)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_CHAR_IN_HEX_SEQ)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_CHAR_IN_FLOAT_SEQ)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_CHAR_IN_VARIBLE_SEQ)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_NUMBER_BASE)
    INNER_ENUM_2_STRING_MAP(E_THROW_DIFFERENT_NUMBER_BASE)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_DECIMAL_NUMBER)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_OCTAL_NUMBER)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_HEX_NUMBER)
    //
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_LONG_NUMBER_WHEN_CONVERTING)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_UNSIGNED_LONG_NUMBER_WHEN_CONVERTING)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_LONG_LONG_NUMBER_WHEN_CONVERTING)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_UNSIGNED_LONG_LONG_NUMBER_WHEN_CONVERTING)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_FLOAT_NUMBER_WHEN_CONVERTING)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_DOUBLE_NUMBER_WHEN_CONVERTING)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_LONG_DOUBLE_NUMBER_WHEN_CONVERTING)
    //
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_OPERATOR)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_TOKEN_RELATIONSHIP)

    INNER_ENUM_2_STRING_MAP(E_THROW_VARIBLE_ALREADY_DEFINED)
    INNER_ENUM_2_STRING_MAP(E_THROW_VARIBLE_CANNOT_BE_KEYWORD)
    INNER_ENUM_2_STRING_MAP(E_THROW_VARIBLE_NOT_DEFINED)
    INNER_ENUM_2_STRING_MAP(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED)
    INNER_ENUM_2_STRING_MAP(E_THROW_VARIBLE_IS_MISSING)

    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_LESS_TOKEN)
    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_MORE_TOKEN)

    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_PREFIX_TOKENS_ARE_INVALID)
    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_PREFIX_TOKENS_BEFORE_EQUAL_ARE_INVALID)
    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_DEFINITION_PREFIX_IS_NOT_VARIBLE)
    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_2ND_IS_NOT_A_VARIADIC_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_DEFINITION_TOO_MANY_KEYWORDS)
    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_NO_EXPR_BEFORE_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_NO_EXPR_AFTER_ASSIGNMENT)
    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_UNKNOWN_DATA_TYPE)
    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_TPYE_IS_UNDETERMINDED)

    INNER_ENUM_2_STRING_MAP(E_THROW_NO_MATCHED_OPEN_PARENTHESIS)
    INNER_ENUM_2_STRING_MAP(E_THROW_NO_MATCHED_CLOSE_PARENTHESIS)
    INNER_ENUM_2_STRING_MAP(E_THROW_CANNOT_PUSH_TOKEN_KEYWORD)
    
    INNER_ENUM_2_STRING_MAP(E_THROW_SUFFIXEXPR_BINARY_OP_MISS_TWO_OPERANDS)
    INNER_ENUM_2_STRING_MAP(E_THROW_SUFFIXEXPR_UNARY_OP_MISS_ONE_OPERAND)
    INNER_ENUM_2_STRING_MAP(E_THROW_SUFFIXEXPR_FINAL_EVALUATE_MUST_LEAVE_ONLY_ONE_ELEMENT)

    INNER_ENUM_2_STRING_MAP(E_THROW_DIVIDE_ZERO)
    INNER_ENUM_2_STRING_MAP(E_THROW_MODULO_ZERO)
    INNER_ENUM_2_STRING_MAP(E_THROW_MODULO_CANNOT_APPLY_ON_FLOAT)

    INNER_ENUM_2_STRING_MAP(E_THROW_BITAND_CANNOT_APPLY_ON_FLOAT)
    INNER_ENUM_2_STRING_MAP(E_THROW_BITOR_CANNOT_APPLY_ON_FLOAT)
    INNER_ENUM_2_STRING_MAP(E_THROW_BITXOR_CANNOT_APPLY_ON_FLOAT)
    INNER_ENUM_2_STRING_MAP(E_THROW_BITNOT_CANNOT_APPLY_ON_FLOAT)

    INNER_ENUM_2_STRING_MAP(E_THROW_BIT_LEFTSHIFT_CANNOT_APPLY_ON_FLOAT)
    INNER_ENUM_2_STRING_MAP(E_THROW_BIT_RIGHTSHIFT_CANNOT_APPLY_ON_FLOAT)
    INNER_ENUM_2_STRING_MAP(E_THROW_INVALID_SUFFIX_EXPRESSION)
    INNER_ENUM_2_STRING_MAP(E_THROW_LAST_VALID_TOKEN_IS_NOT_SEMICOLON)

    INNER_ENUM_2_STRING_MAP(E_THROW_UNSUPPORTED_FEATURE_INCREASEMENTAL_1)   // ++ is not supported
    INNER_ENUM_2_STRING_MAP(E_THROW_UNSUPPORTED_FEATURE_DECREASEMENTAL_1)   // -- is not supported

    INNER_ENUM_2_STRING_MAP(E_THROW_UNSUPPORTED_FEATURE_LOGIC_AND)       // && is not supported in this version
    INNER_ENUM_2_STRING_MAP(E_THROW_UNSUPPORTED_FEATURE_LOGIC_OR)        // || is not supported in this version

    INNER_ENUM_2_STRING_MAP(E_THROW_FIRST_TOKEN_IS_INVALID)   

    INNER_ENUM_2_STRING_MAP(E_THROW_ALL_TOKENS_ARE_KEYWORD)
    INNER_ENUM_2_STRING_MAP(E_THROW_TOO_MANY_KEYWORDS)
    INNER_ENUM_2_STRING_MAP(E_THROW_SENTENCE_DEFINITION_HAS_MISSED_AN_ASSIGNMENT_OPERATOR)

    INNER_ENUM_2_STRING_MAP(E_THROW_FUNCTION_OBJECT_IS_NULLPTR)
    INNER_ENUM_2_STRING_MAP(E_THROW_CALL_FUNCTION_TOO_MANY_ARGUMENTS)
    INNER_ENUM_2_STRING_MAP(E_THROW_CALL_FUNCTION_TOO_LITTLE_ARGUMENTS)
    INNER_ENUM_2_STRING_MAP(E_THROW_PARSE_FUNCTION_PUSH_TOO_MANY_ARGUMENTS)
    INNER_ENUM_2_STRING_MAP(E_THROW_UNKNOWN_BUILT_IN_FUNCTION_NAME)
    INNER_ENUM_2_STRING_MAP(E_THROW_EMPTY_CONTENT_INSIDE_PARENTHESIS_PAIR_IS_NOT_ALLOWED)

    INNER_ENUM_2_STRING_MAP(E_THROW_CANNOT_DEFINE_A_VARIBLE_WITH_THE_SAME_NAME_OF_BUILT_IN_FUNCTION)
    INNER_ENUM_2_STRING_MAP(E_THROW_FUNCTION_ARGUMENT_CANNOT_BE_EMPTY_WHEN_MEET_COMMA)
    INNER_ENUM_2_STRING_MAP(E_THROW_FUNCTION_IS_NOT_VALID_BY_MISSING_CLOSE_PARENTHESIS)
    INNER_ENUM_2_STRING_MAP(E_THROW_CALL_STACK_SHOULD_NEVER_BE_EMPTY)

    default:
        retstr = string("??? E_ExceptionType ??? | value = ") + to_string( static_cast<int>(tp) );
        break;
    }

    return retstr;
}

 
string EnumUtil::enumName(ParserBase::E_PARSER_TYPE tp)
{

    string retstr;
    switch( tp )
    {
    case ParserBase::E_BASE:
        retstr = "ParserBase";
        break;
    case ParserBase::E_BLANK:
        retstr = "BlankParser";
        break;
    case ParserBase::E_SINGLE_LINE_COMMENT:
        retstr = "SinglelineCommentParser";
        break;
    case ParserBase::E_MULTI_LINE_COMMENT:
        retstr = "MultilineCommentParser";
        break;
    case ParserBase::E_OPERATOR:
        retstr = "OperatorParser";
        break;
    case ParserBase::E_SEQUENCE:
        retstr = "SequenceParser";
        break;
    case ParserBase::E_SEMICOLON:
        retstr = "SemiColonParser";
        break;
    default:
        retstr = string("???  ParserBase::E_PARSER_TYPE ??? | value = ") + to_string( static_cast<int>(tp) );
        break;
    }

    return retstr;
}


// static 
E_OperatorType EnumUtil::getOpType(const string& opSepStr, bool isUnary /* = false*/ )
{
    int  lenOfOp = static_cast<int>( opSepStr.size() );
    char firstCh = opSepStr.at(0);
    E_OperatorType retOpTp = E_OPERATOR_UNKNOWN;
    if ( lenOfOp == 1 ) {
        switch ( firstCh )
        {
        case '+':
            retOpTp = (!isUnary ?  E_ADD : E_POSITIVE);
            break;
        case '-':
            retOpTp = (!isUnary ?  E_MINUS : E_NEGATIVE);
            break;
        case '*':
            retOpTp = E_MULTIPLY;
            break;
        case '/':
            retOpTp = E_DIVIDE;
            break;
        case '%':
            retOpTp = E_MOD;
            break;
        case '&':
            retOpTp = E_BIT_AND;
            break;
        case '|':
            retOpTp = E_BIT_OR;
            break;
        case '^':
            retOpTp = E_BIT_XOR;
            break;
        case '~':
            retOpTp = E_BIT_NOT;
            break;
        case '(':
            retOpTp = E_OPEN_PARENTHESIS;
            break;
        case ')':
            retOpTp = E_CLOSE_PARENTHESIS;
            break;
        case '=':
            retOpTp = E_ASSIGNMENT;
            break;
        case ',':
            retOpTp = E_COMMA;
            break;
        default:
            {
                MyException e(E_THROW_INVALID_OPERATOR);
                e.setDetail( string(1, firstCh) );
                throw e;
            }
            break;
        }
    } else if ( lenOfOp == 2 ) {
        char secondCh = opSepStr.at(1);
        switch( secondCh )
        {
        case '=':
            {
                switch( firstCh )
                {
                case '+':
                    retOpTp = E_ADD_ASSIGNMENT;
                    break;
                case '-':
                    retOpTp = E_MINUS_ASSIGNMENT;
                    break;
                case '*':
                    retOpTp = E_MULTIPLY_ASSIGNMENT;
                    break;
                case '/':
                    retOpTp = E_DIVIDE_ASSIGNMENT;
                    break;
                case '%':
                    retOpTp = E_MOD_ASSIGNMENT;
                    break;
                case '&':
                    retOpTp = E_BIT_AND_ASSIGNMENT;
                    break;
                case '|':
                    retOpTp = E_BIT_OR_ASSIGNMENT;
                    break;
                case '^':
                    retOpTp = E_BIT_XOR_ASSIGNMENT;
                    break;
                default:
                    {
                        MyException e(E_THROW_INVALID_OPERATOR);
                        e.setDetail( opSepStr );
                        throw e;
                    }
                    break;
                }
            }
            break;
        case '<':
            retOpTp = E_BIT_LEFT_SHIFT;
            break;
        case '>':
            retOpTp = E_BIT_RIGHT_SHIFT;
            break;
        default:
            {
                MyException e(E_THROW_INVALID_OPERATOR);
                e.setDetail( opSepStr );
                throw e;
            }
            break;
        }
    } else if ( lenOfOp == 3 ) {
        if ( opSepStr == "<<=" ) {
            retOpTp = E_BIT_LEFT_SHIFT_ASSIGNMENT;
        } else if ( opSepStr == ">>=" ) {
            retOpTp = E_BIT_RIGHT_SHIFT_ASSIGNMENT;
        }
    }

    return retOpTp;
}
