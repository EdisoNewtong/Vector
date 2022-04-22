#ifndef PARSER_OPTION_H
#define PARSER_OPTION_H

#include <vector>
#include <string>

class ParserOption
{
public:
    static bool analyzeOption(const std::vector<std::string>& args, std::string& errorMsg);
    static std::string getUserManual();

    // get Option/Flag value
    static int  getDebugOption();
    static int  getFlag();
protected:
    ParserOption() = delete;
    virtual ~ParserOption() = delete;

    static int s_option;
    static int s_flag;

};


#endif
