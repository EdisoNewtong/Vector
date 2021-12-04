#include "enum2name.h"

using namespace std;

namespace Enum2Name
{


string getParserName(E_PaserType tp)
{	
	string retStr;

	switch( tp ) 
	{
	case E_P_DEFAULT:
		retStr = "E_P_DEFAULT";
		break;
	case E_P_DECIMAL:
		retStr = "E_P_DECIMAL";
		break;
	case E_P_OCTAL:
		retStr = "E_P_OCTAL";
		break;
	case E_P_HEX:
		retStr = "E_P_HEX";
		break;
	case E_P_FLOAT:
		retStr = "E_P_FLOAT";
		break;
	case E_P_VARIBLE:
		retStr = "E_P_VARIBLE";
		break;
	case E_P_OPERATOR:
		retStr = "E_P_OPERATOR";
		break;
	case E_P_SINGLE_LINE_COMMENT:
		retStr = "E_P_SINGLE_LINE_COMMENT";
		break;
	case E_P_MULTI_LINE_COMMENT:
		retStr = "E_P_MULTI_LINE_COMMENT";
		break;
	case E_P_BLANK:
		retStr = "E_P_BLANK";
		break;
	case E_P_ENDLESS_SEMICOLON:
		retStr = "E_P_ENDLESS_SEMICOLON";
		break;
	default:
		retStr = "E_P_UNKNOWN ???";
		break;
	}

	return retStr;
}

string getTokenName(E_TokenType tp)
{	
	string ret;
	switch( tp )
	{
	case E_TOKEN_UNKNOWN:
		ret = "E_TOKEN_UNKNOWN";
		break;
	case E_TOKEN_BLANK:
		ret = "E_TOKEN_BLANK";
		break;
	case E_TOKEN_COMMENT_TYPE:
		ret = "E_TOKEN_COMMENT_TYPE";
		break;
	case E_TOKEN_INTEGER_NUMBER:
		ret = "E_TOKEN_INTEGER_NUMBER";
		break;
	case E_TOKEN_FLOAT_NUMBER:
		ret = "E_TOKEN_FLOAT_NUMBER";
		break;
	case E_TOKEN_VARIBLE:
		ret = "E_TOKEN_VARIBLE";
		break;
	case E_TOKEN_OP_OPEN_PARENTHESES: // = 8,  // (
		ret = "'('";
		break;
	case E_TOKEN_OP_CLOSE_PARENTHESES: // = 9, // )
		ret = "')'";
		break;
	case E_TOKEN_OP_ADD: // = 10,       // +
		ret = "'+'";
		break;
	case E_TOKEN_OP_POSITIVE: // = 11,     // +3
		ret = "'+' positive ";
		break;
	case E_TOKEN_OP_MINUS: // = 12,             // -
		ret = "'-'";
		break;
	case E_TOKEN_OP_NEGATIVE: // = 13,     // -3
		ret = "'-' negative";
		break;
	case E_TOKEN_OP_MULTIPLY: // = 14,          // *
		ret = "'*'";
		break;
	case E_TOKEN_OP_DIVIDE: // = 15,            // /
		ret = "'/'";
		break;
	case E_TOKEN_OP_MOD: // = 16,               // %
		ret = "'%'";
		break;
	case E_TOKEN_OP_BIT_AND: // = 17,           // &
		ret = "'&'";
		break;
	case E_TOKEN_OP_BIT_OR: // = 18,            // |
		ret = "'|'";
		break;
	case E_TOKEN_OP_BIT_XOR: // = 19,           // ^
		ret = "'^'";
		break;
	case E_TOKEN_OP_BIT_NOT: // = 20,           // ~
		ret = "'~'";
		break;
	case E_TOKEN_OP_BIT_LEFT_SHIFT: // = 21,    // <<
		ret = "'<<'";
		break;
	case E_TOKEN_OP_BIT_RIGHT_SHIFT: // = 22,   // >>
		ret = "'>>'";
		break;
	case E_TOKEN_OP_ASSIGNMENT: // = 23,        // =
		ret = "'='";
		break;
	default:
		ret = "In switch-case default part :  E_TOKEN_UNKNOWN";
		break;
	}

	return ret;

}


string getDataType(E_DataType tp)
{
	string retStr;

	switch ( tp )
	{
	case E_TP_U_CHAR:
		retStr = "unsigned char";
		break;
	case E_TP_S_CHAR:
		retStr = "signed char";
		break;
	case E_TP_U_SHORT:
		retStr = "unsigned short";
		break;
	case E_TP_S_SHORT:
		retStr = "signed short";
		break;
	case E_TP_U_INT:
		retStr = "unsigned int";
		break;
	case E_TP_S_INT:
		retStr = "signed int";
		break;
	case E_TP_U_LONG:
		retStr = "unsigned long";
		break;
	case E_TP_S_LONG:
		retStr = "signed long";
		break;
	case E_TP_U_LONG_LONG:
		retStr = "unsigned long long";
		break;
	case E_TP_S_LONG_LONG:
		retStr = "signed long long";
		break;
    //////////////////////////////////////////////////
	case E_TP_FLOAT:
		retStr = "float";
		break;
	case E_TP_DOUBLE:
		retStr = "double";
		break;
	case E_TP_LONG_DOUBLE:
		retStr = "long double";
		break;
	default:
		retStr = "Unknown Data Type ???";
		break;
	}

	return retStr;
}




} // end namespace Enum2Name
