#include <iostream>
#include <cmath>
using namespace std;

#include "AAA.h"

AAA::AAA(double n) : m_number(n)
{
    cout << "AAA Create. " << endl;
}


AAA::~AAA()
{
    cout << "AAA Destroy. " << endl;
}


double AAA::getSquareRoot()
{
    return std::sqrt(m_number);
}


