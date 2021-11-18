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
  E_P_DEFAULT,
  //
  E_P_DECIMAL,
  E_P_OCTAL,
  E_P_HEX,
  //
  E_P_FLOAT,
  //
  E_P_VARIBLE,
  //
  E_P_OPERATOR,
  //
  E_P_SINGLE_LINE_COMMENT,
  E_P_MULTI_LINE_COMMENT,
  //
  E_P_BLANK,
  //
  E_P_ENDLESEE_SEMICOLON, // To Do Calculation

};


enum E_TokenType
{
	E_TOKEN_UNKNOWN,

	E_TOKEN_DECIMAL_NUMBER,
	E_TOKEN_OCTAL_NUMBER,
	E_TOKEN_HEX_NUMBER,

	E_TOKEN_FLOAT_NUMBER,

	E_TOKEN_VARIBLE,

	E_TOKEN_OPERATOR,
	/*
	   Sub Operator Type
    */
	//////////////////////////////////////////////////
	E_TOKEN_OP_OPEN_PARENTHESES,  // (
	E_TOKEN_OP_CLOSE_PARENTHESES, // )

	E_TOKEN_OP_ADD,               // +
	E_TOKEN_OP_MINUS,             // -
	E_TOKEN_OP_MULTIPLY,          // *
	E_TOKEN_OP_DIVIDE,            // /
	E_TOKEN_OP_MOD,               // %
	E_TOKEN_OP_BIT_AND,           // &
	E_TOKEN_OP_BIT_OR,            // |
	E_TOKEN_OP_BIT_XOR,           // ^
	E_TOKEN_OP_BIT_NOT,           // ~
	E_TOKEN_OP_BIT_LEFT_SHIFT,    // <<
	E_TOKEN_OP_BIT_RIGHT_SHIFT,   // >>
	E_TOKEN_OP_ASSIGNMENT,        // =
	//////////////////////////////////////////////////

	E_TOKEN_SINGLE_COMMENT,    //    //
	E_TOKEN_MULTI_COMMENT,     //    /*   */

	E_TOKEN_BLANK,  // <Space> <Tab> \r \n

	E_TOKEN_SEMICOLON,

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


#endif
