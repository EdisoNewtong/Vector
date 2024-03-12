module person;

using namespace std;

MyPersonWithAge::MyPersonWithAge(const string& firstName, const string& lastName, int age)
    : MyPerson(firstName, lastName)
    , m_age(age)
{ }

// virtual 
// MyPersonWithAge::~MyPersonWithAge() { }

int MyPersonWithAge::getAge() const { return m_age; }


