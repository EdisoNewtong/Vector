#ifndef TOKEN_MGR_H
#define TOKEN_MGR_H


#include "tokenBase.h"
#include "commonEnum.h"
#include "variblePool.h"
#include "dataValue.h"
#include "dataTypeUtil.h"
#include "functionBase.h"
#include <string>
// #include <stack>
#include <list>
#include <vector>
#include <unordered_map>
#include <utility>

#define OPERATOR_CNT       25
//    + - * / %     : 5
//    & | ^ ~ << >> : 6
//    ( ) =         : 3
//    += -= *= /* %=     &= |= ^=    <<=  >>=    10
//    ,             : 1
// ----------------------
//  Total          =  25 = ( 5 + 6 + 3 + 10 + 1)
//

class TokenMgr
{
protected:
    struct OpPairCfg
    {
        OpPairCfg(bool flag1, bool flag2) : closeAvaliable(flag1), seperateAvaliable(flag2) { };

        bool           closeAvaliable;    //   permission for  the 2 operators are adjacent without any Space or Comment
        bool           seperateAvaliable; //   permission for  the 2 operators are not adjacent but there is at least some Space or Comment token in the middle
    };

public:
    static constexpr int s_TABLE_SIZE = OPERATOR_CNT * OPERATOR_CNT;

    // Singleton Instance
    static void init();
    static void release();
    static TokenMgr* getInstance();

    static void setUnInitializedVaribleAsError(bool flag);
    static void setNeedTreatBlankStatementAsWarning(bool flag);
public:
    // Core Core Core : Key Logic
    void pushToken(TokenBase* pToken);
    std::pair<bool,std::string> isLastValidTokenSemicolonOrEmpty();

    // Scan all token inside the whole list, and decide whether it is full of comment(s) or blank(s)
    bool isAllTokensTrivial();

protected:
    TokenMgr();
    virtual ~TokenMgr();
    static bool isCommentType(TokenBase* pToken);
    static bool isBlankType(TokenBase* pToken);
    static bool isIgnoredType(TokenBase* pToken);

    std::pair<bool,TokenBase*>        checkAdjacentTokensRelationship_IsValid(TokenBase* toBePushed);
    std::pair<TokenBase*,TokenBase*>  getPreviousToken();

    bool process_SemicolonWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken);
    bool process_SequenceWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken);
    bool process_OperatorWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken, TokenBase* priorClosestToken);

    // for tmp expression check use
    bool is1stTokenTypeKeyWord();
    void executeCode_supportFunction();

    void tracePushedTokenWarning(TokenBase* pToken);//
    void traceBlankStatement();
protected:
    static TokenMgr* s_gInstance;

    static const OpPairCfg s_OpPairCfgTable[OPERATOR_CNT][OPERATOR_CNT];

    static const int s_MAX_KEYWORDS_CNT;
    static const std::unordered_map<std::string, E_DataType> s_keywordsDataTypeMap;
protected:
    std::vector<TokenBase*> m_allTokenList;    // for memory pool manager
                                               //
    // a list for all tokens that is neither comment nor blank ( arbitary <Space> or <Tab> )  ,  So the list can analyze the 2 adjacent statements' relationship.
    std::vector<TokenBase*> m_validTokenList;

    // no ';'
    // a list for only 1 statement with neither comment nor blank and whose delimiter is ';'
    std::vector<TokenBase*> m_oneSentence;

    // to use
    std::list< std::pair<TokenBase*,int> >  m_openParenthesisList;

    int m_execodeIdx;

};

#endif
