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


export class MyPersonWithAge : public MyPerson
{
public:
    MyPersonWithAge(const std::string& firstName, const std::string& lastName, int age);
    // virtual ~MyPersonWithAge();

    virtual int getAge() const;
protected:
    int m_age;
};




