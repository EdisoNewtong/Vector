#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

#include <vector>
#include <string>
#include <utility>

class CmdOptions
{
public:
    static std::pair<bool,std::string>   parseCmdArgs(const std::vector<std::string>& args);
    static std::pair<bool,std::string>   parseCfgFile(bool hasCmdArgs, const std::string& cfgfile, const std::string& binPath);

    static std::string getUserManual();
    static std::string getDefaultCfgFileName();
    static std::string sampleCfgFile();

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

    static bool needPrintSrcCodeLength();
    static bool needTraceParseTimeStep();


protected:

    // get Option/Flag value
    // static unsigned int  getDebugOption();
    // static unsigned int  getFlag();


    CmdOptions() = delete;
    virtual ~CmdOptions() = delete;

    static unsigned int s_debugOption;
    static unsigned int s_flag;

    static const std::string SC_DEFAULT_FILENAME;
    static const std::string CFG_PREFIX; // ("--cfgFile=");
    static const std::string CFG_HINTS;  // ("--cfgFile=<filename>");
    static const std::string SC_CFG_CONTENT;

    static const std::vector< std::pair<std::string,unsigned int> > SC_DEBUG_OPTIONS_MAP;
    static const std::vector< std::pair<std::string,unsigned int> > SC_FLAG_MAP;



};


#endif
