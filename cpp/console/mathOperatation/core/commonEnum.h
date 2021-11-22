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
  E_P_ENDLESEE_SEMICOLON = 11, // To Do Calculation

};


enum E_TokenType
{
	E_TOKEN_UNKNOWN = 0,

	E_TOKEN_DECIMAL_NUMBER = 1,
	E_TOKEN_OCTAL_NUMBER = 2,
	E_TOKEN_HEX_NUMBER = 3,

	E_TOKEN_FLOAT_NUMBER = 4,

	E_TOKEN_VARIBLE = 5,

	E_TOKEN_OPERATOR = 6,
	/*
	   Sub Operator Type
    */
	//////////////////////////////////////////////////
	E_TOKEN_OP_OPEN_PARENTHESES = 7,  // (
	E_TOKEN_OP_BEGIN = E_TOKEN_OPERATOR,      // 7
	E_TOKEN_OP_CLOSE_PARENTHESES = 8, // )

	E_TOKEN_OP_ADD = 9,                // +
	E_TOKEN_OP_MINUS = 10,             // -
	E_TOKEN_OP_MULTIPLY = 11,          // *
	E_TOKEN_OP_DIVIDE = 12,            // /
	E_TOKEN_OP_MOD = 13,               // %
	E_TOKEN_OP_BIT_AND = 14,           // &
	E_TOKEN_OP_BIT_OR = 15,            // |
	E_TOKEN_OP_BIT_XOR = 16,           // ^
	E_TOKEN_OP_BIT_NOT = 17,           // ~
	E_TOKEN_OP_BIT_LEFT_SHIFT = 18,    // <<
	E_TOKEN_OP_BIT_RIGHT_SHIFT = 19,   // >>
	E_TOKEN_OP_ASSIGNMENT = 20,        // =

	E_TOKEN_OP_END = E_TOKEN_OP_ASSIGNMENT, // 20
	//////////////////////////////////////////////////

	E_TOKEN_COMMENT_TYPE = 21,
	E_TOKEN_SINGLE_COMMENT = 22,    //    //
	E_TOKEN_MULTI_COMMENT = 23,     //    /*   */

	E_TOKEN_BLANK = 24,  // <Space> <Tab> \r \n

	E_TOKEN_SEMICOLON = 25,

	E_TOKEN_IGNORE, // Placehold Dummy Token Type
};



enum E_ExceptionCode
{
	E_UNKNOWN_CHAR = 0,

};

enum E_DataType
{
    //////////////////////////////////////////////////
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
