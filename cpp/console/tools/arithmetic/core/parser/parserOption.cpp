#include "parserOption.h"

#include <iostream>
#include <exception>
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
unsigned int ParserOption::s_option = 0;
unsigned int ParserOption::s_flag = 0;



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
            try {
                s_option = static_cast<unsigned int>( stoi(optValue) );
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
            errorMsg = singlequoto + sOption + singlequoto + " is unknown";
            break;
        }
    }

    return bret;
}



unsigned int  ParserOption::getDebugOption()
{
    return s_option;
}



unsigned int  ParserOption::getFlag()
{
    return s_flag;
}



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

