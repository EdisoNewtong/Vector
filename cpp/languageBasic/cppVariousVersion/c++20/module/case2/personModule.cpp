export module person;

import <string>;

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Declearation Part
//
////////////////////////////////////////////////////////////////////////////////////////////////////
export class MyPerson
{
public:
    MyPerson(const std::string& firstName, const std::string& lastName);
    virtual ~MyPerson();

    std::string getFirstName() const;
    std::string getLastName()  const;
protected:
    std::string m_firstName;
    std::string m_lastName;
};




////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation Part
//
////////////////////////////////////////////////////////////////////////////////////////////////////

MyPerson::MyPerson(const std::string& firstName, const std::string& lastName)
        : m_firstName{ firstName }
        , m_lastName{ lastName }
{ }

// virtual 
MyPerson::~MyPerson() { }

std::string MyPerson::getFirstName() const { return m_firstName; }
std::string MyPerson::getLastName()  const { return m_lastName;  }


