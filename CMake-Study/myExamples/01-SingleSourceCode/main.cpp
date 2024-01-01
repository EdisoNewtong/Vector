#include <iostream>
using namespace std;

#include "pi.h"

#ifdef  HAPPY_NEW_YEAR
    static const char* SC_WELCOME_TITLE = "Happy New Year, let's welcome 2024. "; 
#else
    static const char* SC_WELCOME_TITLE = "Welcome ... "; 
#endif


int main(int argc, char* argv[])
{
    cout << SC_WELCOME_TITLE << endl
         << endl;

    cout << "PI = " << PI << endl;

    return 0;
}


