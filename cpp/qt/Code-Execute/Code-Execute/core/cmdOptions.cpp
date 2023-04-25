#include "cmdOptions.h"
#include "charUtil.h"
#include "dataValue.h"

#include <iostream>
#include <fstream>
#include <exception>
#include <limits>

#include <QTextStream>

using namespace std;


// static 
const string  CmdOptions::SC_DEFAULT_FILENAME("mathCfg.cfg");

const string  CmdOptions::CFG_PREFIX("--cfgFile=");
const string  CmdOptions::CFG_HINTS("--cfgFile=<FileName>");


const string CmdOptions::SC_CFG_CONTENT{
R"([Flag]
    ENABLE_PRINT_VARLIST_AT_LAST = 0
    Dec = 1
    Hex = 0
    Bin = 0
    Oct = 0
    // OctStyle : 0   =>     0xxxx    
    // OctStyle : 1   =>   (0)xxxx
    OctStyle = 0
    //
    BitwiseShift_Mod_Algorithm = 0

[DebugOption]
    PRINT_RUNTIME_WARNING = 1
    PRINT_OPSTACK_ALL = 0
    PRINT_SUFFIXEXP_BEFORE = 0
    PRINT_SUFFIXEXP_AFTER_BUILD = 0
    PRINT_SUFFIXEXP_AFTER_EVALUATE = 0
    TRACE_OPSTACK_SUFFIXEXPR_CHANGE = 0
    TRACE_POSNEGAPROPERTY_CHANGE = 0
    TRACE_TMPEXP_PROCESS = 0
    NEED_PRINT_DATA_TYPE_RANGE = 0

    NEED_TREAT_SIGNED_INTEGER_BIT_SHIFT_AS_WARING = 0
    TRACE_FLOAT_NUMBER_ZERO_WHEN_OP_DIVIDE = 1
    PROCESS_TMP_EXPRESSION_WITHOUT_ASSIGNMENT = 1
    TREAT_BLANK_STATEMENT_AS_WARNING = 0
    TREAT_UNINITIALIZED_VARIBLE_AS_ERROR = 0
    PRINT_PARSE_FILE_LENGTH = 0
    NEED_CHECK_FIXED_LITERAL_INT_RANGE_WHEN_ASSIGN = 1

    TRACE_PARSE_TIME_STEP = 0
)"
};


const vector< pair<string,quint32> > CmdOptions::SC_DEBUG_OPTIONS_MAP{
    { string("PRINT_RUNTIME_WARNING = "),                              0UL  },
    { string("PRINT_OPSTACK_ALL = "),                                  1UL  },
    { string("PRINT_SUFFIXEXP_BEFORE = "),                             2UL  },
    { string("PRINT_SUFFIXEXP_AFTER_BUILD = "),                        3UL  },
    { string("PRINT_SUFFIXEXP_AFTER_EVALUATE = "),                     4UL  },
    { string("TRACE_OPSTACK_SUFFIXEXPR_CHANGE = "),                    5UL  },
    { string("TRACE_POSNEGAPROPERTY_CHANGE = "),                       6UL  },
    { string("TRACE_TMPEXP_PROCESS = "),                               7UL  },
    { string("NEED_PRINT_DATA_TYPE_RANGE = "),                         8UL  },

    { string("NEED_TREAT_SIGNED_INTEGER_BIT_SHIFT_AS_WARING = "),      9UL },
    { string("TRACE_FLOAT_NUMBER_ZERO_WHEN_OP_DIVIDE = "),            10UL },
    { string("PROCESS_TMP_EXPRESSION_WITHOUT_ASSIGNMENT = "),         11UL },
    { string("TREAT_BLANK_STATEMENT_AS_WARNING = "),                  12UL },
    { string("TREAT_UNINITIALIZED_VARIBLE_AS_ERROR = "),              13UL },
    { string("PRINT_PARSE_FILE_LENGTH = "),                           14UL },
    { string("NEED_CHECK_FIXED_LITERAL_INT_RANGE_WHEN_ASSIGN = "),    15UL },

    { string("TRACE_PARSE_TIME_STEP = "),                             23UL }
};


 
const vector< pair<string,quint32> > CmdOptions::SC_FLAG_MAP{
    { string("ENABLE_PRINT_VARLIST_AT_LAST = "), 0UL   },
    { string("Dec = "),      1UL   },
    { string("Hex = "),      2UL   },
    { string("Bin = "),      3UL   },
    { string("Oct = "),      4UL   },
    { string("OctStyle = "), 5UL   },
    //////////////////////////////////////////////////
    { string("BitwiseShift_Mod_Algorithm = "), 6UL   }

    // OctStyle = 0
};





//
// static Member Data Init
//
quint32 CmdOptions::s_debugOption = 0x8001UL;
quint32 CmdOptions::s_flag = 3UL;

string CmdOptions::getDefaultCfgFileName()
{
    return SC_DEFAULT_FILENAME;
}


// static 
pair<bool,string> CmdOptions::parseCmdArgs(const vector<string>& args)
{
    using namespace charutil;
    string errorMsg;

    vector<string> unknowArgList;
    string cfgfile;
    auto bret = false;

    int matchCnt = 0;

    for ( const auto& sOption : args ) 
    {
        auto pos = sOption.find( CFG_PREFIX );
        if ( pos == 0 ) {
            cfgfile = sOption.substr(  CFG_PREFIX.size() );
            ++matchCnt;
        } else {
            unknowArgList.push_back(sOption);
        }
    }

    if ( !unknowArgList.empty() ) {
        for( auto unknownArg : unknowArgList ) {
            errorMsg += ("Unknown Option : " + unknownArg + NEW_LINE_N);
        }
        bret = false;
    } else {
        if ( matchCnt == 1 ) {
            // bret = true;
            if ( cfgfile.empty() ) {
                errorMsg = "missing config file name";
                bret = false;
            } else {
                errorMsg = cfgfile;
                bret = true;
            } 
        } else {
            errorMsg = "more or less for matched argument != 1  , =>  " + to_string(matchCnt);
            bret = false;
        }
    }


    return make_pair(bret, errorMsg);
}



// static 
pair<bool,string> CmdOptions::parseCfgFile(bool hasCmdArgs, const string& cfgfile, const string& binPath)
{
    string errorMsg;
    bool bret = false;

    string realCfgPath;
    string abspath;
    if ( hasCmdArgs ) {
        abspath = cfgfile;
        realCfgPath = cfgfile;
    } else {
        abspath = binPath + SC_DEFAULT_FILENAME;
        realCfgPath = SC_DEFAULT_FILENAME;
    }

    // try reading the cfg file in the running path
    ifstream inCfgfile(abspath.c_str(), ios::in);
    if ( !inCfgfile ) {
        bret = false;
        errorMsg = "Can't open config file : " + abspath;
        // Use original value
        inCfgfile.close();
    } else {
        while ( !inCfgfile.eof() ) {
            string line;
            getline(inCfgfile, line);

            auto isMatched1Option = false;

            //
            // Set CmdOptions::s_flag
            //
            auto foundIdx = -1;
            quint32 flagValue = 1;
            for ( int idx = 0; idx < static_cast<int>( SC_FLAG_MAP.size() ); ++idx ) 
            {
                auto pr = SC_FLAG_MAP.at(idx);
                auto pos = line.find(pr.first);
                if ( pos != string::npos ) {
                    string restStr =  line.substr( pos + pr.first.size() );
                    if ( restStr.empty() ) {
                        flagValue = 0;
                    } else {
                        try {
                            flagValue = static_cast<quint32>( stoi(restStr) );
                        } catch ( const exception& /* e */ ) {
                            flagValue = 0;
                        }
                    }

                    foundIdx = idx;
                    break;
                }
            }

            if ( foundIdx != -1 ) {
                isMatched1Option = true;
                if ( flagValue != 0 ) { // set a given bit to 1
                    s_flag |= (1UL << SC_FLAG_MAP.at(foundIdx).second );
                } else { // set a given bit to 0
                    s_flag &= ( ~(1UL << SC_FLAG_MAP.at(foundIdx).second ) );
                }
            }


            if ( !isMatched1Option ) {
                //
                // Set CmdOptions::s_debugOption
                //
                auto foundIdx2 = -1;
                quint32 flagValue = 1;
                for ( int idx = 0; idx < static_cast<int>( SC_DEBUG_OPTIONS_MAP.size() ); ++idx ) 
                {
                    auto pr = SC_DEBUG_OPTIONS_MAP.at(idx);
                    auto pos = line.find(pr.first);
                    if ( pos != string::npos ) {
                        string restStr =  line.substr( pos + pr.first.size() );
                        if ( restStr.empty() ) {
                            flagValue = 0;
                        } else {
                            try {
                                flagValue = static_cast<quint32>( stoi(restStr) );
                            } catch ( const exception& /* e */ ) {
                                flagValue = 0;
                            }
                        }

                        foundIdx2 = idx;
                        break;
                    }
                }

                if ( foundIdx2 != -1 ) {
                    if ( flagValue != 0 ) {
                        s_debugOption  |= (1UL << SC_DEBUG_OPTIONS_MAP.at(foundIdx2).second );
                    } else {
                        s_debugOption &= ( ~(1UL << SC_DEBUG_OPTIONS_MAP.at(foundIdx2).second ) );
                    }
                }
            }

        }

        inCfgfile.close();
        bret = true;
        errorMsg = realCfgPath;
    }


    return make_pair(bret , errorMsg);

}







// unsigned int  CmdOptions::getFlag()
// {
//     return s_flag;
// }


// static 
QString CmdOptions::printDataTypeRangeIfNecessary()
{
    using namespace intTypeRange;

    QString retStr;
    QTextStream ts(&retStr);

    if ( needPrintDataTypeRange() ) {
        ts << "===========================================================================" << endl;

        size_t bytes = sizeof(char);
        size_t bits = bytes * 8;
        ts << "char : " << bytes << (bytes > 1 ? " bytes" : " byte") << " => "<< bits << " bits. "
             << "char is" << (numeric_limits<char>::is_signed ? " <S>igned " : " <Un>signed .") << endl
             << "\t" << "signed  : "    << static_cast<int>(sch_min) << " ~ " << static_cast<int>(sch_max)
             << "\t" << ", unsigned  : " << static_cast<int>(uch_min) << " ~ " << static_cast<int>(uch_max) 
             << endl
             << endl;

        bytes = sizeof(short);
        bits = bytes * 8;
        ts << "short : " << bytes << (bytes > 1 ? " bytes" : " byte") << " => " << bits << " bits. " << endl
             << "\t" << "signed : "      << sshort_min << " ~ " << sshort_max
             << "\t" << " , unsigned  : " << ushort_min << " ~ " << ushort_max
             << endl
             << endl;


        auto equal1 =  (sizeof(int) == sizeof(long) );
        bytes = sizeof(int);
        bits = bytes * 8;
        ts << "int : " << bytes << (bytes > 1 ? " bytes" : " byte") << " => " << bits << " bits. ";
        if ( equal1 ) {
            ts << "| int <--> long | are same size. " << endl;
        } else {
            ts << endl;
        }
        ts << "\t" << "signed : " << sint_min << " ~ " << sint_max
             << "\t" << " , unsigned : " << uint_min << " ~ " << uint_max
             << endl
             << endl;

        auto equal2 = ( sizeof(long) == sizeof(long long) );
        bytes = sizeof(long long);
        bits = bytes * 8;
        ts << "long long : " << bytes << (bytes > 1 ? " bytes" : " byte") << " => " << bits << " bits. ";
        if ( equal2 ) {
            ts << "| long long <--> long | are same size. " << endl;
        } else {
            ts << endl;
        }
        ts << "\t" << "signed : "      << slonglong_min << " ~ " << slonglong_max
             << "\t" << " , unsigned : " << ulonglong_min << " ~ " << ulonglong_max
             << endl
             << endl;
        ts << "===========================================================================" << endl;
    }

    ts.flush();

    return  retStr;
}


// static 
string CmdOptions::getUserManual()
{
    using namespace charutil;

    string strUserManul;
    strUserManul += "====================================================================================================\n";
    strUserManul += "Usage : \n";
    strUserManul += "\t<programName>   ";
    strUserManul += "[";
    strUserManul += CFG_HINTS;
    strUserManul += "]";

    strUserManul += SPACE_2;
    strUserManul += "<sourceCode>\n";
    strUserManul += "====================================================================================================\n";

    return strUserManul;
}





//
// print varible flag
//
bool CmdOptions::needPrintVaribleFinally() { return (  s_flag         & 0x1UL) != 0; }

bool CmdOptions::needPrintVarible_10()     { return ( (s_flag >> 1UL) & 0x1UL) != 0; }
bool CmdOptions::needPrintVarible_16()     { return ( (s_flag >> 2UL) & 0x1UL) != 0; }
bool CmdOptions::needPrintVarible_2()      { return ( (s_flag >> 3UL) & 0x1UL) != 0; }
bool CmdOptions::needPrintVarible_8()      { return ( (s_flag >> 4UL) & 0x1UL) != 0; }
// 0 : default  0xxxxx    1: special format (0)xxxx
bool CmdOptions::isOctalDefaultStyle()     { return ( (s_flag >> 5UL) & 0x1UL) == 0; }
bool CmdOptions::needDoBitwiseShift_Mod_Algorithm() { return ( (s_flag >> 6UL) & 0x1UL) == 0; }

quint32 CmdOptions::getFlag()         { return s_flag; }
quint32 CmdOptions::getDebugOption() { return s_debugOption; }

void CmdOptions::forceSetFlag(quint32 val)   { s_flag = val; }
void CmdOptions::forceSetOption(quint32 val) { s_debugOption = val; }

void CmdOptions::toggleFlagBit(quint32 iBit)        { s_flag ^= (1UL << iBit); }
void CmdOptions::toggleDebugOptionBit(quint32 iBit) { s_debugOption ^= (1UL << iBit); }

bool CmdOptions::isPrintVaribleFormatValid() 
{ 
   return    CmdOptions::needPrintVarible_10() 
          || CmdOptions::needPrintVarible_16()
          || CmdOptions::needPrintVarible_2()
          || CmdOptions::needPrintVarible_8(); 
}



//
// Debug Intermediate Process
//
bool CmdOptions::needPrintParseRuntimeWarning()                { return  (  s_debugOption              & 0x1UL) != 0;  }

bool CmdOptions::needPrintOperatorStackAll()                   { return  ( (s_debugOption >> 1UL)      & 0x1UL) != 0;  }
bool CmdOptions::needPrintSuffixExpressionBefore()             { return  ( (s_debugOption >> 2UL)      & 0x1UL) != 0;  }
bool CmdOptions::needPrintSuffixExpressionAfterBuild()         { return  ( (s_debugOption >> 3UL)      & 0x1UL) != 0;  }
bool CmdOptions::needPrintSuffixExpressionAfterEvaluate()      { return  ( (s_debugOption >> 4UL)      & 0x1UL) != 0;  }
bool CmdOptions::needTraceOperatorStackSuffixExpressionChange(){ return  ( (s_debugOption >> 5UL)      & 0x1UL) != 0;  } 
bool CmdOptions::needTracePositiveNegativePropertyChange()     { return  ( (s_debugOption >> 6UL)      & 0x1UL) != 0;  }
bool CmdOptions::needTraceTmpExpressionProcess()               { return  ( (s_debugOption >> 7UL)      & 0x1UL) != 0;  }
bool CmdOptions::needPrintDataTypeRange()                      { return  ( (s_debugOption >> 8UL)      & 0x1UL) != 0;  }


bool CmdOptions::needTreatSignedIntergerBitShiftAsWarning()    { return  ( (s_debugOption >>  9UL)     & 0x1UL) != 0;  }
bool CmdOptions::needCheckFloatNumberZero()                    { return  ( (s_debugOption >> 10UL)     & 0x1UL) != 0;  }
bool CmdOptions::needProcessTmpExpressionWithoutAssignment()   { return  ( (s_debugOption >> 11UL)     & 0x1UL) != 0;  }
bool CmdOptions::needTreatBlankStatementAsWarning()            { return  ( (s_debugOption >> 12UL)     & 0x1UL) != 0;  }
bool CmdOptions::needTreatUninitializedVaribleAsError()        { return  ( (s_debugOption >> 13UL)     & 0x1UL) != 0;  }
bool CmdOptions::needPrintSrcCodeLength()                      { return  ( (s_debugOption >> 14UL)     & 0x1UL) != 0;  }
bool CmdOptions::needCheckFixedLiteralIntRangeWhenAssign()     { return  ( (s_debugOption >> 15UL)     & 0x1UL) != 0;  }

bool CmdOptions::needTraceParseTimeStep()                      { return  ( (s_debugOption >> 23UL)     & 0x1UL) != 0;  }


string CmdOptions::sampleCfgFile()
{
    return SC_CFG_CONTENT;
}