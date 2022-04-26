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
    static unsigned int  getDebugOption();
    static unsigned int  getFlag();
protected:
    ParserOption() = delete;
    virtual ~ParserOption() = delete;

    static unsigned int s_option;
    static unsigned int s_flag;

};


#endif
