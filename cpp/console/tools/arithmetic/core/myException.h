#ifndef MY_EXCEPTION_H
#define MY_EXCEPTION_H

#include <exception>
#include <unordered_map>
#include <string>
#include "commonEnum.h"
#include "buff.h"

class MyException : public std::exception
{
public:
    MyException(E_ExceptionType errorCode);
    MyException(E_ExceptionType errorCode, ChInfo cur);
    MyException(const  MyException& r) = default;
    // MyException(const  MyException& r);

    std::string getDetail();
    void setDetail(const std::string& detail);
    virtual const char* what() const noexcept override; // throw();
protected:
    E_ExceptionType m_errorCode;
    std::string     m_detail;
    ChInfo          m_cursor;

    bool            m_hasSetCursor;


    static std::string s_retDetail;


#ifdef USE_INT_INSTEAD_OF_ENUM
    typedef int ErrorType;
#else
    typedef E_ExceptionType ErrorType;
#endif

private:


    static std::unordered_map<ErrorType,std::string> s_exceptionMap;
    
};

#endif
