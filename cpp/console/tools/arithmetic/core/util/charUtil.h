#ifndef CHAR_UTIL_H
#define CHAR_UTIL_H

#include "commonEnum.h"
#include <string>

namespace charutil
{
    static const char NEW_LINE_N = '\n';
    static const char NEW_LINE_R = '\r';

    inline bool isBlank(const char& ch)
    {
        return (   ch == ' '
                || ch == '\t'
            //  || ch == '\v'
                || ch == NEW_LINE_R
                || ch == NEW_LINE_N);
    }

    inline bool isUnderline(const char& ch)
    {
        return ch == '_';
    }

    inline bool isNumber(const char& ch)
    {
        return (ch >= '0' && ch <= '9');
    }

    inline bool isAlpha(const char& ch)
    {
        return    (ch>='a' && ch<='z')
               || (ch>='A' && ch<='Z');
    }

    inline bool isHexCode(const char& ch)
    {

        return      isNumber(ch)
                 || (ch>='a' && ch<='f')
                 || (ch>='A' && ch<='F');
    }

    inline bool isOctCode(const char& ch)
    {
        return (ch >= '0' && ch <= '7');
    }

    inline bool isVaribleChar(const char& ch)
    {
        return isUnderline(ch) || isNumber(ch) || isAlpha(ch);
    }

    inline bool isVaribleChar1st(const char& ch)
    {
        return isUnderline(ch) || isAlpha(ch);
    }

    inline bool isOperator(const char& ch)
    {
        return (     ch == '+'
                  || ch == '-'
                  || ch == '*'
                  || ch == '/'
                  || ch == '%'
                  /////////////////////////
                  || ch == '&'
                  || ch == '|'
                  || ch == '^'
                  || ch == '~'
                  /////////////////////////
                  || ch == '<'
                  || ch == '>'
                  /////////////////////////
                  || ch == '('
                  || ch == ')'
                  || ch == '=' 
                );

    }

    inline bool isDot(const char& ch)
    {
        return ch == '.';
    }

    inline bool isSemicolon(const char& ch)
    {
        return ch == ';';
    }

    inline E_CharType getCharType(const char& ch)
    {
        if ( isAlpha(ch) ) {
            return E_CHAR_ALPHA;
        } 

        if ( isNumber(ch) ) {
            return E_CHAR_NUMBER;
        } 

        if ( isOperator(ch) ) {
            return E_CHAR_OPERATOR;
        }

        if ( isBlank(ch) ) {
            return E_CHAR_BLANK;
        }

        //--------------------------------------------------
        //       _                  .               ;
        //--------------------------------------------------
        if ( isUnderline(ch) || isDot(ch) || isSemicolon(ch) ) {
            return E_CHAR_MISC;
        }

        return E_CHAR_UNKNOWN;
    }

    inline E_OperatorType getOperatorType(const std::string& opStr)
    {
        E_OperatorType retType = E_OPERATOR_UNKNOWN;

        int sz = static_cast<int>( opStr.size() );
        if ( sz == 1 || sz == 2 ) {
            char ch1st = opStr.at(0);
            if ( isOperator(ch1st) ) {
                if ( sz == 1 ) {
                    switch( ch1st )
                    {
                    case '+':
                        retType = E_ADD;
                        break;
                    case '-':
                        retType = E_MINUS;
                        break;
                    case '*':
                        retType = E_MULTIPLY;
                        break;
                    case '/':
                        retType = E_DIVIDE;
                        break;
                    case '%':
                        retType = E_MOD;
                        break;
                    case '&':
                        retType = E_BIT_AND;
                        break;
                    case '|':
                        retType = E_BIT_OR;
                        break;
                    case '^':
                        retType = E_BIT_XOR;
                        break;
                    case '~':
                        retType = E_BIT_NOT;
                        break;
                    case '(':
                        retType = E_OPEN_PARENTHESES;
                        break;
                    case ')':
                        retType = E_CLOSE_PARENTHESES;
                        break;
                    case '=':
                        retType = E_ASSIGNMENT;
                        break;
                    default:
                        break;
                    }

                } else {
                    // sz == 2
                    if ( opStr == "<<" ) {
                        retType = E_BIT_LEFT_SHIFT;
                    } else if ( opStr == ">>" ) {
                        retType = E_BIT_RIGHT_SHIFT;
                    }
                }
            } 
        } 

        return retType;
    }


    // <Op>   or   <Blank>  or  ';'  or  Unknown-Char
    inline bool isSeperatorChar(const char& ch)
    {
        if ( isSemicolon(ch) ) {
            return true;
        } else {
            auto dt = getCharType(ch);
            return   (   dt == E_CHAR_OPERATOR 
                      || dt == E_CHAR_BLANK
                      || dt == E_CHAR_UNKNOWN);
        }
    }







    ////////////////////////////////////////////////////////////////////
    //
    // Special Letter :   e/E    f/F     l/L   u/U   x/X
    //
    ////////////////////////////////////////////////////////////////////
    inline bool is_eE(const char& ch)
    {
        //         3.4e2   or     3.4E2
        return ch == 'e'   || ch == 'E';
    }

    inline bool is_fF(const char& ch)
    {
        //         3.4f  or     3.4F
        return ch == 'f' || ch == 'F';
    }

    inline bool is_lL(const char& ch)
    {
        //         123l  or     123L
        return ch == 'l' || ch == 'L';
    }

    inline bool is_uU(const char& ch)
    {
        //         123u  or     123U
        return ch == 'u' || ch == 'U';
    }


    inline bool is_xX(const char& ch)
    {
        //           0x   or      0X
        return ch == 'x' || ch == 'X';
    }

    inline bool isPositive(const char& ch)
    {
        return ch == '+';
    }

    inline bool isNegative(const char& ch)
    {
        return ch == '-';
    }


}



#endif

