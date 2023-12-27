#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

#include <vector>
#include <string>
#include <utility>

#include <QtGlobal>
#include <QString>

class CmdOptions
{
public:
    static std::pair<bool,std::string>   parseCmdArgs(const std::vector<std::string>& args);
    static std::pair<bool,std::string>   parseCfgFile(bool hasCmdArgs, const std::string& cfgfile, const std::string& binPath);


    static std::string getFunctionsList();
    static std::string getUserManual();
    static std::string getDefaultCfgFileName();
    static std::string sampleCfgFile();

    //
    // print varible flag
    //
    static bool needPrintVaribleFinally();

    static bool needPrintVarible_10();
    static bool needPrintVarible_16();
    static bool needPrintVarible_2();
    static bool needPrintVarible_8();
    static bool isOctalDefaultStyle();

    static bool needDoBitwiseShift_Mod_Algorithm();


	/***************************************************

	     int a = 1;

	     int add(int n1, int n2) { return n1+n2; }

	    c call  : add( a, a = 10);  =>   n1 <-  1,  n2 = 10 ,  return  1+10; return 11;
	    stdcall : add( a, a = 10);  =>   n1 <- 10,  n2 = 10 ,  return 10+10; return 20;

	****************************************************/
    static bool isFunctionUseStdCallConvension();

    static bool isPrintVaribleFormatValid();


    static quint32  getFlag();
    static quint32  getDebugOption();
    static void     forceSetFlag(quint32 val);
    static void     forceSetOption(quint32 val);

    static void     toggleFlagBit(quint32 iBit);
    static void     toggleDebugOptionBit(quint32 iBit);

    //
    // debug intermediate process
    //
    static bool needPrintParseRuntimeWarning();

    static bool needPrintOperatorStackAll();
    static bool needPrintSuffixExpressionBefore();
    static bool needPrintSuffixExpressionAfterBuild();
    static bool needPrintSuffixExpressionAfterEvaluate();

    static bool needTraceOperatorStackSuffixExpressionChange();
    // static bool needTraceOperatorStackChange();
    // static bool needTraceSuffixExpressionChange();

    static bool needTracePositiveNegativePropertyChange();
    static bool needTraceTmpExpressionProcess();
    static QString printDataTypeRangeIfNecessary();

    static bool needCheckFixedLiteralIntRangeWhenAssign();
    static bool needTreatSignedIntergerBitShiftAsWarning();
    static bool needCheckFloatNumberZero();
    static bool needProcessTmpExpressionWithoutAssignment();
    static bool needTreatBlankStatementAsWarning();
    static bool needTreatUninitializedVaribleAsError();
    static bool needTraceParseTimeStep();
    static bool needPrintSrcCodeLength();

    static bool needPrintDataTypeRange();
	static bool needTraceUninitializedVaribleWhenEvaluatingExpression();
protected:
    // get Option/Flag value
    // static unsigned int  getDebugOption();
    // static unsigned int  getFlag();

    CmdOptions() { }
    virtual ~CmdOptions() { }

    static quint32 s_debugOption;
    static quint32 s_flag;

    static const std::string SC_DEFAULT_FILENAME;
    static const std::string CFG_PREFIX; 
    static const std::string CFG_HINTS;
    static const std::string SC_CFG_CONTENT;

    static const std::vector< std::pair<std::string, quint32> > SC_DEBUG_OPTIONS_MAP;
    static const std::vector< std::pair<std::string, quint32> > SC_FLAG_MAP;



};


#endif
