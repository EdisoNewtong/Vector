#include "cmdOptions.h"
#include "charUtil.h"

#include <iostream>
#include <exception>
using namespace std;


namespace
{
    const string OPT_PREFIX("--debugOption=");
    const string OPT_HINTS("--debugOption=<IntegerNumber>");

    const string FLGS_PREFIX("--flag=");
    const string FLGS_HINTS("--flag=<IntegerNumber>");
}

//
// static Member Data Init
//
unsigned int CmdOptions::s_debugOption = 0;
unsigned int CmdOptions::s_flag = 1;



// static 
bool CmdOptions::analyzeOption(const vector<string>& args, string& errorMsg)
{
    using namespace charutil;

    const string singlequoto("\'");
    auto bret = true;
    errorMsg = "";

    for ( const auto& sOption : args ) 
    {
        auto pos1 = sOption.find(OPT_PREFIX);
        auto pos2 = sOption.find(FLGS_PREFIX);

        if ( pos1 == 0  ) {
            // set debug option
            string optValue = sOption.substr(  OPT_PREFIX.size() );
            try {
                s_debugOption = static_cast<unsigned int>( stoi(optValue) );
            } catch ( const exception& e ) {
                bret = false;
                cout << "\"" << optValue << "\" after " << OPT_PREFIX << " can't convert to a number. " << endl;
            }
        } else if ( pos2 == 0 ) {
            string flagValue = sOption.substr(  FLGS_PREFIX.size() );
            try {
                s_flag = static_cast<unsigned int>( stoi( flagValue ) );
            } catch ( const exception& e ) {
                bret = false;
                cout << "\"" << flagValue << "\" after " << FLGS_PREFIX  << " can't convert to a number. " << endl;
            }
        } else {
            bret = false;
            errorMsg = SINGLE_QUOTO + sOption + SINGLE_QUOTO + " is unknown";
            break;
        }
    }

    return bret;
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
    string strUserManul;
    strUserManul += "====================================================================================================\n";
    strUserManul += "Usage : \n";
    strUserManul += "\t<programName>   ";
    strUserManul += "[";
    strUserManul += OPT_HINTS;
    strUserManul += "]";
    strUserManul += "   ";
    strUserManul += "[";
    strUserManul += FLGS_HINTS;
    strUserManul += "]";

    strUserManul += "   ";
    strUserManul += "<sourceCode>\n";
    strUserManul += "====================================================================================================\n";

    return strUserManul;
}





//
// print varible flag
//
bool CmdOptions::needPrintVaribleFinally() { return (  s_flag      & 0x1U) != 0; }
bool CmdOptions::needPrintVarible_16()     { return ( (s_flag>> 1) & 0x1U) != 0; }
bool CmdOptions::needPrintVarible_2()      { return ( (s_flag>> 2) & 0x1U) != 0; }
bool CmdOptions::needPrintVarible_8()      { return ( (s_flag>> 3) & 0x1U) != 0; }




//
// Debug Intermediate Process
//
bool CmdOptions::needPrintParseRuntimeWarning()            { return  (  s_debugOption          & 0x1) != 0;  }

bool CmdOptions::needPrintOperatorStackAll()               { return  ( (s_debugOption>>1)      & 0x1) != 0;  }
bool CmdOptions::needPrintSuffixExpressionBefore()         { return  ( (s_debugOption>>2)      & 0x1) != 0;  }
bool CmdOptions::needPrintSuffixExpressionAfterBuild()     { return  ( (s_debugOption>>3)      & 0x1) != 0;  }
bool CmdOptions::needPrintSuffixExpressionAfterEvaluate()  { return  ( (s_debugOption>>4)      & 0x1) != 0;  }

bool CmdOptions::needTraceOperatorStackChange()            { return  ( (s_debugOption>>5)      & 0x1) != 0;  }
bool CmdOptions::needTraceSuffixExpressionChange()         { return  ( (s_debugOption>>6)      & 0x1) != 0;  }

bool CmdOptions::needTracePositiveNegativePropertyChange() { return  ( (s_debugOption>>7)      & 0x1) != 0;  }
bool CmdOptions::needTraceTmpExpressionProcess()           { return  ( (s_debugOption>>8)      & 0x1) != 0;  }

bool CmdOptions::needTraceParseTimeStep()                  { return  ( (s_debugOption>>23)     & 0x1) != 0;  }



