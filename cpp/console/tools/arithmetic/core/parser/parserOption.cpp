#include "parserOption.h"

using namespace std;

namespace
{
    const string OPT_PREFIX("--option=");
    const string OPT_HINTS("--option=<IntegerNumber>");

    const string FLGS_PREFIX("--flag=");
    const string FLGS_HINTS("--flag=<IntegerNumber>");
}

//
// static Member Data Init
//
int ParserOption::s_option = 0;
int ParserOption::s_flag = 0;



// static 
bool ParserOption::analyzeOption(const vector<string>& args, string& errorMsg)
{
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
            s_option = stoi(optValue);
        } else if ( pos2 == 0 ) {
            string flagValue = sOption.substr(  FLGS_PREFIX.size() );
            s_flag = stoi( flagValue );
        } else {
            bret = false;
            errorMsg = singlequoto + sOption + singlequoto + " is unknown";
            break;
        }
    }

    return bret;
}



int  ParserOption::getDebugOption()
{
    return s_option;
}

int  ParserOption::getFlag()
{
    return s_flag;
}

// static 
string ParserOption::getUserManual()
{
    string strUserManul;
    strUserManul += "==================================================\n";
    strUserManul += "Usage : \n";
    strUserManul += "\t<programName> ";
    strUserManul += OPT_HINTS;
    strUserManul += "  ";
    strUserManul += FLGS_HINTS;

    strUserManul += "   ";
    strUserManul += "<sourceCode>\n";
    strUserManul += "==================================================\n";

    return strUserManul;
}

