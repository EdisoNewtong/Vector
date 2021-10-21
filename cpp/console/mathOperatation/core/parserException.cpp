#include "parserException.h"

using namespace std;

ParserExpection::ParserExpection(E_ExceptionCode errorCode)
	: m_code(errorCode)
	, m_exceptionDetail("Unknown Error")
{
	m_shortInfoMap.clear();
	m_shortInfoMap.insert( make_pair(E_UNKNOWN_CHAR, string("Unknown Character") ) );
}


void ParserExpection::setDetail(const string& detail)
{
	m_exceptionDetail = detail;
}

// virtual 
const char* ParserExpection::what() const throw()
{
	string detail;
	auto it = m_shortInfoMap.find(m_code);
	if ( it != m_shortInfoMap.end() ) {
		detail += it->second;
		detail += " : ";
	} 
	detail += m_exceptionDetail;

	return detail.c_str();
}

