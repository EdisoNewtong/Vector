#include "parserOptions.h"

using namespace std;

// static varible init
const string ParserOptions::s_optKeyWordprefix_1 = "--option=";
const string ParserOptions::s_optKeyWord_1       = "--option=<number>";
const string ParserOptions::s_optKeyWordprefix_2 = "--flags=";
const string ParserOptions::s_optKeyWord_2       = "--flags=<number>";
int          ParserOptions::s_debugOption        = 0;
int          ParserOptions::s_flag               = 0;


// static 
bool ParserOptions::analyzeOptionArgs(const list<string> args, string& errorMsg)
{
	auto bret = true;

	for ( const auto& sOption : args ) 
	{
		auto pos1 = sOption.find(s_optKeyWordprefix_1);
		auto pos2 = sOption.find(s_optKeyWordprefix_2);

		if ( pos1 != string::npos ) {
			// set debug option
			string optValue = sOption.substr( pos1 + s_optKeyWordprefix_1.size() );
			s_debugOption = atoi( optValue.c_str() );
		} else if ( pos2 != string::npos ) {
			string flagValue = sOption.substr( pos2 + s_optKeyWordprefix_2.size() );
			s_flag = atoi( flagValue.c_str() );
		} else {
			bret = false;
			errorMsg = sOption;
			break;
		}
	}

	return bret;

}


int  ParserOptions::getDebugOption()
{
	return s_debugOption;
}

int  ParserOptions::getFlag()
{
	return s_flag;
}

// static 
string ParserOptions::getUserManual()
{
	string strUserManul;
	strUserManul += "==================================================\n";
	strUserManul += "Usage : \n";
	strUserManul += "\t<programName> ";
	strUserManul += s_optKeyWord_1;
	strUserManul += "  ";
	strUserManul += s_optKeyWord_2;

	strUserManul += "   ";
	strUserManul += "<src-FileName>\n";
	strUserManul += "==================================================\n";

	return strUserManul;
}

