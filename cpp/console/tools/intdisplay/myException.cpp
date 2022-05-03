#include "myException.h"

MyException::MyException(const std::string& desc) 
    : std::exception() 
    , m_description(desc) 
{

}


// virtual 
const char* MyException::what() const noexcept // override 
{
    return m_description.c_str();
}


std::string  MyException::getDetail()  const
{
    return m_description;
}

