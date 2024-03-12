module person;

using namespace std;

MyPerson::MyPerson(const string& firstName, const string& lastName)
        : m_firstName{ firstName }
        , m_lastName{ lastName }
{ }

// virtual 
MyPerson::~MyPerson() { }


string MyPerson::getFirstName() const { return m_firstName; }
string MyPerson::getLastName()  const { return m_lastName;  }


