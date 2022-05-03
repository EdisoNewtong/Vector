#ifndef MY_EXCEPTION_H
#define MY_EXCEPTION_H

#include <exception>
#include <string>

class MyException : public std::exception
{
public:
    MyException(const std::string& desc);

    ~MyException() = default;
    MyException(const MyException& r) = default;

    virtual const char* what() const noexcept override;
    std::string  getDetail() const;
protected:
    std::string m_description;
};

#endif
