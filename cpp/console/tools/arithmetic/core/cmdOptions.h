#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

#include <vector>
#include <string>

class CmdOptions
{
public:
    static bool analyzeOption(const std::vector<std::string>& args, std::string& errorMsg);
    static std::string getUserManual();

    //
    // print varible flag
    //
    static bool needPrintVaribleFinally();
    static bool needPrintVarible_16();
    static bool needPrintVarible_2();
    static bool needPrintVarible_8();

    //
    // debug intermediate process
    //
    static bool needPrintParseRuntimeWarning();

    static bool needPrintOperatorStackAll();
    static bool needPrintSuffixExpressionBefore();
    static bool needPrintSuffixExpressionAfterBuild();
    static bool needPrintSuffixExpressionAfterEvaluate();
    static bool needTraceOperatorStackChange();
    static bool needTraceSuffixExpressionChange();
    static bool needTracePositiveNegativePropertyChange();
    static bool needTraceTmpExpressionProcess();

    static bool needTraceParseTimeStep();


protected:

    // get Option/Flag value
    // static unsigned int  getDebugOption();
    // static unsigned int  getFlag();


    CmdOptions() = delete;
    virtual ~CmdOptions() = delete;

    static unsigned int s_debugOption;
    static unsigned int s_flag;

};


#endif
