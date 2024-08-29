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
    E_OPERATOR_UNKNOWN = -1,

/*0*/     E_ADD = 0,            // +  , e.g.   a + b
/*1*/     E_MINUS,              // -  , e.g.   a - b
/*2*/     E_MULTIPLY,           // *
/*3*/     E_DIVIDE,             // /
/*4*/     E_MOD,                // %

/*5*/     E_BIT_AND,            // &
/*6*/     E_BIT_OR,             // |
/*7*/     E_BIT_XOR,            // ^
/*8*/     E_BIT_NOT,            // ~
/*9*/     E_BIT_LEFT_SHIFT,     // <<
/*10*/    E_BIT_RIGHT_SHIFT,    // >>

/*11*/    E_OPEN_PARENTHESIS,   // (
/*12*/    E_CLOSE_PARENTHESIS,  // )
                          
/*13*/    E_ASSIGNMENT,                     // =
/*14*/    E_ADD_ASSIGNMENT,                 // +=
/*15*/    E_MINUS_ASSIGNMENT,               // -=
/*16*/    E_MULTIPLY_ASSIGNMENT,            // *=
/*17*/    E_DIVIDE_ASSIGNMENT,              // /=
/*18*/    E_MOD_ASSIGNMENT,                 // %=
/*19*/    E_BIT_AND_ASSIGNMENT,             // &=
/*20*/    E_BIT_OR_ASSIGNMENT,              // |=
/*21*/    E_BIT_XOR_ASSIGNMENT,             // ^=
/*22*/    E_BIT_LEFT_SHIFT_ASSIGNMENT,      // <<=
/*23*/    E_BIT_RIGHT_SHIFT_ASSIGNMENT,     // >>=

     // new feature for function call
/*24*/    E_COMMA,                          // ,

        E_POSITIVE, // +  , e.g.   +a
        E_NEGATIVE, // -  , e.g.   -a

        E_FORCE_TYPE_CAST,  // int a = (int)( 3.14f  * 2.5 ); for   force type cast     new feature

    // E_OPERATOR_UNKNOWN 

};

enum E_OpAnotherRoleFlag
{
    E_OP_FLAG_UNKNOWN = 0,

    E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE,        // a = ( b + c ) * d
                                                        //     ^
    E_OP_FLAG_OPEN_PARENTHESIS_FUNCTION_START,          // a = sin( angle )
                                                        //        ^

    E_OP_FLAG_OPEN_PARENTHESIS_FORCE_TYPE_CAST_START,   // int a = (int)3.14f;
                                                        //         ^

    E_OP_FLAG_OPEN_PARENTHESIS_FORCE_TYPE_CAST_END,     // int a = (int)3.14f;
                                                        //             ^

    E_OP_FLAG_CLOSE_PARENTHESIS_PRIORITY_PREMOTE,       //  a = ( b + c ) * d
                                                        //              ^
    E_OP_FLAG_CLOSE_PARENTHESIS_FUNCTION_END,           //  a = sin( angle ) 
                                                        //                 ^

    E_OP_COMMA_NORMAL_FOR_COMMA_EXPRESSION,             //  a = 3 , 5
                                                        //        ^
    E_OP_COMMA_FUNCTION_ARG_SEP,                        // function( a , b )
                                                        //             ^
    E_OP_COMMA_DEFINATION_SEP                           //   int a , b , c
                                                        //         ^   ^
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

    E_THROW_UNSUPPORTED_FEATURE_LESS_THAN,        // << : OK ,  less than ( a < b ) is an unsupported feature
    E_THROW_UNSUPPORTED_FEATURE_GREATER_THAN,     // >> : OK ,  greater than ( a > b ) is an unsupported feature

    E_THROW_UNSUPPORTED_FEATURE_NO_GREATER_THAN,  // << : OK ,  No Greater Than ( a <= b ) is an unsupported feature
    E_THROW_UNSUPPORTED_FEATURE_NO_LESS_THAN,     // >> : OK ,  No Less Than ( a >= b ) is an unsupported feature

    E_THROW_UNSUPPORTED_FEATURE_EQUAL_TO,        // == is an unsupported feature

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
    E_THROW_VARIBLE_IS_MISSING,

    E_THROW_SENTENCE_LESS_TOKEN,
    E_THROW_SENTENCE_MORE_TOKEN,

    E_THROW_SENTENCE_PREFIX_TOKENS_ARE_INVALID,
    E_THROW_SENTENCE_PREFIX_TOKENS_BEFORE_EQUAL_ARE_INVALID,

    E_THROW_SENTENCE_DEFINITION_PREFIX_IS_NOT_VARIBLE,
    E_THROW_SENTENCE_DEFINITION_TOO_MANY_KEYWORDS,
    E_THROW_SENTENCE_NO_EXPR_BEFORE_ASSIGNMENT,
    E_THROW_SENTENCE_NO_EXPR_AFTER_ASSIGNMENT,
    E_THROW_SENTENCE_2ND_IS_NOT_A_VARIADIC_ASSIGNMENT,
    E_THROW_SENTENCE_UNKNOWN_DATA_TYPE,
    E_THROW_SENTENCE_TPYE_IS_UNDETERMINDED,

    E_THROW_NO_MATCHED_OPEN_PARENTHESIS,
    E_THROW_NO_MATCHED_CLOSE_PARENTHESIS,
    E_THROW_CANNOT_PUSH_TOKEN_KEYWORD,

    E_THROW_SUFFIXEXPR_BINARY_OP_MISS_TWO_OPERANDS,
    E_THROW_SUFFIXEXPR_UNARY_OP_MISS_ONE_OPERAND,
    E_THROW_SUFFIXEXPR_FINAL_EVALUATE_MUST_LEAVE_ONLY_ONE_ELEMENT,


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

    E_THROW_UNSUPPORTED_FEATURE_INCREASEMENTAL_1, // ++ is not supported in this version
    E_THROW_UNSUPPORTED_FEATURE_DECREASEMENTAL_1, // -- is not supported in this version

    E_THROW_UNSUPPORTED_FEATURE_LOGIC_AND,       // && is not supported in this version
    E_THROW_UNSUPPORTED_FEATURE_LOGIC_OR,        // || is not supported in this version


    E_THROW_FIRST_TOKEN_IS_INVALID,

    // fix a big bug : multi keywords is not allowed at this program
    //     int      :  is OK
    //     short    :  is OK
    //     unsigned int :  run-time error
    E_THROW_ALL_TOKENS_ARE_KEYWORD,
    E_THROW_TOO_MANY_KEYWORDS,
    E_THROW_SENTENCE_DEFINITION_HAS_MISSED_AN_ASSIGNMENT_OPERATOR,


	////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Function Call relative exception
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////
    E_THROW_FUNCTION_OBJECT_IS_NULLPTR,
    E_THROW_CALL_FUNCTION_TOO_MANY_ARGUMENTS,
	E_THROW_CALL_FUNCTION_TOO_LITTLE_ARGUMENTS,

	E_THROW_PARSE_FUNCTION_PUSH_TOO_MANY_ARGUMENTS,
    E_THROW_UNKNOWN_BUILT_IN_FUNCTION_NAME,
    E_THROW_EMPTY_CONTENT_INSIDE_PARENTHESIS_PAIR_IS_NOT_ALLOWED, // none function  ()   is not allowed

	E_THROW_CANNOT_DEFINE_A_VARIBLE_WITH_THE_SAME_NAME_OF_BUILT_IN_FUNCTION,
    E_THROW_FUNCTION_ARGUMENT_CANNOT_BE_EMPTY_WHEN_MEET_COMMA,
    E_THROW_FUNCTION_IS_NOT_VALID_BY_MISSING_CLOSE_PARENTHESIS,

    E_THROW_FORCE_TYPE_CAST_LIST_IS_EMPTY, // for force type cast   for new feature

    E_THROW_CALL_STACK_SHOULD_NEVER_BE_EMPTY,


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
