#ifndef COMMON_ENUM_H
#define COMMON_ENUM_H


enum E_ChType
{
  E_LETTER = 0,      // a-z A-Z
  E_NUMBER,          // 0-9
  E_DOT,             // .
  E_UNDERLINE,       // _

  //
  E_ADD,             // +
  E_MINUS,           // -
  E_MULTIPLY,        // *
  E_DIVIDE,          // /
  E_MOD,             // %

  //
  E_BIT_AND,         // &
  E_BIT_OR,          // |
  E_BIT_XOR,         // ^
  E_BIT_NOT,         // ~

  //
  E_LESS_THAN,       // <
  E_GREATER_THAN,    // >

  //
  E_OPEN_PARENTHESES,  // (
  E_CLOSE_PARENTHESES, // )

  //
  E_SEMICOLON,  // ;
  E_ASSIGN,     // =

  // 
  E_SPACE,
  E_TAB,
  E_NEW_LINE_R, // \r
  E_NEW_LINE_N, // \n

  E_SIGN_POSITIVE, // +
  E_SIGN_NEGATIVE, // -

  E_PREFIX,   //    0 or 0x or 0X
  E_SUFFIX,   //    u U ul Ul uL UL

  E_COMMENT_CHAR_1, //     '/'
  E_COMMENT_CHAR_2, //     '*'

};



enum E_PaserType
{

  E_UNDETERMIND = 0,

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
  E_P_SINGLE_LINE_COMMENT,
  E_P_MULTI_LINE_COMMENT,
  //
  E_P_BLANK
};


enum E_ExceptionCode
{
	E_UNKNOWN_CHAR = 0,

};


#endif
