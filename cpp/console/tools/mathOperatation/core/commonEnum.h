#ifndef COMMON_ENUM_H
#define COMMON_ENUM_H

enum E_CharType
{
	E_CHTYPE_ALPHA,    // a-z A-Z
	E_CHTYPE_NUMBER,   // 0-9
	E_CHTYPE_OPERATOR, // +  -  *  /  %  &  |  ^  ~   <   >   (  )  =
	E_CHTYPE_BLANK,    // <Space>   <Tab>   <\r>    <\n>
	E_CHTYPE_OTHER,    // _    .   ;
};



enum E_PaserType
{
  E_P_DEFAULT = 1,
  //
  E_P_DECIMAL = 2,
  E_P_OCTAL = 3,
  E_P_HEX = 4,
  //
  E_P_FLOAT = 5,
  //
  E_P_VARIBLE = 6,
  //
  E_P_OPERATOR = 7,
  //
  E_P_SINGLE_LINE_COMMENT = 8,
  E_P_MULTI_LINE_COMMENT = 9,
  //
  E_P_BLANK = 10,
  //
  E_P_ENDLESS_SEMICOLON = 11, // To Do Calculation

};


enum E_TokenType
{
	E_TOKEN_UNKNOWN = 0,

	E_TOKEN_INTEGER_NUMBER = 1,
	  E_TOKEN_DECIMAL_NUMBER = 2,
	  E_TOKEN_OCTAL_NUMBER = 3,
	  E_TOKEN_HEX_NUMBER = 4,

	E_TOKEN_FLOAT_NUMBER = 5,

	E_TOKEN_VARIBLE = 6,

	E_TOKEN_OPERATOR = 7,
	/*
	   Sub Operator Type
    */
	//////////////////////////////////////////////////
	E_TOKEN_OP_BEGIN = E_TOKEN_OPERATOR,      // 7
	E_TOKEN_OP_OPEN_PARENTHESES = 8,  // (
	E_TOKEN_OP_CLOSE_PARENTHESES = 9, // )

	E_TOKEN_OP_ADD = 10,               // +
     E_TOKEN_OP_POSITIVE = 11,          // +3
	E_TOKEN_OP_MINUS = 12,             // -
     E_TOKEN_OP_NEGATIVE = 13,          // -3

	E_TOKEN_OP_MULTIPLY = 14,          // *
	E_TOKEN_OP_DIVIDE = 15,            // /
	E_TOKEN_OP_MOD = 16,               // %
	E_TOKEN_OP_BIT_AND = 17,           // &
	E_TOKEN_OP_BIT_OR = 18,            // |
	E_TOKEN_OP_BIT_XOR = 19,           // ^
	E_TOKEN_OP_BIT_NOT = 20,           // ~
	E_TOKEN_OP_BIT_LEFT_SHIFT = 21,    // <<
	E_TOKEN_OP_BIT_RIGHT_SHIFT = 22,   // >>
	E_TOKEN_OP_ASSIGNMENT = 23,        // =

	E_TOKEN_OP_END = E_TOKEN_OP_ASSIGNMENT, // 20
	//////////////////////////////////////////////////

	E_TOKEN_COMMENT_TYPE = 24,
	E_TOKEN_SINGLE_COMMENT = 25,    //    //
	E_TOKEN_MULTI_COMMENT = 26,     //    /*   */

	E_TOKEN_BLANK = 27,  // <Space> <Tab> \r \n

	E_TOKEN_SEMICOLON = 28,

	E_TOKEN_IGNORE, // Placehold Dummy Token Type
};



enum E_ExceptionCode
{
	E_UNKNOWN_CHAR = 0,

	E_BLANK_INVALID_FORMAT = 1,

	E_MULTI_LINE_COMMENT_INVALID_FORMAT = 2,
	E_SINGLE_LINE_COMMENT_INVALID_FORMAT = 3,

	E_DECIMAL_INVALID_FORMAT = 4,
	E_OCT_INVALID_FORMAT     = 5,
	E_HEX_INVALID_FORMAT     = 6,

	E_FLOAT_INVALID_FORMAT = 7,

	E_OPERATOR_INVALID_FORMAT = 8,

	E_VARIBLE_INVALID_FORMAT = 9,

	E_EXP_EVALUATION_INVALID_FORMAT = 10,

	E_TOKEN_LOGIC_INVALID = 11,

    E_TOKEN_CLOSE_PARENTHESES_MISMATCHED = 12,
    E_TOKEN_OPEN_PARENTHESES_MISMATCHED  = 13,

    E_INVALID_NUMBER_BASE = 14,

    E_INVALID_OCTAL_NUMBER = 15,
    E_INVALID_DECIMAL_NUMBER = 16,
    E_INVALID_HEX_NUMBER = 17,

    E_DIFFERENT_NUMBER_BASE = 18,

    E_VARIBLE_NAME_CANNOT_BE_KEYWORD = 19,

    E_VARIBLE_NAME_ALREADY_DECLARED = 20,


    E_SWITCH_CASE_DEFAULT_FLOW = 21,


};

enum E_DataType
{
    E_TP_UNKNOWN,
    //////////////////////////////////////////////////
    E_TP_CHAR,
	E_TP_U_CHAR,
	E_TP_S_CHAR,

	E_TP_U_SHORT,
	E_TP_S_SHORT,

	E_TP_U_INT,
	E_TP_S_INT,

	E_TP_U_LONG,
	E_TP_S_LONG,

	E_TP_U_LONG_LONG,
	E_TP_S_LONG_LONG,
    //////////////////////////////////////////////////

	E_TP_FLOAT,
	E_TP_DOUBLE,
	E_TP_LONG_DOUBLE,
};

// Common Convert Marco
#ifdef USE_INT_INSTEAD_OF_ENUM
    #define enumCvt(enumVar)  ( static_cast<int>(enumVar) )
#else
    #define enumCvt(enumVar)  ( enumVar )
#endif


#endif