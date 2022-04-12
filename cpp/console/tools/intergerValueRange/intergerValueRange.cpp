#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <sstream>
using namespace std;

void printUsage()
{
    cout << 
R"(Usage:
           ./<programName>
      or   ./<programName>  <Data Type Name>   ([unsigned/signed] <char/short/int/long/long long>)
)" << endl;

}

template<typename T>
string getTypeValueRange(const string& prefixHints,int width)
{
    stringstream ss;

    auto minValue = numeric_limits<T>::min();
    auto maxValue = numeric_limits<T>::max();
    ss << prefixHints << " : ( " << setw(width) << minValue << " ~ " << setw(width) << maxValue << " )";

    return ss.str();
}


template<>
string getTypeValueRange<char>(const string& prefixHints,int width)
{
    stringstream ss;

    auto minValue = static_cast<int>( numeric_limits<char>::min()  );
    auto maxValue = static_cast<int>( numeric_limits<char>::max()  );
    ss << prefixHints << " : ( " << setw(width) << minValue << " ~ " << setw(width) << maxValue << " )";

    return ss.str();
}


template<>
string getTypeValueRange<unsigned char>(const string& prefixHints,int width)
{
    stringstream ss;

    auto minValue = static_cast<int>( numeric_limits<unsigned char>::min() & 0xFF );
    auto maxValue = static_cast<int>( numeric_limits<unsigned char>::max() & 0xFF );
    ss << prefixHints << " : ( " << setw(width) << minValue << " ~ " << setw(width) << maxValue << " )";

    return ss.str();
}

template<>
string getTypeValueRange<signed char>(const string& prefixHints,int width)
{
    stringstream ss;

    auto minValue = static_cast<int>( numeric_limits<signed char>::min()  );
    auto maxValue = static_cast<int>( numeric_limits<signed char>::max()  );
    ss << prefixHints << " : ( " << setw(width) << minValue << " ~ " << setw(width) << maxValue << " )";

    return ss.str();
}

template<typename T>
int getBits()
{
    return static_cast<int>( sizeof(T) * 8 );
}



void printAll(bool b1,bool b2)
{
    cout <<  "===========================================================================" << endl << endl;
    cout << "char Part : " << sizeof(char) << " byte , " << getBits<char>() << " bits" << endl << endl;
    cout << "\t" << getTypeValueRange<unsigned char>("unsigned char",4) << endl;
    cout << "\t" << getTypeValueRange<  signed char>("  signed char",4) << endl;
    cout << "\t" << getTypeValueRange<         char>("         char",4) << endl;
    cout << endl << "===========================================================================" << endl << endl;
    cout << "short Part : " << sizeof(short) << " bytes , " << getBits<short>() << " bits" << endl << endl;
    cout << "\t" << getTypeValueRange<unsigned short>("unsigned short",6) << endl;
    cout << "\t" << getTypeValueRange<  signed short>("  signed short",6) << endl;
    cout << endl << "===========================================================================" << endl;
    if ( b1 ) {
        cout << "int Part ( same as long ) : " << sizeof(int) << " bytes , " << getBits<int>() << " bits" << endl << endl;
    } else {
        cout << "int Part : " << sizeof(int) << " bytes , " << getBits<int>() << " bits" << endl << endl;
    } 
    cout << "\t" << getTypeValueRange<unsigned int>("unsigned int",11) << endl;
    cout << "\t" << getTypeValueRange<  signed int>("  signed int",11) << endl;
    cout << endl << "===========================================================================" << endl << endl;
    if ( b2 ) {
        cout << "long Part : ( Same as long long ) " << sizeof(long) << " bytes , " << getBits<long>() << " bits" << endl << endl;
    } else {
        cout << "long Part : " << sizeof(long) << " bytes , " << getBits<long>() << " bits" << endl << endl;
    }
    cout << "\t" << getTypeValueRange<unsigned long>("unsigned long",20) << endl;
    cout << "\t" << getTypeValueRange<  signed long>("  signed long",20) << endl;
    cout << endl << "===========================================================================" << endl << endl;
}


int main(int argc, char* argv[], char* env[])
{
    if ( argc > 4 ) {
        cout << "[ERROR] : Too many arguments " << endl;
        printUsage();
        return -1;
    }

    bool intLongEqual = (sizeof(int) == sizeof(long));
    bool longTypeEqual = (sizeof(long) == sizeof(long long));

    // argc <=4
    if ( argc == 1 ) {
        printAll(intLongEqual, longTypeEqual);
        return -1;
    } else {
        // argc >= 2
        int typeArgCnt = argc -1;
        string typeArgs = string(argv[1]);
        if ( argc >= 3 ) {
            typeArgs += string(" ");
            typeArgs += string(argv[2]);
        }

        if ( argc == 4 ) {
            typeArgs += string(" ");
            typeArgs += string(argv[3]);
        }


        auto notMatchFlag = false;
        if ( typeArgCnt == 1 ) {
            //
            // typeArgCnt == 1
            //

            cout <<  "===========================================================================" << endl << endl;
            if ( typeArgs == "char" ) {
                cout << "\t" << getTypeValueRange<char>("char",4) << "    " << sizeof(char) << " byte , " << (sizeof(char) * 8) << " bits. " << endl;
            } else if ( typeArgs == "short" ) {
                cout << "\t" << getTypeValueRange<short>("short",8) << "    " << sizeof(short) << " bytes , " << (sizeof(short) * 8) << " bits. " << endl;
            } else if ( typeArgs == "int" ) {
                cout << "\t" << getTypeValueRange<int>("int",11) << "    " << sizeof(int) << " bytes , " << (sizeof(int) * 8) << " bits. " << endl;
            } else if ( typeArgs == "long" ) {
                cout << "\t" << getTypeValueRange<long>("long",(longTypeEqual ? 20 : 11) ) << "    " << sizeof(long) << " bytes , " << (sizeof(long) * 8) << " bits. " << endl;
            } else if ( typeArgs == "unsigned" ) {
                cout << "\t" << getTypeValueRange<unsigned>("unsigned ( unsigned int ) ",11) << "    " << sizeof(unsigned) << " bytes , " << (sizeof(unsigned) * 8) << " bits. " << endl;
                // unsigned means -> unsigned int
            } else if ( typeArgs == "signed" ) {
                cout << "\t" << getTypeValueRange<signed>("signed ( signed int ) ",11) << "    " << sizeof(signed) << " bytes , " << (sizeof(signed) * 8) << " bits. " << endl;
                // signed means -> signed int
            } else {
                notMatchFlag = true;
            }

        } else if ( typeArgCnt == 2 ) {
            //
            // typeArgCnt == 2
            //
            if ( typeArgs == "unsigned char" ) {
                cout << "\t" << getTypeValueRange<unsigned char>("unsigned char",4) << "    " << sizeof(unsigned char) << " bytes , " << (sizeof(unsigned char) * 8) << " bits. " <<   endl;
            } else if ( typeArgs == "signed char" ) {
                cout << "\t" << getTypeValueRange<signed char>("signed char",4) <<  "    " << sizeof(signed char) << " bytes , " << (sizeof(signed char) * 8) << " bits. " << endl;
            } else if ( typeArgs == "unsigned short" ) {
                cout << "\t" << getTypeValueRange<unsigned short>("unsigned short",8) <<  "    " << sizeof(unsigned short) << " bytes , " << (sizeof(unsigned short) * 8) << " bits. " << endl;
            } else if ( typeArgs == "signed short" ) {
                cout << "\t" << getTypeValueRange<signed short>("signed short",8) <<  "    " << sizeof(signed short) << " bytes , " << (sizeof(signed short) * 8) << " bits. " << endl;
            } else if ( typeArgs == "unsigned int" ) {
                cout << "\t" << getTypeValueRange<unsigned int>("unsigned int",11) <<  "    " << sizeof(unsigned int) << " bytes , " << (sizeof(unsigned int) * 8) << " bits. " << endl;
            } else if ( typeArgs == "signed int" ) {
                cout << "\t" << getTypeValueRange<signed int>("signed int",11) <<  "    " << sizeof(signed int) << " bytes , " << (sizeof(signed int) * 8) << " bits. " << endl;
            } else if ( typeArgs == "unsigned long" ) {
                cout << "\t" << getTypeValueRange<unsigned long>("unsigned long",(longTypeEqual ? 20 : 11)) <<  "    " << sizeof(unsigned long) << " bytes , " << (sizeof(unsigned long) * 8) << " bits. " << endl;
            } else if ( typeArgs == "signed long" ) {
                cout << "\t" << getTypeValueRange<signed long>("signed long",(longTypeEqual ? 20 : 11)) <<  "    " << sizeof(signed long) << " bytes , " << (sizeof(signed long) * 8) << " bits. " << endl;
            } else if ( typeArgs == "long long" ) {
                cout << "\t" << getTypeValueRange<long long>("long long",20) <<  "    " << sizeof(long long) << " bytes , " << (sizeof(long long) * 8) << " bits. " << endl;
            } else {
                notMatchFlag = true;
            }
        } else {
            //
            // typeArgs == 3
            //
            if ( typeArgs == "unsigned long long" ) {
                cout << "\t" << getTypeValueRange<unsigned long long>("unsigned long long",20) <<  "    " << sizeof(unsigned long long) << " bytes , " << (sizeof(unsigned long long) * 8) << " bits. " << endl;
            } else if ( typeArgs == "signed long long" ) {
                cout << "\t" << getTypeValueRange<signed long long>("signed long long",20) <<  "    " << sizeof(signed long long) << " bytes , " << (sizeof(signed long long) * 8) << " bits. " << endl;
            } else {
                notMatchFlag = true;
            }

        }

        if ( notMatchFlag ) {
            cout << "Unknown Type : \"" << typeArgs << "\" ??? " << endl << endl;
            printUsage();
        } else {
            cout << endl <<  "===========================================================================" << endl; 
        }
    } 
    
    

    
    return 0;
}

