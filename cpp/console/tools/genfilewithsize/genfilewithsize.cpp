#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <exception>
#include <ctime>
#include <random>
#include <vector>
#include <map>
using namespace std;

static const int SC_2 = 2;
static const int SC_4 = 4;

static const string SC_B_ARY[SC_2]  = { string("B"), string("b") };
static const string SC_KB_ARY[SC_4] = { string("KB"), string("Kb"), string("kb"), string("kB") };
static const string SC_MB_ARY[SC_4] = { string("MB"), string("Mb"), string("mb"), string("mB") };
static const string SC_GB_ARY[SC_4] = { string("GB"), string("Gb"), string("gb"), string("gB") };
static const string SC_SEP(",");



void printUsage()
{
    // 
    cout << "Usage : " << endl;
    cout << "\t\t$ genfilewithsize <filename> --size=<a-certain-size-by-unit> [--random]   <Enter>" << endl;
    cout << endl;
    cout << "\t\t//                             size=1036 bytes" << endl;
    cout << "\t\t                                     |        " << endl;
    cout << "\t\t$ genfilewithsize output.txt --size=1MB,10KB,2B --random " << endl;
    cout << endl;
}



bool isFileExisted(const char* fileName)
{
    ifstream file( fileName, ios::in | ios::binary);
    return static_cast<bool>( file );
}


bool analyzeArg3(const string& arg3, vector<string>& sizeGrp, vector< pair<string, int> >& unitPartAry, size_t& finalGenSz, string& errorMsg)
{
    auto bHasError = false;
    string arg3rest4size = arg3;
    string::size_type basePos = 0;
    if ( arg3rest4size.empty() ) {
        errorMsg = "[ERROR] No arguments after   \"--size=\"";
        return false;
    }

    while( true )
    {
        string::size_type pos = arg3rest4size.find(SC_SEP, basePos);
        string cut;
        if ( pos != string::npos ) {
            cut = arg3rest4size.substr(basePos, pos-basePos);
            if ( cut.empty() ) {
                errorMsg = "[ERROR] The cut string seperated by ',' is empty.";
                bHasError = true;
                break;
            }

            sizeGrp.push_back( cut );
            basePos = pos+1;
            if ( basePos >= arg3rest4size.size() ) {
                break;
            }
        } else {
            // not found ','
            cut = arg3rest4size.substr(basePos);

            sizeGrp.push_back( cut );
            break;
        }
    }

    if ( bHasError ) {
        return false;
    }

    map<string,int> szMap;
    szMap["B"] = 0; szMap["KB"] = 0; szMap["MB"] = 0; szMap["GB"] = 0;

    errorMsg = "";
    bHasError = false;
    int grpsz = static_cast<int>( sizeGrp.size() );
    for( int i = 0; i < grpsz; ++i ) {
        string element = sizeGrp[i];
        // if(1) { cout << (i+1) << ". \"" << element << "\"" << endl; }
        auto esz = static_cast<int>( element.size() );
        if ( esz < 2 ) {
            bHasError = true;
            errorMsg = (string("[ERROR] ") + element) + " , size_element's size < 2";
            break;
        }

        char lastButOneChar = element[element.size()-2];
        char lastChar = element.back();
        string last2Chars = string(1,lastButOneChar); 
        last2Chars += lastChar;

        auto bHasFound = false;
        for ( auto c : SC_B_ARY ) {
            if( lastChar == c[0] ) {
                bHasFound = true;
                break;
            }
        }

        if ( !bHasFound ) {
            string ss = string("[ERROR] lastChar is b/B. LastCh = ");
            ss += lastChar;
            errorMsg = ss;
            break;
        }

        // B:0  KB:1   MB:2  GB:3
        if ( lastButOneChar>='0' && lastButOneChar<='9' ) {
            //   ??n B   or ??n b
            unitPartAry.push_back( make_pair( element.substr(0,element.size()-1), 0) );
        } else {
            int iBIdx = -1;
            auto bKBFlag = false;
            for( auto s : SC_KB_ARY ) {
                if ( last2Chars == s ) {
                    bKBFlag = true;
                    break;
                }
            }

            if ( !bKBFlag ) {
                auto bMBFlag = false;
                for( auto s : SC_MB_ARY ) {
                    if ( last2Chars == s ) {
                        bMBFlag = true;
                        break;
                    }
                }

                if ( !bMBFlag ) {
                    auto bGBFlag = false;
                    for( auto s : SC_GB_ARY ) {
                        if ( last2Chars == s ) {
                            bGBFlag = true;
                            break;
                        }
                    }

                    if ( bGBFlag ) {
                        iBIdx = 3;
                    }
                } else {
                    iBIdx = 2;
                }
            } else {
                iBIdx = 1;
            }

            if ( iBIdx == -1 ) {
                bHasError = true;
                string ss = string("[ERROR] Unknown unit : ");
                ss += last2Chars;
                errorMsg = ss;
                break;
            }

            unitPartAry.push_back( make_pair( element.substr(0,element.size()-2), iBIdx) );
        }
    }

    if ( bHasError ) {
        return false;
    }

    bHasError = false;
    errorMsg = "";
    for( const auto& pr : unitPartAry ) {
        string strNum = pr.first;
        int unit = pr.second;
        auto isValidNumber = true;
        for( const auto& ch : strNum ) {
            if ( !(ch>='0' && ch<='9') ) {
                isValidNumber = false;
                break;
            }
        }

        if ( !isValidNumber ) {
            bHasError = true;
            errorMsg = string("[ERROR] Invalid number string : ");
            errorMsg += strNum;
            break;
        }

        int n = atoi( strNum.c_str() );
        if ( unit == 0 ) {
            szMap["B"] += n;
        } else if ( unit == 1 ) {
            szMap["KB"] += n;
        } else if ( unit == 2 ) {
            szMap["MB"] += n;
        } else if ( unit == 3 ) {
            szMap["GB"] += n;
        }
    }

    if ( bHasError ) {
        return false;
    }

    //
    // calculate the final generate size by byte(s)
    //
    finalGenSz = 0ull;
    size_t bUnitRate = 0;
    if ( szMap["B"] > 0 ) {
        bUnitRate = 1;
        finalGenSz += (szMap["B"] * bUnitRate);
    }

    if ( szMap["KB"] > 0 ) {
        bUnitRate = 1024ull;
        finalGenSz += (szMap["KB"] * bUnitRate);
    }

    if ( szMap["MB"] > 0 ) {
        bUnitRate = 1024ull * 1024ull;
        finalGenSz += (szMap["MB"] * bUnitRate);
    }

    if ( szMap["GB"] > 0 ) {
        bUnitRate = 1024ull * 1024ull * 1024ull;
        finalGenSz += (szMap["GB"] * bUnitRate);
    }

    return true;
}


void generateFile(const string& fileName, size_t finalGenSz, bool bRandomContentFlag)
{
    const int BUF_SZ = 4096;
    char chBuf[BUF_SZ] = { 0 };
    if ( bRandomContentFlag ) {
        // init buf
        default_random_engine dftEng;       // no   seed constructor
        uniform_int_distribution<int> range(-128, 127);
        for ( int i = 0; i < BUF_SZ; ++i ) {
            chBuf[i] = static_cast<char>( range(dftEng) );
        }
    }

    ofstream outputFile( fileName.c_str() , ios::out | ios::binary | ios::trunc);
	auto seed = time(nullptr);
    int times = finalGenSz / BUF_SZ;
    int rest  = finalGenSz % BUF_SZ;
    for( int i = 0; i < times; ++i ) {
        if ( bRandomContentFlag ) {
            default_random_engine dftEng(seed); // with seed constructor
            uniform_int_distribution<int> range_i(-128, 127);
            for ( int i = 0; i < BUF_SZ; ++i ) {
                chBuf[i] = static_cast<char>( range_i(dftEng) );
            }
        } 
       
        outputFile.write(chBuf, static_cast<streamsize>( BUF_SZ ) );
    }

    if ( rest > 0 ) {
        if ( bRandomContentFlag ) {
            default_random_engine dftEng(seed);
            uniform_int_distribution<int> range_j(-128, 127);
            for ( int i = 0; i < BUF_SZ; ++i ) {
                chBuf[i] = static_cast<char>( range_j(dftEng) );
            }
        }
        outputFile.write(chBuf, static_cast<streamsize>( rest ) );
    }

    outputFile.flush();
    outputFile.close();
    cout << "[DONE] Generate file Succesfully :) , size = " << finalGenSz << " bytes. " << endl;
}


int main(int argc, char* argv[], char* env[])
{
    if ( !(argc>=3 && argc<=4) ) {
        printUsage();
        return -1;
    }

    std::string fileName = argv[1];
    if ( isFileExisted( fileName.c_str() ) ) {
        cout << "[ERROR] The given file has been already existed! Please try another name. " << endl
             << "Program Quit. " 
             << endl;
        return -1;
    }

    const string defaultStr3("--size=");
    const string defaultStr4("--random");
    string arg3 = argv[2];
    if ( arg3.size() < defaultStr3.size() ) {
        cout << "[ERROR] arg3 is not started with \"" << defaultStr3 << "\"" << endl;
        printUsage();
        cout << endl << "Program Quit. " 
             << endl;
        return -1;
    }

    string arg3cut = arg3.substr(0, defaultStr3.size() );
    if ( arg3cut != defaultStr3 ) {
        cout << "[ERROR] arg3 is not started with \"" << defaultStr3 << "\"" << endl;
        printUsage();
        cout << endl << "Program Quit. " 
             << endl;
        return -1;
    }

    //
    // Analyze argument#3 && calculate the final file bytes to generate 
    //
    string arg3rest4size = arg3.substr( defaultStr3.size() );
    vector<string> sizeGrp;
    vector< pair<string, int> > unitPartAry;
    size_t finalGenSz = 0;
    string errorMsg;
    auto bAnaFlag = analyzeArg3(arg3rest4size, sizeGrp, unitPartAry, finalGenSz, errorMsg);
    if ( !bAnaFlag ) {
        cout << errorMsg << endl;
        printUsage();
        return -1;
    }

    // analyze arg#4
    bool bRandomContentFlag = false;
    if ( argc == 4 ) {
        string arg4( argv[3] );
        if ( arg4 != defaultStr4 ) {
            cout << "[ERROR] Unknown argument : " << arg4 << endl;
            printUsage();
            return -1;
        } else {
            bRandomContentFlag = true;
        }
    }

    // [Finally] generate new file by ( fileName && finalGenSz && randomFlag )
    generateFile(fileName, finalGenSz, bRandomContentFlag);

    return 0;
}






