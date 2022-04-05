// #include <iostream>
#include "myException.h"

using namespace std;

MyException::MyException(E_ExceptionCode errorCode)
	: m_code(errorCode)
	, m_exceptionDetail("Unknown Error")
	, m_formatDetail("")
	, m_shortInfoMap()
{
	m_shortInfoMap.clear();
	m_shortInfoMap.insert( make_pair( enumCvt(E_UNKNOWN_CHAR), string("Unknown Character") ) );

	m_shortInfoMap.insert( make_pair( enumCvt(E_MULTI_LINE_COMMENT_INVALID_FORMAT), string("Invalid Mutli-Line Comment Format") ) );
	m_shortInfoMap.insert( make_pair( enumCvt(E_SINGLE_LINE_COMMENT_INVALID_FORMAT), string("Invalid Single-Line Comment Format") ) );

	m_shortInfoMap.insert( make_pair( enumCvt(E_DECIMAL_INVALID_FORMAT), string("Invalid Decimal Format") ) );
	m_shortInfoMap.insert( make_pair( enumCvt(E_OCT_INVALID_FORMAT), string("Invalid Oct Format") ) );
	m_shortInfoMap.insert( make_pair( enumCvt(E_HEX_INVALID_FORMAT), string("Invalid Hex Format") ) );

	m_shortInfoMap.insert( make_pair( enumCvt(E_FLOAT_INVALID_FORMAT), string("Invalid Float Format") ) );

	m_shortInfoMap.insert( make_pair( enumCvt(E_OPERATOR_INVALID_FORMAT), string("Invalid Operator Format") ) );
	m_shortInfoMap.insert( make_pair( enumCvt(E_VARIBLE_INVALID_FORMAT), string("Invalid Varible Format") ) );
    m_shortInfoMap.insert( make_pair( enumCvt(E_EXP_EVALUATION_INVALID_FORMAT), string("Invalid when pushing ';' ") ) );
	m_shortInfoMap.insert( make_pair( enumCvt(E_TOKEN_LOGIC_INVALID), string("Invalid Logic when pushing a certain token") ) );
    m_shortInfoMap.insert( make_pair( enumCvt(E_TOKEN_CLOSE_PARENTHESES_MISMATCHED), string("')' Mismatched Close Parenthese ") ) );
    m_shortInfoMap.insert( make_pair( enumCvt(E_TOKEN_OPEN_PARENTHESES_MISMATCHED), string("'(' Mismatched Open Parenthese ") ) );

    m_shortInfoMap.insert( make_pair( enumCvt(E_INVALID_NUMBER_BASE), string("Invalid number base ") ) );

    m_shortInfoMap.insert( make_pair( enumCvt(E_INVALID_OCTAL_NUMBER), string("Invalid octal number : ") ) );
    m_shortInfoMap.insert( make_pair( enumCvt(E_INVALID_DECIMAL_NUMBER), string("Invalid decimal number : ") ) );
    m_shortInfoMap.insert( make_pair( enumCvt(E_INVALID_HEX_NUMBER), string("Invalid hex number : ") ) );
    m_shortInfoMap.insert( make_pair( enumCvt(E_DIFFERENT_NUMBER_BASE), string("Diff Base for 2 numbers compare : ") ) );

    m_shortInfoMap.insert( make_pair( enumCvt(E_VARIBLE_NAME_CANNOT_BE_KEYWORD), string("Varible name can't be c++ Key Word : ") ) );
    m_shortInfoMap.insert( make_pair( enumCvt(E_VARIBLE_NAME_ALREADY_DECLARED), string("Varible name has already be declared : ") ) );

    m_shortInfoMap.insert( make_pair( enumCvt(E_SWITCH_CASE_DEFAULT_FLOW), string("In switch-case default flow : ") ) );
}

/*
MyException::MyException(const  MyException& r)
	: m_code(r.m_code)
    , m_exceptionDetail( r.m_exceptionDetail )
	, m_formatDetail( r.m_formatDetail )
    , m_shortInfoMap( r.m_shortInfoMap )
{

	// cout << endl << "\tCopying ()" << endl;
}
*/

void MyException::setDetail(const string& detail)
{
	m_exceptionDetail = detail;
}

// virtual 
const char* MyException::what() const throw()
{
	// cout << "InSide MyException ... ()" << endl;
	auto it = m_shortInfoMap.find( enumCvt(m_code) );
	if ( it != m_shortInfoMap.end() ) {
		m_formatDetail += (it->second);
		m_formatDetail += std::string(" : ");
		m_formatDetail += "\n";
	}

	if ( !m_exceptionDetail.empty() ) {
		m_formatDetail += "==================================================\n";
		m_formatDetail += m_exceptionDetail;
		m_formatDetail += "\n";
		m_formatDetail += "==================================================\n";
	}

	return m_formatDetail.c_str();
}

