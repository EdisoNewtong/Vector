// #include <iostream>
#include "parserException.h"

using namespace std;

ParserException::ParserException(E_ExceptionCode errorCode)
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
	m_shortInfoMap.insert( make_pair( enumCvt(E_TOKEN_LOGIC_INVALID), string("Invalid Token Pushing Logic") ) );

}

ParserException::ParserException(const  ParserException& r)
	: m_code(r.m_code)
    , m_exceptionDetail( r.m_exceptionDetail )
	, m_formatDetail( r.m_formatDetail )
    , m_shortInfoMap( r.m_shortInfoMap )
{

	// cout << endl << "\tCopying ()" << endl;
}


void ParserException::setDetail(const string& detail)
{
	m_exceptionDetail = detail;
}

// virtual 
const char* ParserException::what() const throw()
{
	// cout << "InSide ParserException ... ()" << endl;
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

