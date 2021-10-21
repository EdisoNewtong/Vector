#ifndef PARSER_EXCEPTION_H
#define PARSER_EXCEPTION_H

#include <exception>
#include <string>
#include <unordered_map>
#include "commonEnum.h"

class ParserExpection : public std::exception
{
public:
	ParserExpection(E_ExceptionCode errorCode);
	virtual const char* what() const throw();

	void setDetail(const std::string& detail);
protected:
	E_ExceptionCode m_code;
	std::string m_exceptionDetail;

    std::unordered_map<E_ExceptionCode, std::string> m_shortInfoMap;

};

#endif
