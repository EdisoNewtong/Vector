#include "cmdOptions.h"
#include "charUtil.h"

#include <iostream>
#include <fstream>
#include <exception>

using namespace std;


// static 
const string  CmdOptions::SC_DEFAULT_FILENAME("mathCfg.cfg");

const string  CmdOptions::CFG_PREFIX("--cfgFile=");
const string  CmdOptions::CFG_HINTS("--cfgFile=<FileName>");


const string CmdOptions::SC_CFG_CONTENT{
R"([DebugOption]
    PRINT_RUNTIME_WARNING=0
    PRINT_OPSTACK_ALL=0
    PRINT_SUFFIXEXP_BEFORE=0
    PRINT_SUFFIXEXP_AFTER_BUILD=0
    PRINT_SUFFIXEXP_AFTER_EVALUATE=0
    TRACE_OPSTACK_CHANGE=0
    TRACE_SUFFIXEXP_CHANGE=0
    TRACE_POSNEGAPROPERTY_CHANGE=0
    TRACE_TMPEXP_PROCESS=0
    TRACE_FLOAT_NUMBER_ZERO_WHEN_OP_DIVIDE=0
    PROCESS_TMP_EXPRESSION_WITHOUT_ASSIGNMENT=0
    TREAT_BLANK_STATEMENT_AS_WARNING=0
    TREAT_UNINITIALIZED_VARIBLE_AS_ERROR=0
    PRINT_PARSE_FILE_LENGTH=0
    TRACE_PARSE_TIME_STEP=0

[Flag]
    Dec=1
    Hex=0
    Bin=0
    Oct=0)"};



const vector< pair<string,unsigned long> > CmdOptions::SC_DEBUG_OPTIONS_MAP{
    { string("PRINT_RUNTIME_WARNING="),                         0UL  },
    { string("PRINT_OPSTACK_ALL="),                             1UL  },
    { string("PRINT_SUFFIXEXP_BEFORE="),                        2UL  },
    { string("PRINT_SUFFIXEXP_AFTER_BUILD="),                   3UL  },
    { string("PRINT_SUFFIXEXP_AFTER_EVALUATE="),                4UL  },
    { string("TRACE_OPSTACK_CHANGE="),                          5UL  },
    { string("TRACE_SUFFIXEXP_CHANGE="),                        6UL  },
    { string("TRACE_POSNEGAPROPERTY_CHANGE="),                  7UL  },
    { string("TRACE_TMPEXP_PROCESS="),                          8UL  },
    { string("TRACE_FLOAT_NUMBER_ZERO_WHEN_OP_DIVIDE="),        10UL },
    { string("PROCESS_TMP_EXPRESSION_WITHOUT_ASSIGNMENT="),     11UL },
    { string("TREAT_BLANK_STATEMENT_AS_WARNING="),              12UL },
    { string("TREAT_UNINITIALIZED_VARIBLE_AS_ERROR="),          13UL },
    { string("PRINT_PARSE_FILE_LENGTH="),                       14UL },
    { string("TRACE_PARSE_TIME_STEP="),                         15UL }
};


 
const vector< pair<string,unsigned long> > CmdOptions::SC_FLAG_MAP{
    { string("Dec="), 0UL   },
    { string("Hex="), 1UL   },
    { string("Bin="), 2UL   },
    { string("Oct="), 3UL   }
};





//
// static Member Data Init
//
unsigned long CmdOptions::s_debugOption = 1UL;
unsigned long CmdOptions::s_flag = 1UL;

string CmdOptions::getDefaultCfgFileName()
{
    return CmdOptions::SC_DEFAULT_FILENAME;
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
        auto pos = sOption.find( CmdOptions::CFG_PREFIX );
        if ( pos == 0 ) {
            cfgfile = sOption.substr(  CmdOptions::CFG_PREFIX.size() );
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
        abspath = binPath + CmdOptions::SC_DEFAULT_FILENAME;
        realCfgPath = CmdOptions::SC_DEFAULT_FILENAME;
    }

    // try reading the cfg file in the running path
    ifstream inCfgfile(abspath.c_str(), ios::in);
    if ( !inCfgfile ) {
        bret = false;
        errorMsg = "Can't open config file : " + abspath;
        // Use original value
        inCfgfile.close();
    } else {
        int printableFlag = -1;
        while ( !inCfgfile.eof() ) {
            string line;
            getline(inCfgfile, line);

            auto isMatched1Option = false;
            //
            // Set CmdOptions::s_flag
            //
            if ( printableFlag != 0  ) {
                auto fountIdx = -1;
                unsigned int flagValue = 1;
                for ( int idx = 0; idx < static_cast<int>( CmdOptions::SC_FLAG_MAP.size() ); ++idx ) 
                {
                    auto pr = CmdOptions::SC_FLAG_MAP.at(idx);
                    auto pos = line.find(pr.first);
                    if ( pos != string::npos ) {
                        string restStr =  line.substr( pos + pr.first.size() );
                        if ( restStr.empty() ) {
                            flagValue = 0;
                        } else {
                            try {
                                flagValue = static_cast<unsigned int>( stoi(restStr) );
                            } catch ( const exception& /* e */ ) {
                                flagValue = 0;
                            }
                        }

                        fountIdx = idx;
                        break;
                    }
                }

                if ( fountIdx != -1 ) {
                    isMatched1Option = true;
                    if ( fountIdx == 0 ) {
                        printableFlag = flagValue;
                        if ( flagValue == 0 ) {
                            CmdOptions::s_flag = 0UL;
                        } else {
                            CmdOptions::s_flag |= (1 << CmdOptions::SC_FLAG_MAP.at(fountIdx).second );
                        }
                    } else {
                        if ( flagValue != 0 ) {
                            CmdOptions::s_flag |= (1UL << CmdOptions::SC_FLAG_MAP.at(fountIdx).second );
                        }
                    }
                }
            }


            if ( !isMatched1Option ) {
                //
                // Set CmdOptions::s_debugOption
                //
                auto fountIdx = -1;
                unsigned int flagValue = 1;
                for ( int idx = 0; idx < static_cast<int>( CmdOptions::SC_DEBUG_OPTIONS_MAP.size() ); ++idx ) 
                {
                    auto pr = CmdOptions::SC_DEBUG_OPTIONS_MAP.at(idx);
                    auto pos = line.find(pr.first);
                    if ( pos != string::npos ) {
                        string restStr =  line.substr( pos + pr.first.size() );
                        if ( restStr.empty() ) {
                            flagValue = 0;
                        } else {
                            try {
                                flagValue = static_cast<unsigned int>( stoi(restStr) );
                            } catch ( const exception& /* e */ ) {
                                flagValue = 0;
                            }
                        }

                        fountIdx = idx;
                        break;
                    }
                }

                if ( fountIdx != -1 ) {
                    if ( flagValue != 0 ) {
                        s_debugOption  |= (1UL << CmdOptions::SC_DEBUG_OPTIONS_MAP.at(fountIdx).second );
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




// unsigned int  CmdOptions::getDebugOption()
// {
//     return s_debugOption;
// }



// unsigned int  CmdOptions::getFlag()
// {
//     return s_flag;
// }



// static 
string CmdOptions::getUserManual()
{
    using namespace charutil;

    string strUserManul;
    strUserManul += "====================================================================================================\n";
    strUserManul += "Usage : \n";
    strUserManul += "\t<programName>   ";
    strUserManul += "[";
    strUserManul += CmdOptions::CFG_HINTS;
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
bool CmdOptions::needPrintVarible_16()     { return ( (s_flag >> 1UL) & 0x1UL) != 0; }
bool CmdOptions::needPrintVarible_2()      { return ( (s_flag >> 2UL) & 0x1UL) != 0; }
bool CmdOptions::needPrintVarible_8()      { return ( (s_flag >> 3UL) & 0x1UL) != 0; }




//
// Debug Intermediate Process
//
bool CmdOptions::needPrintParseRuntimeWarning()                { return  (  s_debugOption              & 0x1UL) != 0;  }

bool CmdOptions::needPrintOperatorStackAll()                   { return  ( (s_debugOption >> 1UL)      & 0x1UL) != 0;  }
bool CmdOptions::needPrintSuffixExpressionBefore()             { return  ( (s_debugOption >> 2UL)      & 0x1UL) != 0;  }
bool CmdOptions::needPrintSuffixExpressionAfterBuild()         { return  ( (s_debugOption >> 3UL)      & 0x1UL) != 0;  }
bool CmdOptions::needPrintSuffixExpressionAfterEvaluate()      { return  ( (s_debugOption >> 4UL)      & 0x1UL) != 0;  }

bool CmdOptions::needTraceOperatorStackChange()                { return  ( (s_debugOption >> 5UL)      & 0x1UL) != 0;  }
bool CmdOptions::needTraceSuffixExpressionChange()             { return  ( (s_debugOption >> 6UL)      & 0x1UL) != 0;  }

bool CmdOptions::needTracePositiveNegativePropertyChange()     { return  ( (s_debugOption >> 7UL)      & 0x1UL) != 0;  }
bool CmdOptions::needTraceTmpExpressionProcess()               { return  ( (s_debugOption >> 8UL)      & 0x1UL) != 0;  }

bool CmdOptions::needCheckFloatNumberZero()                    { return  ( (s_debugOption >> 10UL)     & 0x1UL) != 0;  }
bool CmdOptions::needProcessTmpExpressionWithoutAssignment()   { return  ( (s_debugOption >> 11UL)     & 0x1UL) != 0;  }
bool CmdOptions::needTreatBlankStatementAsWarning()            { return  ( (s_debugOption >> 12UL)     & 0x1UL) != 0;  }
bool CmdOptions::needTreatUninitializedVaribleAsError()        { return  ( (s_debugOption >> 13UL)     & 0x1UL) != 0;  }
bool CmdOptions::needPrintSrcCodeLength()                      { return  ( (s_debugOption >> 14UL)     & 0x1UL) != 0;  }
bool CmdOptions::needTraceParseTimeStep()                      { return  ( (s_debugOption >> 15UL)     & 0x1UL) != 0;  }


string CmdOptions::sampleCfgFile()
{
    return CmdOptions::SC_CFG_CONTENT;
}
