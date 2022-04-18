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
string getTypeValueRange(const string& prefixHints,int width, bool printOctHex)
{
    stringstream ss;
    width += 4;

    auto minValue = numeric_limits<T>::min();
    auto maxValue = numeric_limits<T>::max();
    ss << prefixHints << (printOctHex ? " | dec : ( " : " : ( ") << std::right << setw(width) << std::dec << minValue << " ~ " << std::left << setw(width) << std::dec << maxValue << " )";
    if ( printOctHex ) {
        ss << endl
           << "\t" << string(prefixHints.size(), ' ') << " | oct : ( 0"  << std::right << setw(width-1) << std::oct << minValue << " ~ 0"  << std::left << setw(width-1) << std::oct << std::uppercase << maxValue << " )" << endl
           << "\t" << string(prefixHints.size(), ' ') << " | hex : ( 0x" << std::right << setw(width-2) << std::hex << minValue << " ~ 0x" << std::left << setw(width-2) << std::hex << std::uppercase << maxValue << " )";
    }

    return ss.str();
}


template<>
string getTypeValueRange<char>(const string& prefixHints,int width, bool printOctHex)
{
    stringstream ss;
    width += 4;

    auto minValue = static_cast<int>( numeric_limits<char>::min()  );
    auto maxValue = static_cast<int>( numeric_limits<char>::max()  );
    ss << prefixHints << (printOctHex ? " | dec : ( " : " : ( ") << std::right << setw(width) << std::dec << minValue << " ~ " << std::left << setw(width) << std::dec << maxValue << " )";
    if ( printOctHex ) {
        ss << endl
           << "\t" << string(prefixHints.size(), ' ') << " | oct : ( " << std::right << setw(width-1) << std::oct << minValue << " ~ 0"  << std::left << setw(width-1) << std::oct << std::uppercase << maxValue << " )" << endl
           << "\t" << string(prefixHints.size(), ' ') << " | hex : ( " << std::right << setw(width-2) << std::hex << minValue << " ~ 0x" << std::left << setw(width-2) << std::hex << std::uppercase << maxValue << " )";
    }

    return ss.str();
}


template<>
string getTypeValueRange<unsigned char>(const string& prefixHints,int width, bool printOctHex)
{
    stringstream ss;
    width += 4;

    auto minValue = static_cast<int>( numeric_limits<unsigned char>::min() & 0xFF );
    auto maxValue = static_cast<int>( numeric_limits<unsigned char>::max() & 0xFF );
    ss << prefixHints << (printOctHex ? " | dec : ( " : " : ( ") << std::right << setw(width) << std::dec << minValue << " ~ " << std::left << setw(width) << std::dec << maxValue << " )";
    if ( printOctHex ) {
        ss << endl
           << "\t" << string(prefixHints.size(), ' ') << " | oct : ( 0"  << std::right << setw(width-1) << std::oct << minValue << " ~ 0"  << std::left << setw(width-1) << std::oct << std::uppercase << maxValue << " )" << endl
           << "\t" << string(prefixHints.size(), ' ') << " | hex : ( 0x" << std::right << setw(width-2) << std::hex << minValue << " ~ 0x" << std::left << setw(width-2) << std::hex << std::uppercase << maxValue << " )";
    }

    return ss.str();
}

template<>
string getTypeValueRange<signed char>(const string& prefixHints,int width, bool printOctHex)
{
    stringstream ss;
    width += 4;

    auto minValue = static_cast<int>( numeric_limits<signed char>::min()  );
    auto maxValue = static_cast<int>( numeric_limits<signed char>::max()  );
    ss << prefixHints << (printOctHex ? " | dec : ( " : " : ( ") << std::right << setw(width) << std::dec << minValue << " ~ " << std::left << setw(width) << std::dec << maxValue << " )";
    if ( printOctHex ) {
        ss << endl
           << "\t" << string(prefixHints.size(), ' ') << " | oct : ( 0"  << std::right << setw(width-1) << std::oct << minValue <<  " ~ 0" << std::left << setw(width-1) << std::oct << std::uppercase << maxValue << " )" << endl
           << "\t" << string(prefixHints.size(), ' ') << " | hex : ( 0x" << std::right << setw(width-2) << std::hex << minValue << " ~ 0x" << std::left << setw(width-2) << std::hex << std::uppercase << maxValue << " )";
    }

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
    cout << "\t" << getTypeValueRange<unsigned char>("unsigned char",4, false) << endl;
    cout << "\t" << getTypeValueRange<  signed char>("  signed char",4, false) << endl;
    cout << "\t" << getTypeValueRange<         char>("         char",4, false) << endl;
    cout << endl << "===========================================================================" << endl << endl;
    cout << "short Part : " << sizeof(short) << " bytes , " << getBits<short>() << " bits" << endl << endl;
    cout << "\t" << getTypeValueRange<unsigned short>("unsigned short",6, false) << endl;
    cout << "\t" << getTypeValueRange<  signed short>("  signed short",6, false) << endl;
    cout << endl << "===========================================================================" << endl << endl;
    if ( b1 ) {
        cout << "int Part ( same as long ) : " << sizeof(int) << " bytes , " << getBits<int>() << " bits" << endl << endl;
    } else {
        cout << "int Part : " << sizeof(int) << " bytes , " << getBits<int>() << " bits" << endl << endl;
    } 
    cout << "\t" << getTypeValueRange<unsigned int>("unsigned int",11, false) << endl;
    cout << "\t" << getTypeValueRange<  signed int>("  signed int",11, false) << endl;
    cout << endl << "===========================================================================" << endl << endl;
    if ( b2 ) {
        cout << "long Part : ( Same as long long ) " << sizeof(long) << " bytes , " << getBits<long>() << " bits" << endl << endl;
    } else {
        cout << "long Part : " << sizeof(long) << " bytes , " << getBits<long>() << " bits" << endl << endl;
    }
    cout << "\t" << getTypeValueRange<unsigned long>("unsigned long",20, false) << endl;
    cout << "\t" << getTypeValueRange<  signed long>("  signed long",20, false) << endl;
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
        cout <<  "===========================================================================" << endl;
        if ( typeArgCnt == 1 ) {
            //
            // typeArgCnt == 1
            //

            if ( typeArgs == "char" ) {
                cout << "\t" << getTypeValueRange<char>("char",4, true) << "    " << sizeof(char) << " byte , " << (sizeof(char) * 8) << " bits. " << endl;
            } else if ( typeArgs == "short" ) {
                cout << "\t" << getTypeValueRange<short>("short",8, true) << "    " << sizeof(short) << " bytes , " << (sizeof(short) * 8) << " bits. " << endl;
            } else if ( typeArgs == "int" ) {
                cout << "\t" << getTypeValueRange<int>("int",11, true) << "    " << sizeof(int) << " bytes , " << (sizeof(int) * 8) << " bits. " << endl;
            } else if ( typeArgs == "long" ) {
                cout << "\t" << getTypeValueRange<long>("long",(longTypeEqual ? 20 : 11), true ) << "    " << sizeof(long) << " bytes , " << (sizeof(long) * 8) << " bits. " << endl;
            } else if ( typeArgs == "unsigned" ) {
                cout << "\t" << getTypeValueRange<unsigned>("unsigned ( unsigned int ) ",11, true) << "    " << sizeof(unsigned) << " bytes , " << (sizeof(unsigned) * 8) << " bits. " << endl;
                // unsigned means -> unsigned int
            } else if ( typeArgs == "signed" ) {
                cout << "\t" << getTypeValueRange<signed>("signed ( signed int ) ",11, true) << "    " << sizeof(signed) << " bytes , " << (sizeof(signed) * 8) << " bits. " << endl;
                // signed means -> signed int
            } else {
                notMatchFlag = true;
            }

        } else if ( typeArgCnt == 2 ) {
            //
            // typeArgCnt == 2
            //
            if ( typeArgs == "unsigned char" ) {
                cout << "\t" << getTypeValueRange<unsigned char>("unsigned char",4, true) << "    " << sizeof(unsigned char) << " bytes , " << (sizeof(unsigned char) * 8) << " bits. " <<   endl;
            } else if ( typeArgs == "signed char" ) {
                cout << "\t" << getTypeValueRange<signed char>("signed char",4, true) <<  "    " << sizeof(signed char) << " bytes , " << (sizeof(signed char) * 8) << " bits. " << endl;
            } else if ( typeArgs == "unsigned short" ) {
                cout << "\t" << getTypeValueRange<unsigned short>("unsigned short",8, true) <<  "    " << sizeof(unsigned short) << " bytes , " << (sizeof(unsigned short) * 8) << " bits. " << endl;
            } else if ( typeArgs == "signed short" ) {
                cout << "\t" << getTypeValueRange<signed short>("signed short",8, true) <<  "    " << sizeof(signed short) << " bytes , " << (sizeof(signed short) * 8) << " bits. " << endl;
            } else if ( typeArgs == "unsigned int" ) {
                cout << "\t" << getTypeValueRange<unsigned int>("unsigned int",11, true) <<  "    " << sizeof(unsigned int) << " bytes , " << (sizeof(unsigned int) * 8) << " bits. " << endl;
            } else if ( typeArgs == "signed int" ) {
                cout << "\t" << getTypeValueRange<signed int>("signed int",11, true) <<  "    " << sizeof(signed int) << " bytes , " << (sizeof(signed int) * 8) << " bits. " << endl;
            } else if ( typeArgs == "unsigned long" ) {
                cout << "\t" << getTypeValueRange<unsigned long>("unsigned long",(longTypeEqual ? 20 : 11), true) <<  "    " << sizeof(unsigned long) << " bytes , " << (sizeof(unsigned long) * 8) << " bits. " << endl;
            } else if ( typeArgs == "signed long" ) {
                cout << "\t" << getTypeValueRange<signed long>("signed long",(longTypeEqual ? 20 : 11), true) <<  "    " << sizeof(signed long) << " bytes , " << (sizeof(signed long) * 8) << " bits. " << endl;
            } else if ( typeArgs == "long int" ) {
                cout << "\t" << getTypeValueRange<long int>("long int",(longTypeEqual ? 20 : 11), true) <<  "    " << sizeof(long int) << " bytes , " << (sizeof(long int) * 8) << " bits. " << endl;
            } else if ( typeArgs == "long long" ) {
                cout << "\t" << getTypeValueRange<long long>("long long",20, true) <<  "    " << sizeof(long long) << " bytes , " << (sizeof(long long) * 8) << " bits. " << endl;
            } else {
                notMatchFlag = true;
            }
        } else {
            //
            // typeArgs == 3
            //
            if ( typeArgs == "unsigned long long" ) {
                cout << "\t" << getTypeValueRange<unsigned long long>("unsigned long long",20, true) <<  "    " << sizeof(unsigned long long) << " bytes , " << (sizeof(unsigned long long) * 8) << " bits. " << endl;
            } else if ( typeArgs == "signed long long" ) {
                cout << "\t" << getTypeValueRange<signed long long>("signed long long",20, true) <<  "    " << sizeof(signed long long) << " bytes , " << (sizeof(signed long long) * 8) << " bits. " << endl;
            } else if ( typeArgs == "unsigned long int" ) {
                cout << "\t" << getTypeValueRange<unsigned long int>("unsigned long int",20, true) <<  "    " << sizeof(unsigned long int) << " bytes , " << (sizeof(unsigned long int) * 8) << " bits. " << endl;
            } else if ( typeArgs == "signed long int" ) {
                cout << "\t" << getTypeValueRange<signed long int>("signed long int",20, true) <<  "    " << sizeof(signed long int) << " bytes , " << (sizeof(signed long int) * 8) << " bits. " << endl;
            } else if ( typeArgs == "long long int" ) {
                cout << "\t" << getTypeValueRange<long long int>("long long int",20, true) <<  "    " << sizeof(long long int) << " bytes , " << (sizeof(long long int) * 8) << " bits. " << endl;
            } else {
                notMatchFlag = true;
            }

        }

        if ( notMatchFlag ) {
            cout << "Unknown Type : \"" << typeArgs << "\" ??? " << endl << endl;
            printUsage();
        } else {
            cout <<  "===========================================================================" << endl; 
        }
    } 
    
    

    
    return 0;
}

