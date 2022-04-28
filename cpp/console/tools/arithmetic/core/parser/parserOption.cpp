#include "parserOption.h"
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
unsigned int ParserOption::s_debugOption = 0;
unsigned int ParserOption::s_flag = 1;



// static 
bool ParserOption::analyzeOption(const vector<string>& args, string& errorMsg)
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



// unsigned int  ParserOption::getDebugOption()
// {
//     return s_debugOption;
// }



// unsigned int  ParserOption::getFlag()
// {
//     return s_flag;
// }



// static 
string ParserOption::getUserManual()
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
bool ParserOption::needPrintVaribleFinally() { return (  s_flag      & 0x1U) != 0; }
bool ParserOption::needPrintVarible_16()     { return ( (s_flag>> 1) & 0x1U) != 0; }
bool ParserOption::needPrintVarible_2()      { return ( (s_flag>> 2) & 0x1U) != 0; }
bool ParserOption::needPrintVarible_8()      { return ( (s_flag>> 3) & 0x1U) != 0; }




//
// Debug Intermediate Process
//
bool ParserOption::needPrintParseRuntimeWarning()            { return  (  s_debugOption          & 0x1) != 0;  }

bool ParserOption::needPrintOperatorStackAll()               { return  ( (s_debugOption>>1)      & 0x1) != 0;  }
bool ParserOption::needPrintSuffixExpressionBefore()         { return  ( (s_debugOption>>2)      & 0x1) != 0;  }
bool ParserOption::needPrintSuffixExpressionAfterBuild()     { return  ( (s_debugOption>>3)      & 0x1) != 0;  }
bool ParserOption::needPrintSuffixExpressionAfterEvaluate()  { return  ( (s_debugOption>>4)      & 0x1) != 0;  }

bool ParserOption::needTraceOperatorStackChange()            { return  ( (s_debugOption>>5)      & 0x1) != 0;  }
bool ParserOption::needTraceSuffixExpressionChange()         { return  ( (s_debugOption>>6)      & 0x1) != 0;  }

bool ParserOption::needTracePositiveNegativePropertyChange() { return  ( (s_debugOption>>7)      & 0x1) != 0;  }
bool ParserOption::needTraceTmpExpressionProcess()           { return  ( (s_debugOption>>8)      & 0x1) != 0;  }

bool ParserOption::needTraceParseTimeStep()                  { return  ( (s_debugOption>>23)     & 0x1) != 0;  }



