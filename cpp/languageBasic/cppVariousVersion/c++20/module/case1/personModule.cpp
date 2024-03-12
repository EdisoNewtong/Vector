export module person;

import <string>;

export class MyPerson
{
public:
    MyPerson(const std::string& firstName, const std::string& lastName)
        : m_firstName{ firstName }
        , m_lastName{ lastName }
    { }

    virtual ~MyPerson() { }

    std::string getFirstName() const { return m_firstName; }
    std::string getLastName()  const { return m_lastName;  }
protected:
    std::string m_firstName;
    std::string m_lastName;
};


