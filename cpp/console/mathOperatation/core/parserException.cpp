// #include <iostream>
#include "parserException.h"

using namespace std;

ParserExpection::ParserExpection(E_ExceptionCode errorCode)
	: m_code(errorCode)
	, m_exceptionDetail("Unknown Error")
	, m_formatDetail("")
{
	m_shortInfoMap.clear();
	m_shortInfoMap.insert( make_pair(E_UNKNOWN_CHAR, string("Unknown Character") ) );
}

ParserExpection::ParserExpection(const  ParserExpection& r)
	: m_code(r.m_code)
    , m_exceptionDetail( r.m_exceptionDetail )
	, m_formatDetail( r.m_formatDetail )
    , m_shortInfoMap( r.m_shortInfoMap )
{

	// cout << endl << "\tCopying ()" << endl;
}


void ParserExpection::setDetail(const string& detail)
{
	m_exceptionDetail = detail;
}

// virtual 
const char* ParserExpection::what() const throw()
{
	// cout << "InSide ParserExpection ... ()" << endl;
	auto it = m_shortInfoMap.find(m_code);
	if ( it != m_shortInfoMap.end() ) {
		m_formatDetail += (it->second);
		m_formatDetail += std::string(" : ");
	} 
	m_formatDetail += m_exceptionDetail;

	return m_formatDetail.c_str();
}

