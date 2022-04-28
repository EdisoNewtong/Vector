#ifndef COMMON_ENUM_H
#define COMMON_ENUM_H

enum E_CharType
{
    E_CHAR_UNKNOWN = 0,

    E_CHAR_ALPHA,    // a-z A-Z  
    E_CHAR_NUMBER,   // 0-9
    E_CHAR_OPERATOR, // + - * / % & | ^ ~       "<<"   ">>"   (   )   =
    E_CHAR_BLANK,    // <Space>  <Tab>   <\r>   <\n>  '\v'
    E_CHAR_MISC      // _   .   ;
};


enum E_OperatorType
{
    E_OPERATOR_UNKNOWN = 0,

    E_ADD,      // +  , e.g.   a + b
     E_POSITIVE, // +  , e.g.   +a
    E_MINUS,    // -  , e.g.   a - b
     E_NEGATIVE, // -  , e.g.   -a
    E_MULTIPLY, // *
    E_DIVIDE,   // /
    E_MOD,      // %

    E_BIT_AND,  // &
    E_BIT_OR,   // |
    E_BIT_XOR,  // ^
    E_BIT_NOT,  // ~
    E_BIT_LEFT_SHIFT,     // <<
    E_BIT_RIGHT_SHIFT,    // >>

    E_OPEN_PARENTHESES,   // (
    E_CLOSE_PARENTHESES,  // )
    E_ASSIGNMENT,         // =

};

enum E_TokenType
{
    E_TOKEN_UNKNOWN = 0,
    //##################################################################################################
    E_TOKEN_BLANK,

    E_TOKEN_SINGLE_LINE_COMMENT,
    E_TOKEN_MULTI_LINE_COMMENT,

    //////////////////////////////////////////////////
    E_TOKEN_EXPRESSION,

        //////////////////////////////////////////////////
        //
        // E_TOKEN_INT_DECIMAL,
        // E_TOKEN_INT_OCTAL,
        // E_TOKEN_INT_HEX,

        // E_TOKEN_FLOAT,
        // E_TOKEN_DOUBLE,

        // E_TOKEN_VARIBLE,
        //
        //////////////////////////////////////////////////

    E_TOKEN_OPERATOR,
    //////////////////////////////////////////////////

    // E_TOKEN_VARIBLE,

    E_TOKEN_SEMICOLON,
    //##################################################################################################

};


enum E_DataType
{
    E_TP_UNKNOWN,
    //////////////////////////////////////////////////
    E_TP_CHAR,    //          char
    E_TP_U_CHAR,  // unsigned char
    E_TP_S_CHAR,  //   signed char

    E_TP_U_SHORT, // unsigned short
    E_TP_S_SHORT, //   signed short

    E_TP_U_INT,   // unsigned int
    E_TP_S_INT,   //   signed int

    E_TP_U_LONG,  // unsigned long
    E_TP_S_LONG,  //   signed long

    E_TP_U_LONG_LONG, // unsigned long long
    E_TP_S_LONG_LONG, //   signed long long
    //////////////////////////////////////////////////

    E_TP_FLOAT,       // float
    E_TP_DOUBLE,      // double
};

enum E_ExceptionType
{
    E_THROW_UNKNOWN_CHAR = 0,

    E_THROW_CODE_CANNOT_REACH_HERE,
    E_THROW_NULL_PTR,

    E_THROW_INVALID_CHAR_AFTER_LEFT_BRACKET,      // << : OK ,  <a  , a is not allowed
    E_THROW_INVALID_CHAR_AFTER_RIGHT_BRACKET,     // >> : OK ,  >a  , a is not allowed

    //////////////////////////////////////////////////
    E_THROW_INVALID_PARSEDSTR_TO_GENERATE_TOKEN,

    E_THROW_INVALID_CHAR_IN_DECIMAL_SEQ,
    E_THROW_INVALID_CHAR_IN_OCTAL_SEQ,
    E_THROW_INVALID_CHAR_IN_HEX_SEQ,
    E_THROW_INVALID_CHAR_IN_FLOAT_SEQ,

    E_THROW_INVALID_CHAR_IN_VARIBLE_SEQ,

    //////////////////////////////////////////////////

    E_THROW_INVALID_NUMBER_BASE,
    E_THROW_DIFFERENT_NUMBER_BASE,  // To make it detail
    E_THROW_INVALID_DECIMAL_NUMBER, // To make it detail
    E_THROW_INVALID_OCTAL_NUMBER,   // To make it detail
    E_THROW_INVALID_HEX_NUMBER,     // To make it detail

    E_THROW_INVALID_LONG_NUMBER_WHEN_CONVERTING,
    E_THROW_INVALID_UNSIGNED_LONG_NUMBER_WHEN_CONVERTING,
    E_THROW_INVALID_LONG_LONG_NUMBER_WHEN_CONVERTING,
    E_THROW_INVALID_UNSIGNED_LONG_LONG_NUMBER_WHEN_CONVERTING,

    E_THROW_INVALID_FLOAT_NUMBER_WHEN_CONVERTING,
    E_THROW_INVALID_DOUBLE_NUMBER_WHEN_CONVERTING,
    E_THROW_INVALID_LONG_DOUBLE_NUMBER_WHEN_CONVERTING,

    //////////////////////////////////////////////////

    E_THROW_INVALID_OPERATOR,

    E_THROW_INVALID_TOKEN_RELATIONSHIP,

    E_THROW_VARIBLE_ALREADY_DEFINED,
    E_THROW_VARIBLE_CANNOT_BE_KEYWORD,
    E_THROW_VARIBLE_NOT_DEFINED,
    E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED,

    E_THROW_SENTENCE_TOO_LESS_TOKEN,
    E_THROW_SENTENCE_TOO_MORE_ASSIGNMENT,
    E_THROW_SENTENCE_DEFINITION_PREFIX_IS_NOT_ALL_KEYWORD,
    E_THROW_SENTENCE_DEFINITION_SUFFIX_IS_NOT_VARIBLE,
    E_THROW_SENTENCE_DEFINITION_TOO_MANY_KEYWORDS,
    E_THROW_SENTENCE_NO_EXPR_BEFORE_ASSIGNMENT,
    E_THROW_SENTENCE_NO_EXPR_AFTER_ASSIGNMENT,
    E_THROW_SENTENCE_UNKNOWN_DATA_TYPE,
    E_THROW_NO_MATCHED_OPEN_PARENTHESES,
    E_THROW_CANNOT_PUSH_TOKEN_KEYWORD,

    E_THROW_SUFFIXEXPR_BINARY_OP_MISS_TWO_OPERANDS,
    E_THROW_SUFFIXEXPR_UNARY_OP_MISS_ONE_OPERAND,

    E_THROW_DIVIDE_ZERO,
    E_THROW_MODULO_ZERO,
    E_THROW_MODULO_CANNOT_APPLY_ON_FLOAT,

    E_THROW_BITAND_CANNOT_APPLY_ON_FLOAT,
    E_THROW_BITOR_CANNOT_APPLY_ON_FLOAT,
    E_THROW_BITXOR_CANNOT_APPLY_ON_FLOAT,
    E_THROW_BITNOT_CANNOT_APPLY_ON_FLOAT,

    E_THROW_BIT_LEFTSHIFT_CANNOT_APPLY_ON_FLOAT,
    E_THROW_BIT_RIGHTSHIFT_CANNOT_APPLY_ON_FLOAT,

    E_THROW_INVALID_SUFFIX_EXPRESSION,
    E_THROW_LAST_VALID_TOKEN_IS_NOT_SEMICOLON,

};

enum E_ParserAction
{
    E_NO_ACTION = 0,

    // Base -> Special
    E_SWITCH_FROM_BASE_TO_SPECIAL_PARSER,

    // e.g   
    //     from  operatorParser : /* //  
    //       to  commentParser
    E_TRANSFER_TO_OTHER,

    // e.g.  
    //       <Tab> <Tab>ab , 
    //                  ^ // BlankParser process 'a' , do not point to 'b' , switch BlankParser -> DefaultParser , DefaultParser to parse 'a'
    E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_KEEP_CURSOR,

    // e.g.   

    //           1. /* .... */a,     when parsing        "*/" , move to next 'a'
    //           2. //       \r\n,   when parsing        "\n" , move to next 'a'
    //                        a
    //                        ^
    //           3. <<a   ,          when parsing the 2nd '<' , move to next 'a'
    //             
    E_GENERATE_TOKEN_SWITCH_TO_DEFAULT_STEP_NEXT,
    
};


// Common Convert Marco
#ifdef USE_INT_INSTEAD_OF_ENUM
    #define enumCvt(enumVar)  ( static_cast<int>(enumVar) )
#else
    #define enumCvt(enumVar)  ( enumVar )
#endif


#define INNER_SAFE_DELETE(ptr) if( ptr != nullptr ) { delete ptr; ptr = nullptr; }

#endif
