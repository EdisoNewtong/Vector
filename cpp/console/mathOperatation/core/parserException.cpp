// #include <iostream>
#include "parserException.h"

using namespace std;

ParserExpection::ParserExpection(E_ExceptionCode errorCode, ParsedCharInfo* pInfo)
	: m_code(errorCode)
	, m_exceptionDetail("Unknown Error")
	, m_formatDetail("")
	, m_shortInfoMap()
	, m_pParsedCharInfo(pInfo)
{
	m_shortInfoMap.clear();
	m_shortInfoMap.insert( make_pair(static_cast<int>(E_UNKNOWN_CHAR), string("Unknown Character") ) );
}

ParserExpection::ParserExpection(const  ParserExpection& r)
	: m_code(r.m_code)
    , m_exceptionDetail( r.m_exceptionDetail )
	, m_formatDetail( r.m_formatDetail )
    , m_shortInfoMap( r.m_shortInfoMap )
	, m_pParsedCharInfo( r.m_pParsedCharInfo )
{

	// cout << endl << "\tCopying ()" << endl;
}


void ParserExpection::setDetail(const string& detail)
{
/*

string info;
if ( ch >= 0 ) {
	// < 32
	if ( ch < 32 ) {
		if ( ch == '\t' ) {
			info += "'\\t'";
		} else if ( ch == '\r' ) {
			info += "'\\r'";
		} else if ( ch == '\n' ) {
			info += "'\\n'";
		} else {
			info += " ? , code = ";  
			info += std::to_string( static_cast<int>(ch & 0xFFU) );
		}
	} else {
		if ( ch == 32 ) {
			info += "' '";
		} else {
			info += "'";
			info += ch;
			info += "'";
		}
	}
} else {
	// < 0
	info += " ? , code = ";  
	info += std::to_string( static_cast<int>(ch & 0xFFU) );
}

info += "  | index = ";
info += std::to_string(pInfo->position.nCharIdx);
info += " @Line ";
info += std::to_string(pInfo->position.nLine);
info += ":";
info += std::to_string(pInfo->position.nCol);

e.setDetail(info);


*/
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

