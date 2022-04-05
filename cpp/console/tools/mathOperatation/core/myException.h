#ifndef PARSER_EXCEPTION_H
#define PARSER_EXCEPTION_H

#include <exception>
#include <string>
#include <unordered_map>
#include "commonEnum.h"



class MyException : public std::exception
{
public:
	MyException(E_ExceptionCode errorCode);
	MyException(const  MyException& r) = default;
	virtual const char* what() const throw();

	void setDetail(const std::string& detail);
protected:
	E_ExceptionCode m_code;
	std::string m_exceptionDetail;
	mutable std::string m_formatDetail;

#ifdef USE_INT_INSTEAD_OF_ENUM
    std::unordered_map<int, std::string> m_shortInfoMap;
#else
    std::unordered_map<E_ExceptionCode, std::string> m_shortInfoMap;
#endif


};

#endif
