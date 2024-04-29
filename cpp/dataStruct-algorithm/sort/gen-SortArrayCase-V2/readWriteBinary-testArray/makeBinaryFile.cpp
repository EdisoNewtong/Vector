#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <vector>
using namespace std;

/******************************************************************************************************
 
  2 ~ 10,  max element is 10 ( 0xA -> 0b1010 ) , only contain 4 bits in all
so 2 elements can be merged into 1 byte

if the array's size is odd number such as 3 , the array will be like [1,2,3]

encoding in detail will be like the following : 
        [1, 2,   3]
        00010010 0011???? 

    the total bytes equaltion will be like :  ( array_size / 2 ) + (array_size % 2 == 0 : 0 : 1)

the possibility equaltion : pow(3, (array_size-1) )

    array with 2 elements : 3 possiblilities [ < , = , > ]
        3 * (2 * 4 bytes) in all

the binary file format is like the following :

----------------------------------------------------------------------------------------------------
         1 byte
2 :   | array-size | array#1 | array#2 | ... | array#possibility-count
3 :   | array-size | array#1 | array#2 | ... | array#possibility-count
4 :   | array-size | array#1 | array#2 | ... | array#possibility-count

                 ...
                 ...
                 ...

10 :  | array-size | array#1 | array#2 | ... | array#possibility-count

----------------------------------------------------------------------------------------------------

******************************************************************************************************/

bool readAndEncoding(int arraySize, const string& fname, ofstream& fileObject)
{
    // (void)fileObject;
    ifstream inFile(fname.c_str(), ios::in);
    if ( !inFile ) {
        cerr << "[ERROR] Can't open file \"" << fname  << "\" for reading sort test case array. " << endl;
        return false;
    }

    const char CS_SEP1 = ':';
    const char CS_SEP2 = '[';
    const char CS_SEP3 = ']';
    const char CS_SEP4 = ',';

    const string errorPrefix("Can't find char : ");
    const int cmpPossibilityCnt = 3; // < or == or > 
    const int c_leftShiftWidth = 4;
    const int lines2read = pow(cmpPossibilityCnt, arraySize -1 );
    int alreadreadlineCnt = 0;
    bool meetSomeError = false;
    string errorString;

    char ch_arraySize = static_cast<int>( arraySize );
    fileObject.write( &ch_arraySize, sizeof(ch_arraySize) );
    fileObject.flush();

    while( !inFile.eof() )
    {
        string strLine;
        getline(inFile, strLine );

        // [ = ] :  [ 1, 1 ]. 
        
        // :
        string::size_type pos = strLine.find(CS_SEP1, 0);
        if ( pos == string::npos ) {
            errorString = errorPrefix + CS_SEP1;
            break;
        }
        strLine = strLine.substr(pos+1);

        // [
        pos = strLine.find(CS_SEP2, 0);
        if ( pos == string::npos ) {
            errorString = errorPrefix +  CS_SEP2;
            break;
        }
        strLine = strLine.substr(pos+1);

        // ]
        string::size_type endPos = strLine.find(CS_SEP3, 0);
        if ( pos == string::npos ) {
            errorString = errorPrefix + CS_SEP3;
            break;
        }
        strLine = strLine.substr(0, endPos);

        string::size_type begPos = 0;
        string::size_type sepPos;

        vector<int> ary;
        // ,
        while ( (sepPos = strLine.find(CS_SEP4, begPos)) != string::npos )
        {
            string strNumber = strLine.substr(begPos, sepPos-begPos);
            ary.push_back( stoi(strNumber) );
            begPos = sepPos + 1;
        }
        ary.push_back( stoi(strLine.substr(begPos)) );

        // Real Write Buffers into files
        char ch_4_2_elements = 0;
        for( auto idx = 0 ; idx < static_cast<int>( ary.size()); ++idx ) {
            auto e = ary[idx];
            bool isEven = (idx % 2 == 0);
            if ( isEven ) {
                // ch_4_2_elements |= (e << c_leftShiftWidth);
                ch_4_2_elements |= ((e << c_leftShiftWidth) & 0xF0);
            } else {
                // idx % 2 == 1 , every 4 bits per group
                ch_4_2_elements |= e;

                fileObject.write( &ch_4_2_elements, sizeof(ch_4_2_elements) );
                fileObject.flush();

                ch_4_2_elements = 0;
            }

            if ( isEven && (idx == static_cast<int>( ary.size() - 1) ) ) {
                fileObject.write( &ch_4_2_elements, sizeof(ch_4_2_elements) );
                fileObject.flush();

                ch_4_2_elements = 0;
            }
        }


        /*
        // print result
        cout << (alreadreadlineCnt + 1) << ". [ ";
        for( auto e : ary ) {
            cout << e << " ";
        }
        cout << " ]. " << endl;
        */

        ++alreadreadlineCnt;
        if ( alreadreadlineCnt == lines2read ) {
            break;
        }
    }
    inFile.close();

    if ( meetSomeError ) {
        cerr << "[ERROR] meet an error : " << errorString << endl;
        return false;
    }

    if ( alreadreadlineCnt < lines2read ) {
        cerr << "[ERROR] missed some lines : Need to read " << lines2read << " arrays , got " << alreadreadlineCnt << " lines actually. " << endl;
        return false;
    }

    fileObject.flush();

    return true;
}

bool readBinary(const string& fileName)
{
    // [ 2, 10]
    ifstream file2Read(fileName.c_str(), ios::in | ios::binary);
    if ( !file2Read ) {
        cerr << "[ERROR] Can't open binary file \"" << fileName << "\" for reading sort test case array. " << endl;
        return false;
    }

    size_t fSz = 0;
	file2Read.seekg(0, ios::end);
	fSz = static_cast<size_t>( file2Read.tellg() );
	file2Read.seekg(0, ios::beg);

    const int c_leftShiftWidth = 4;
    const int cmpPossibilityCnt = 3; // < or == or > 
    int nElementCnt = 0;                                    
    int nPossibilityCnt = 0;                                    
    int possibilities = 0;
    int iArySize = 0;
    vector<int> vecAry;
    // 0: read array.size
    // 1: read array's element value one after another
    int step = 0;
    for ( size_t i = 0; i < fSz; ++i ) {
        char ch = 0;
        file2Read.read(&ch, sizeof(ch) );

        if ( step == 0 ) {
            iArySize =  static_cast<int>( ch );
            cout << "Array.size => [ " << iArySize << " ]" << endl;
            possibilities = pow(cmpPossibilityCnt, iArySize-1);

            nPossibilityCnt = 0;
            nElementCnt = 0;
            vecAry.clear();

            step = 1;
        } else {
            // ch <= 0
            int firstNum  = static_cast<int>( (ch >> c_leftShiftWidth) & 0x0F);
            int secondNum = static_cast<int>( ch  & 0x0F);
            nElementCnt += 2;

            if ( nElementCnt < iArySize ) {
                vecAry.push_back( firstNum );
                vecAry.push_back( secondNum );
            } else {
                // >=
                if ( nElementCnt > iArySize ) {
                    // push first only
                    vecAry.push_back( firstNum );
                } else {
                    // ==
                    vecAry.push_back( firstNum );
                    vecAry.push_back( secondNum );
                }

                nElementCnt = 0;

                cout << "[ "; for( int j = 0; j < static_cast<int>(vecAry.size()); ++j ) { cout << vecAry[j] << (( j != static_cast<int>(vecAry.size()-1) ) ? ", " : ""); }   cout << " ]. " << endl;
                vecAry.clear();

                ++nPossibilityCnt;
                if ( nPossibilityCnt == possibilities ) {
                    cout << endl;
                    step = 0;
                }
            }
        }

    }

    return true;

}

int main(int argc, char* argv[], char* env[])
{
    // [2,10]
    const auto c_filename = "sortTestCase2-10.bin";

    if ( argc == 1 ) {
        ofstream sortTestCaseFile(c_filename,  ios::out | ios::trunc | ios::binary);
        if ( !sortTestCaseFile ) {
            cerr << "[ERROR] Can't create file \"" << c_filename << "\" for saving sort test case array. " << endl;
            sortTestCaseFile.close();
            return -1;
        }


        bool meetError = false;
        string eFile;
        for( auto i = 2; i<=10; ++i ) {
            string finName = "g";
            finName += to_string(i);
            finName += ".txt";

            if ( !readAndEncoding( i, finName, sortTestCaseFile) ) {
                meetError = true;
                eFile = finName;
                break;
            }
            cout << finName << " process successfully. " << endl;
        }

        sortTestCaseFile.flush();
        sortTestCaseFile.close();
        if ( meetError ) {
            cerr << "[ERROR] Can't read file \"" << eFile << "\" for reading sort test case array. " << endl;
            return -1;
        }

    } else {
        // argc > 1
        readBinary(c_filename);
    }

    return 0;
}


