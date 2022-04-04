#ifndef PARSER_OPTIONS_H
#define PARSER_OPTIONS_H

#include <list>
#include <string>

class ParserOptions
{
public:
	static bool analyzeOptionArgs(const std::list<std::string> args, std::string& error);
	static int  getDebugOption();
	static int  getFlag();
	static std::string getUserManual();
protected:
	static const std::string s_optKeyWordprefix_1; // = "--option=";
	static const std::string s_optKeyWord_1;       // = "--option=<number>";
	static const std::string s_optKeyWordprefix_2; // = "--flags=";
	static const std::string s_optKeyWord_2;       // = "--flags=<number>";

	static int s_debugOption;
	static int s_flag;
};


#endif
