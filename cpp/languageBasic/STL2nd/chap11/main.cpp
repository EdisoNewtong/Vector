#include <iostream>
#include "NonModifying.h"
#include "Modifying.h"
#include "Removing.h"
#include "Mutating.h"
#include "Sorting.h"
#include "SortedRange.h"
#include "Numeric.h"

using namespace std;



int main(int argc, char* argv[])
{
    NonModifying::testCaseAll();
    cout << endl;

    Modifying::testCaseAll();
    cout << endl;

    Removing::testCaseAll();
    cout << endl;

    Mutating::testCaseAll();
    cout << endl;

    Sorting::testCaseAll();
    cout << endl;

    SortedRange::testCaseAll();
    cout << endl;

    Numeric::testCaseAll();
    cout << endl;

    return 0;
}
