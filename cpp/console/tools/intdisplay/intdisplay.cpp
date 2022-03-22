#include <iostream>
#include <iomanip>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_set>

#include <limits>
#include <sstream>

#include "myException.h"
#include "charutil.h"
#include "stringNumber.h"

using namespace std;

/*
   - ( negative flag )

   0~9

   a-f  or A-F

   x or X                     // such as : 0xaaa
   b or B ( binary suffix )   // such as : 10001b or 10001B

*/



void printUsgae()
{
    cout << "Usage : " << endl;
    cout << "       $ ./intdisplay    <intnumber>" << endl;
    cout << " decimal format :  (-)1...3"    << endl;
    cout << " hex     format     :  0x1A2B...3F (0~9a-fA-F)" << endl;
    cout << " oct     format     :  01...123 (0~7))" << endl;
    cout << " binary  format     :  1011...01b or 1011...01B (0 or 1 and endwith B)" << endl;
}



using namespace charutil;


int main(int argc, char* argv[], char* env[])
{

    if ( argc != 2 ) {
        printUsgae();
        return -1;
    }


    // argc == 2
    string inputNumber( argv[1] );
    if ( inputNumber.empty() ) {
        cout << "Empty string . Do Nothing !" << endl;
        return 0;
    }

    // inputNumber.size() >= 1
    unordered_set<char> availbleCharSet;
    availbleCharSet.insert('-');
    for( char num = '0'; num <= '9'; ++num ) {
        availbleCharSet.insert(num);
    }

    for( char letter = 'a'; letter  <= 'f'; ++letter ) {
        availbleCharSet.insert(letter);
    }

    for( char letter = 'A'; letter  <= 'F'; ++letter ) {
        availbleCharSet.insert(letter);
    }
    availbleCharSet.insert('x');
    availbleCharSet.insert('X');
    availbleCharSet.insert('b');
    availbleCharSet.insert('B');

    try {
        int len = static_cast<int>(inputNumber.size() );
        string errorMsg;

        int formatType = -1;

        auto negativeCnt = 0;
        auto firstZeroFlag = false;
        auto isLastCh = false;
        string prefix;

        // inputNumber.length >= 1
        for( auto idx = 0; idx < len; ++idx )
        {
            char ch = inputNumber.at(idx);

            if ( availbleCharSet.find(ch) == availbleCharSet.end() ) {
                stringstream ss("[ERROR] char : ");
                if ( ch < 32 || ch == 127 ) {
                    ss << "0x" << std::uppercase << std::hex << static_cast<int>(ch & 0xFFU);
                } else {
                    ss << '\'' <<  ch << '\'';
                } 
                ss << " is not in the valid charset !";

                errorMsg = ss.str();
                throw MyException(errorMsg);
            }

            prefix += ch;

            isLastCh =  (idx == (len-1) );

            if ( idx == 0 ) {
                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // idx == 0
                //
                //////////////////////////////////////////////////////////////////////////////////////////
                if ( !isLastCh ) {
                    bool neFlag = isNegative(ch);
                    bool numberFlag = isNumber(ch);
                    if ( neFlag  ) {
                        ++negativeCnt;
                        formatType = 10;
                    } else if ( numberFlag ) {
                        firstZeroFlag =  ( ch == '0' );
                        if ( !firstZeroFlag ) {
                            formatType = 10;
                        }
                    } else {
                        errorMsg += "\'";
                        errorMsg += ch;
                        errorMsg += "\' is not valid at prefix.";
                        throw MyException(errorMsg);
                    }
                } else {
                    //
                    // the 1st char is also the last char 
                    //
                    if ( !isNumber(ch) ) {
                        errorMsg = string("[ERROR] Invalid integer Format : \"") + inputNumber + string("\"");
                        throw MyException(errorMsg);
                    } else {
                        // inputNumber.size() == 1 and   inputNumber.at(0) is in the range 0~9
                        formatType = 10;
                    }
                }
            } else if ( idx == 1 ) {
                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // idx == 1
                //
                //////////////////////////////////////////////////////////////////////////////////////////
                if ( negativeCnt > 0 ) {
                    // -?
                    if ( isNumber(ch) ) {
                        // -[0-9]
                        formatType = 10;
                    } else {
                        errorMsg = string("[ERROR] Invalid prefix integer Format : \"") + prefix + string("\"");
                        throw MyException(errorMsg);
                    }
                } else {
                    // [0-9]?
                    if ( firstZeroFlag ) {
                        // 0?
                        if ( isxX(ch) ) {
                            // 0x or 0X
                            formatType = 16;
                        } else if ( isNumber(ch)  ) {
                            if ( isOctNumber(ch) ) {
                                formatType = 8;
                            } else {
                                errorMsg = string("[ERROR] Invalid octal integer format : \"") + prefix + string("\"");
                                throw MyException(errorMsg);
                            }
                        } else if( isbB(ch) ) {
                            // 0b or 0B
                            formatType = 2;
                        } else {
                            errorMsg = string("[ERROR] Invalid prefix integer Format : \"") + prefix + string("\"");
                            throw MyException(errorMsg);
                        }
                    } else {
                        // [1-9]?
                        char l1st = inputNumber.at(0);
                        if ( l1st == '1' ) {
                            if ( isNumber(ch) ) {
                                // 1[0-9]
                                formatType = 10;
                            } else if ( isbB(ch) ) {
                                // 1b or 1B
                                formatType = 2;
                            } else {
                                errorMsg = string("[ERROR] Invalid prefix integer Format : \"") + prefix + string("\"");
                                throw MyException(errorMsg);
                            }
                        } else {
                            // [2-9]?
                            if ( isNumber(ch) ) {
                                formatType = 10;
                            } else {
                                errorMsg = string("[ERROR] Invalid prefix integer Format : \"") + prefix + string("\"");
                                throw MyException(errorMsg);
                            }
                        }
                        
                    }
                } 
            } else {
                //
                // idx >=2
                //
                auto validFlag = true;
                if ( formatType == 2 ) {
                    // already meet b/B
                    validFlag = false;
                } else if ( formatType == 8 ) {
                    if ( !isOctNumber(ch) ) {
                        validFlag = false;
                    }
                } else if ( formatType == 10 ) {
                    if ( !isNumber(ch) ) {
                        validFlag = false;
                    }
                } else if ( formatType == 16 ) {
                    if( !isHexNumber(ch) ) {
                        validFlag = false;
                    }
                }

                auto throwFlag = false;
                if( formatType == 2 ) {
                    throwFlag = true;
                } else {
                    if( !validFlag ) {
                        if ( isbB(ch) ) {
                            formatType = 2;
                        } else {
                            throwFlag = true;
                        }
                    }
                }

                if ( throwFlag ) {
                    errorMsg = string("[ERROR] Invalid interger format : \"") + prefix + string("\"");
                    throw MyException(errorMsg);
                }
            }
        } // end for


        if ( formatType == -1 ) {
            errorMsg = string("[ERROR] Invalid integer Format : \"") + inputNumber + string("\"");
            throw MyException(errorMsg);
        } else {
            // string convertResult;
            StringNumber strnumber(inputNumber, formatType);
            cout << strnumber.getDetail() << endl;

            /*
            string binStr          = strnumber.calcBinary();
            if ( !isbB(binStr.back()) ) {
                binStr += "B";
            }

            string octalStr        = strnumber.calcOctal();
            if ( octalStr.front() != '0' || octalStr.size() == 1 ) {
                octalStr = string("0") + octalStr;
            }

            string unsignedDecimal = strnumber.calcDecimal();
            string signedDecimal   = strnumber.calcDecimalWithSignedFlag();
            string hexStr          = strnumber.calcHex();
            string hexPrefix = hexStr.substr(0,2);
            if ( hexPrefix != "0x" && hexPrefix != "0X" ) {
                hexStr = string("0x") + hexStr;
            }
            
            cout << "Binary           : " << binStr          << endl;
            cout << "Octal            : " << octalStr        << endl;
            cout << "Decimal unsigned : " << unsignedDecimal << endl;
            cout << "Decimal   signed : " << signedDecimal   << endl;
            cout << "Hex              : " << hexStr          << endl;
            */
        }

    }
    catch( const MyException& e ) {
        cout << "[ERROR] : Meet a self-defined exception : " << e.what() << endl;
    }
    catch( const exception& e ) {
        cout << "[ERROR] : Meet a normal exception : " << e.what() << endl;
    }


    return 0;
}
