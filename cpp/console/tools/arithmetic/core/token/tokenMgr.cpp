#include "tokenMgr.h"
#include "myException.h"
#include "enumUtil.h"
#include "opUtil.h"
#include "cmdOptions.h"
#include "charUtil.h"
#include "expEvaluation.h"
#include <iterator>
#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////
//
//   Static Member Data && Functions
//
//////////////////////////////////////////////////////////////////////

TokenMgr* TokenMgr::s_gInstance = nullptr;

const TokenMgr::OpPairCfg TokenMgr::s_OpPairCfgTable[OPERATOR_CNT][OPERATOR_CNT] = {
    {   //  E_ADD   closeAdjacent  ,  seperate with <Space> / tab 
         TokenMgr::OpPairCfg( false, true ),      // +  + 
         TokenMgr::OpPairCfg( true, true ),       // +  - 
         TokenMgr::OpPairCfg( false, false ),     // +  * 
         TokenMgr::OpPairCfg( false, false ),     // +  / 
         TokenMgr::OpPairCfg( false, false ),     // +  % 
         TokenMgr::OpPairCfg( false, false ),     // +  & 
         TokenMgr::OpPairCfg( false, false ),     // +  | 
         TokenMgr::OpPairCfg( false, false ),     // +  ^ 
         TokenMgr::OpPairCfg( true, true ),       // +  ~ 
         TokenMgr::OpPairCfg( false, false ),     // +  << 
         TokenMgr::OpPairCfg( false, false ),     // +  >> 
         TokenMgr::OpPairCfg( true, true ),       // +  ( 
         TokenMgr::OpPairCfg( false, false ),     // +  ) 
         TokenMgr::OpPairCfg( false, false ),     // +  = 
         TokenMgr::OpPairCfg( false, false ),     // +  += 
         TokenMgr::OpPairCfg( false, false ),     // +  -= 
         TokenMgr::OpPairCfg( false, false ),     // +  *= 
         TokenMgr::OpPairCfg( false, false ),     // +  /= 
         TokenMgr::OpPairCfg( false, false ),     // +  %= 
         TokenMgr::OpPairCfg( false, false ),     // +  &= 
         TokenMgr::OpPairCfg( false, false ),     // +  |= 
         TokenMgr::OpPairCfg( false, false ),     // +  ^= 
         TokenMgr::OpPairCfg( false, false ),     // +  <<= 
         TokenMgr::OpPairCfg( false, false ),     // +  >>= 
         TokenMgr::OpPairCfg( false, false )      // +  ,
    },
    {   //  E_MINUS
        TokenMgr::OpPairCfg( true, true ),       // -  + 
        TokenMgr::OpPairCfg( false, true ),      // -  - 
        TokenMgr::OpPairCfg( false, false ),     // -  * 
        TokenMgr::OpPairCfg( false, false ),     // -  / 
        TokenMgr::OpPairCfg( false, false ),     // -  % 
        TokenMgr::OpPairCfg( false, false ),     // -  & 
        TokenMgr::OpPairCfg( false, false ),     // -  | 
        TokenMgr::OpPairCfg( false, false ),     // -  ^ 
        TokenMgr::OpPairCfg( true, true ),       // -  ~ 
        TokenMgr::OpPairCfg( false, false ),     // -  << 
        TokenMgr::OpPairCfg( false, false ),     // -  >> 
        TokenMgr::OpPairCfg( true, true ),       // -  ( 
        TokenMgr::OpPairCfg( false, false ),     // -  ) 
        TokenMgr::OpPairCfg( false, false ),     // -  = 
        TokenMgr::OpPairCfg( false, false ),     // -  += 
        TokenMgr::OpPairCfg( false, false ),     // -  -= 
        TokenMgr::OpPairCfg( false, false ),     // -  *= 
        TokenMgr::OpPairCfg( false, false ),     // -  /= 
        TokenMgr::OpPairCfg( false, false ),     // -  %= 
        TokenMgr::OpPairCfg( false, false ),     // -  &= 
        TokenMgr::OpPairCfg( false, false ),     // -  |= 
        TokenMgr::OpPairCfg( false, false ),     // -  ^= 
        TokenMgr::OpPairCfg( false, false ),     // -  <<= 
        TokenMgr::OpPairCfg( false, false ),     // -  >>= 
        TokenMgr::OpPairCfg( false, false )      // -  ,
    },
    {   //  E_MULTIPLY
        TokenMgr::OpPairCfg( true, true ),       // *  + 
        TokenMgr::OpPairCfg( true, true ),       // *  - 
        TokenMgr::OpPairCfg( false, false ),     // *  * 
        TokenMgr::OpPairCfg( false, false ),     // *  / 
        TokenMgr::OpPairCfg( false, false ),     // *  % 
        TokenMgr::OpPairCfg( false, false ),     // *  & 
        TokenMgr::OpPairCfg( false, false ),     // *  | 
        TokenMgr::OpPairCfg( false, false ),     // *  ^ 
        TokenMgr::OpPairCfg( true, true ),       // *  ~ 
        TokenMgr::OpPairCfg( false, false ),     // *  << 
        TokenMgr::OpPairCfg( false, false ),     // *  >> 
        TokenMgr::OpPairCfg( true, true ),       // *  ( 
        TokenMgr::OpPairCfg( false, false ),     // *  ) 
        TokenMgr::OpPairCfg( false, false ),     // *  = 
        TokenMgr::OpPairCfg( false, false ),     // *  += 
        TokenMgr::OpPairCfg( false, false ),     // *  -= 
        TokenMgr::OpPairCfg( false, false ),     // *  *= 
        TokenMgr::OpPairCfg( false, false ),     // *  /= 
        TokenMgr::OpPairCfg( false, false ),     // *  %= 
        TokenMgr::OpPairCfg( false, false ),     // *  &= 
        TokenMgr::OpPairCfg( false, false ),     // *  |= 
        TokenMgr::OpPairCfg( false, false ),     // *  ^= 
        TokenMgr::OpPairCfg( false, false ),     // *  <<= 
        TokenMgr::OpPairCfg( false, false ),     // *  >>= 
        TokenMgr::OpPairCfg( false, false )      // *  ,
    },
    {   //  E_DIVIDE
        TokenMgr::OpPairCfg( true, true ),       // /  + 
        TokenMgr::OpPairCfg( true, true ),       // /  - 
        TokenMgr::OpPairCfg( false, false ),     // /  * 
        TokenMgr::OpPairCfg( false, false ),     // /  / 
        TokenMgr::OpPairCfg( false, false ),     // /  % 
        TokenMgr::OpPairCfg( false, false ),     // /  & 
        TokenMgr::OpPairCfg( false, false ),     // /  | 
        TokenMgr::OpPairCfg( false, false ),     // /  ^ 
        TokenMgr::OpPairCfg( true, true ),       // /  ~ 
        TokenMgr::OpPairCfg( false, false ),     // /  << 
        TokenMgr::OpPairCfg( false, false ),     // /  >> 
        TokenMgr::OpPairCfg( true, true ),       // /  ( 
        TokenMgr::OpPairCfg( false, false ),     // /  ) 
        TokenMgr::OpPairCfg( false, false ),     // /  = 
        TokenMgr::OpPairCfg( false, false ),     // /  += 
        TokenMgr::OpPairCfg( false, false ),     // /  -= 
        TokenMgr::OpPairCfg( false, false ),     // /  *= 
        TokenMgr::OpPairCfg( false, false ),     // /  /= 
        TokenMgr::OpPairCfg( false, false ),     // /  %= 
        TokenMgr::OpPairCfg( false, false ),     // /  &= 
        TokenMgr::OpPairCfg( false, false ),     // /  |= 
        TokenMgr::OpPairCfg( false, false ),     // /  ^= 
        TokenMgr::OpPairCfg( false, false ),     // /  <<= 
        TokenMgr::OpPairCfg( false, false ),     // /  >>= 
        TokenMgr::OpPairCfg( false, false )      // /  ,
    },
    {   //  E_MOD
        TokenMgr::OpPairCfg( true, true ),       // %  + 
        TokenMgr::OpPairCfg( true, true ),       // %  - 
        TokenMgr::OpPairCfg( false, false ),     // %  * 
        TokenMgr::OpPairCfg( false, false ),     // %  / 
        TokenMgr::OpPairCfg( false, false ),     // %  % 
        TokenMgr::OpPairCfg( false, false ),     // %  & 
        TokenMgr::OpPairCfg( false, false ),     // %  | 
        TokenMgr::OpPairCfg( false, false ),     // %  ^ 
        TokenMgr::OpPairCfg( true, true ),       // %  ~ 
        TokenMgr::OpPairCfg( false, false ),     // %  << 
        TokenMgr::OpPairCfg( false, false ),     // %  >> 
        TokenMgr::OpPairCfg( true, true ),       // %  ( 
        TokenMgr::OpPairCfg( false, false ),     // %  ) 
        TokenMgr::OpPairCfg( false, false ),     // %  = 
        TokenMgr::OpPairCfg( false, false ),     // %  += 
        TokenMgr::OpPairCfg( false, false ),     // %  -= 
        TokenMgr::OpPairCfg( false, false ),     // %  *= 
        TokenMgr::OpPairCfg( false, false ),     // %  /= 
        TokenMgr::OpPairCfg( false, false ),     // %  %= 
        TokenMgr::OpPairCfg( false, false ),     // %  &= 
        TokenMgr::OpPairCfg( false, false ),     // %  |= 
        TokenMgr::OpPairCfg( false, false ),     // %  ^= 
        TokenMgr::OpPairCfg( false, false ),     // %  <<= 
        TokenMgr::OpPairCfg( false, false ),     // %  >>= 
        TokenMgr::OpPairCfg( false, false )      // %  ,
    },
    {   //  E_BIT_AND
        TokenMgr::OpPairCfg( true, true ),       // &  + 
        TokenMgr::OpPairCfg( true, true ),       // &  - 
        TokenMgr::OpPairCfg( false, false ),     // &  * 
        TokenMgr::OpPairCfg( false, false ),     // &  / 
        TokenMgr::OpPairCfg( false, false ),     // &  % 
        TokenMgr::OpPairCfg( false, false ),     // &  & 
        TokenMgr::OpPairCfg( false, false ),     // &  | 
        TokenMgr::OpPairCfg( false, false ),     // &  ^ 
        TokenMgr::OpPairCfg( true, true ),       // &  ~ 
        TokenMgr::OpPairCfg( false, false ),     // &  << 
        TokenMgr::OpPairCfg( false, false ),     // &  >> 
        TokenMgr::OpPairCfg( true, true ),       // &  ( 
        TokenMgr::OpPairCfg( false, false ),     // &  ) 
        TokenMgr::OpPairCfg( false, false ),     // &  = 
        TokenMgr::OpPairCfg( false, false ),     // &  += 
        TokenMgr::OpPairCfg( false, false ),     // &  -= 
        TokenMgr::OpPairCfg( false, false ),     // &  *= 
        TokenMgr::OpPairCfg( false, false ),     // &  /= 
        TokenMgr::OpPairCfg( false, false ),     // &  %= 
        TokenMgr::OpPairCfg( false, false ),     // &  &= 
        TokenMgr::OpPairCfg( false, false ),     // &  |= 
        TokenMgr::OpPairCfg( false, false ),     // &  ^= 
        TokenMgr::OpPairCfg( false, false ),     // &  <<= 
        TokenMgr::OpPairCfg( false, false ),     // &  >>= 
        TokenMgr::OpPairCfg( false, false )      // &  ,
    },
    {   //  E_BIT_OR
        TokenMgr::OpPairCfg( true, true ),       // |  + 
        TokenMgr::OpPairCfg( true, true ),       // |  - 
        TokenMgr::OpPairCfg( false, false ),     // |  * 
        TokenMgr::OpPairCfg( false, false ),     // |  / 
        TokenMgr::OpPairCfg( false, false ),     // |  % 
        TokenMgr::OpPairCfg( false, false ),     // |  & 
        TokenMgr::OpPairCfg( false, false ),     // |  | 
        TokenMgr::OpPairCfg( false, false ),     // |  ^ 
        TokenMgr::OpPairCfg( true, true ),       // |  ~ 
        TokenMgr::OpPairCfg( false, false ),     // |  << 
        TokenMgr::OpPairCfg( false, false ),     // |  >> 
        TokenMgr::OpPairCfg( true, true ),       // |  ( 
        TokenMgr::OpPairCfg( false, false ),     // |  ) 
        TokenMgr::OpPairCfg( false, false ),     // |  = 
        TokenMgr::OpPairCfg( false, false ),     // |  += 
        TokenMgr::OpPairCfg( false, false ),     // |  -= 
        TokenMgr::OpPairCfg( false, false ),     // |  *= 
        TokenMgr::OpPairCfg( false, false ),     // |  /= 
        TokenMgr::OpPairCfg( false, false ),     // |  %= 
        TokenMgr::OpPairCfg( false, false ),     // |  &= 
        TokenMgr::OpPairCfg( false, false ),     // |  |= 
        TokenMgr::OpPairCfg( false, false ),     // |  ^= 
        TokenMgr::OpPairCfg( false, false ),     // |  <<= 
        TokenMgr::OpPairCfg( false, false ),     // |  >>= 
        TokenMgr::OpPairCfg( false, false )      // |  ,
    },
    {   //  E_BIT_XOR
        TokenMgr::OpPairCfg( true, true ),       // ^  + 
        TokenMgr::OpPairCfg( true, true ),       // ^  - 
        TokenMgr::OpPairCfg( false, false ),     // ^  * 
        TokenMgr::OpPairCfg( false, false ),     // ^  / 
        TokenMgr::OpPairCfg( false, false ),     // ^  % 
        TokenMgr::OpPairCfg( false, false ),     // ^  & 
        TokenMgr::OpPairCfg( false, false ),     // ^  | 
        TokenMgr::OpPairCfg( false, false ),     // ^  ^ 
        TokenMgr::OpPairCfg( true, true ),       // ^  ~ 
        TokenMgr::OpPairCfg( false, false ),     // ^  << 
        TokenMgr::OpPairCfg( false, false ),     // ^  >> 
        TokenMgr::OpPairCfg( true, true ),       // ^  ( 
        TokenMgr::OpPairCfg( false, false ),     // ^  ) 
        TokenMgr::OpPairCfg( false, false ),     // ^  = 
        TokenMgr::OpPairCfg( false, false ),     // ^  += 
        TokenMgr::OpPairCfg( false, false ),     // ^  -= 
        TokenMgr::OpPairCfg( false, false ),     // ^  *= 
        TokenMgr::OpPairCfg( false, false ),     // ^  /= 
        TokenMgr::OpPairCfg( false, false ),     // ^  %= 
        TokenMgr::OpPairCfg( false, false ),     // ^  &= 
        TokenMgr::OpPairCfg( false, false ),     // ^  |= 
        TokenMgr::OpPairCfg( false, false ),     // ^  ^= 
        TokenMgr::OpPairCfg( false, false ),     // ^  <<= 
        TokenMgr::OpPairCfg( false, false ),     // ^  >>= 
        TokenMgr::OpPairCfg( false, false )      // ^  ,
    },
    {   //  E_BIT_NOT
        TokenMgr::OpPairCfg( true, true ),       // ~  + 
        TokenMgr::OpPairCfg( true, true ),       // ~  - 
        TokenMgr::OpPairCfg( false, false ),     // ~  * 
        TokenMgr::OpPairCfg( false, false ),     // ~  / 
        TokenMgr::OpPairCfg( false, false ),     // ~  % 
        TokenMgr::OpPairCfg( false, false ),     // ~  & 
        TokenMgr::OpPairCfg( false, false ),     // ~  | 
        TokenMgr::OpPairCfg( false, false ),     // ~  ^ 
        TokenMgr::OpPairCfg( true, true ),       // ~  ~ 
        TokenMgr::OpPairCfg( false, false ),     // ~  << 
        TokenMgr::OpPairCfg( false, false ),     // ~  >> 
        TokenMgr::OpPairCfg( true, true ),       // ~  ( 
        TokenMgr::OpPairCfg( false, false ),     // ~  ) 
        TokenMgr::OpPairCfg( false, false ),     // ~  = 
        TokenMgr::OpPairCfg( false, false ),     // ~  += 
        TokenMgr::OpPairCfg( false, false ),     // ~  -= 
        TokenMgr::OpPairCfg( false, false ),     // ~  *= 
        TokenMgr::OpPairCfg( false, false ),     // ~  /= 
        TokenMgr::OpPairCfg( false, false ),     // ~  %= 
        TokenMgr::OpPairCfg( false, false ),     // ~  &= 
        TokenMgr::OpPairCfg( false, false ),     // ~  |= 
        TokenMgr::OpPairCfg( false, false ),     // ~  ^= 
        TokenMgr::OpPairCfg( false, false ),     // ~  <<= 
        TokenMgr::OpPairCfg( false, false ),     // ~  >>= 
        TokenMgr::OpPairCfg( false, false )      // ~  ,
    },
    {   //  E_BIT_LEFT_SHIFT
        TokenMgr::OpPairCfg( true, true ),       // <<  + 
        TokenMgr::OpPairCfg( true, true ),       // <<  - 
        TokenMgr::OpPairCfg( false, false ),     // <<  * 
        TokenMgr::OpPairCfg( false, false ),     // <<  / 
        TokenMgr::OpPairCfg( false, false ),     // <<  % 
        TokenMgr::OpPairCfg( false, false ),     // <<  & 
        TokenMgr::OpPairCfg( false, false ),     // <<  | 
        TokenMgr::OpPairCfg( false, false ),     // <<  ^ 
        TokenMgr::OpPairCfg( true, true ),       // <<  ~ 
        TokenMgr::OpPairCfg( false, false ),     // <<  << 
        TokenMgr::OpPairCfg( false, false ),     // <<  >> 
        TokenMgr::OpPairCfg( true, true ),       // <<  ( 
        TokenMgr::OpPairCfg( false, false ),     // <<  ) 
        TokenMgr::OpPairCfg( false, false ),     // <<  = 
        TokenMgr::OpPairCfg( false, false ),     // <<  += 
        TokenMgr::OpPairCfg( false, false ),     // <<  -= 
        TokenMgr::OpPairCfg( false, false ),     // <<  *= 
        TokenMgr::OpPairCfg( false, false ),     // <<  /= 
        TokenMgr::OpPairCfg( false, false ),     // <<  %= 
        TokenMgr::OpPairCfg( false, false ),     // <<  &= 
        TokenMgr::OpPairCfg( false, false ),     // <<  |= 
        TokenMgr::OpPairCfg( false, false ),     // <<  ^= 
        TokenMgr::OpPairCfg( false, false ),     // <<  <<= 
        TokenMgr::OpPairCfg( false, false ),     // <<  >>= 
        TokenMgr::OpPairCfg( false, false )      // <<  ,
    },
    {   //  E_BIT_RIGHT_SHIFT
        TokenMgr::OpPairCfg( true, true ),       // >>  + 
        TokenMgr::OpPairCfg( true, true ),       // >>  - 
        TokenMgr::OpPairCfg( false, false ),     // >>  * 
        TokenMgr::OpPairCfg( false, false ),     // >>  / 
        TokenMgr::OpPairCfg( false, false ),     // >>  % 
        TokenMgr::OpPairCfg( false, false ),     // >>  & 
        TokenMgr::OpPairCfg( false, false ),     // >>  | 
        TokenMgr::OpPairCfg( false, false ),     // >>  ^ 
        TokenMgr::OpPairCfg( true, true ),       // >>  ~ 
        TokenMgr::OpPairCfg( false, false ),     // >>  << 
        TokenMgr::OpPairCfg( false, false ),     // >>  >> 
        TokenMgr::OpPairCfg( true, true ),       // >>  ( 
        TokenMgr::OpPairCfg( false, false ),     // >>  ) 
        TokenMgr::OpPairCfg( false, false ),     // >>  = 
        TokenMgr::OpPairCfg( false, false ),     // >>  += 
        TokenMgr::OpPairCfg( false, false ),     // >>  -= 
        TokenMgr::OpPairCfg( false, false ),     // >>  *= 
        TokenMgr::OpPairCfg( false, false ),     // >>  /= 
        TokenMgr::OpPairCfg( false, false ),     // >>  %= 
        TokenMgr::OpPairCfg( false, false ),     // >>  &= 
        TokenMgr::OpPairCfg( false, false ),     // >>  |= 
        TokenMgr::OpPairCfg( false, false ),     // >>  ^= 
        TokenMgr::OpPairCfg( false, false ),     // >>  <<= 
        TokenMgr::OpPairCfg( false, false ),    // >>  >>= 
        TokenMgr::OpPairCfg( false, false )     // >>  ,
    },
    {   //  E_OPEN_PARENTHESIS
        TokenMgr::OpPairCfg( true, true ),       // (  + 
        TokenMgr::OpPairCfg( true, true ),       // (  - 
        TokenMgr::OpPairCfg( false, false ),     // (  * 
        TokenMgr::OpPairCfg( false, false ),     // (  / 
        TokenMgr::OpPairCfg( false, false ),     // (  % 
        TokenMgr::OpPairCfg( false, false ),     // (  & 
        TokenMgr::OpPairCfg( false, false ),     // (  | 
        TokenMgr::OpPairCfg( false, false ),     // (  ^ 
        TokenMgr::OpPairCfg( true, true ),       // (  ~ 
        TokenMgr::OpPairCfg( false, false ),     // (  << 
        TokenMgr::OpPairCfg( false, false ),     // (  >> 
        TokenMgr::OpPairCfg( true, true ),       // (  ( 
        TokenMgr::OpPairCfg( true, true ),       // (  )   // if function feature is implemented , set false,false  ==>  true, true
        TokenMgr::OpPairCfg( false, false ),     // (  = 
        TokenMgr::OpPairCfg( false, false ),     // (  += 
        TokenMgr::OpPairCfg( false, false ),     // (  -= 
        TokenMgr::OpPairCfg( false, false ),     // (  *= 
        TokenMgr::OpPairCfg( false, false ),     // (  /= 
        TokenMgr::OpPairCfg( false, false ),     // (  %= 
        TokenMgr::OpPairCfg( false, false ),     // (  &= 
        TokenMgr::OpPairCfg( false, false ),     // (  |= 
        TokenMgr::OpPairCfg( false, false ),     // (  ^= 
        TokenMgr::OpPairCfg( false, false ),     // (  <<= 
        TokenMgr::OpPairCfg( false, false ),     // (  >>= 
        TokenMgr::OpPairCfg( false, false )      // (  ,
    },
    {   //  E_CLOSE_PARENTHESIS
        TokenMgr::OpPairCfg( true, true ),     // )  + 
        TokenMgr::OpPairCfg( true, true ),     // )  - 
        TokenMgr::OpPairCfg( true, true ),     // )  * 
        TokenMgr::OpPairCfg( true, true ),     // )  / 
        TokenMgr::OpPairCfg( true, true ),     // )  % 
        TokenMgr::OpPairCfg( true, true ),     // )  & 
        TokenMgr::OpPairCfg( true, true ),     // )  | 
        TokenMgr::OpPairCfg( true, true ),     // )  ^ 
        TokenMgr::OpPairCfg( true, true ),     // )  ~     only if (int)~5  :  the ) is the end of force-type-cast ( new feature )
        TokenMgr::OpPairCfg( true, true ),     // )  << 
        TokenMgr::OpPairCfg( true, true ),     // )  >> 
        TokenMgr::OpPairCfg( true, true ),     // )  (     =>   (   only if (int)( 3.14f * 2)    the 1st ) is the end of force-type-cast ( new feature )
        TokenMgr::OpPairCfg( true, true ),     // )  ) 
        TokenMgr::OpPairCfg( true, true ),     // )  =   // (a) = 3;  or  (a)=3;      is valid
        TokenMgr::OpPairCfg( true, true ),     // )  += 
        TokenMgr::OpPairCfg( true, true ),     // )  -= 
        TokenMgr::OpPairCfg( true, true ),     // )  *= 
        TokenMgr::OpPairCfg( true, true ),     // )  /= 
        TokenMgr::OpPairCfg( true, true ),     // )  %= 
        TokenMgr::OpPairCfg( true, true ),     // )  &= 
        TokenMgr::OpPairCfg( true, true ),     // )  |= 
        TokenMgr::OpPairCfg( true, true ),     // )  ^= 
        TokenMgr::OpPairCfg( true, true ),     // )  <<= 
        TokenMgr::OpPairCfg( true, true ),     // )  >>= 
        TokenMgr::OpPairCfg( true, true )      // )  ,     such as : int a = (3),4;
    },
    {   //  E_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // =  + 
        TokenMgr::OpPairCfg( true, true ),     // =  - 
        TokenMgr::OpPairCfg( false, false ),   // =  * 
        TokenMgr::OpPairCfg( false, false ),   // =  / 
        TokenMgr::OpPairCfg( false, false ),   // =  % 
        TokenMgr::OpPairCfg( false, false ),   // =  & 
        TokenMgr::OpPairCfg( false, false ),   // =  | 
        TokenMgr::OpPairCfg( false, false ),   // =  ^ 
        TokenMgr::OpPairCfg( true, true ),     // =  ~ 
        TokenMgr::OpPairCfg( false, false ),   // =  << 
        TokenMgr::OpPairCfg( false, false ),   // =  >> 
        TokenMgr::OpPairCfg( true, true ),     // =  ( 
        TokenMgr::OpPairCfg( false, false ),   // =  ) 
        TokenMgr::OpPairCfg( false, false ),   // =  = 
        TokenMgr::OpPairCfg( false, false ),   // =  += 
        TokenMgr::OpPairCfg( false, false ),   // =  -= 
        TokenMgr::OpPairCfg( false, false ),   // =  *= 
        TokenMgr::OpPairCfg( false, false ),   // =  /= 
        TokenMgr::OpPairCfg( false, false ),   // =  %= 
        TokenMgr::OpPairCfg( false, false ),   // =  &= 
        TokenMgr::OpPairCfg( false, false ),   // =  |= 
        TokenMgr::OpPairCfg( false, false ),   // =  ^= 
        TokenMgr::OpPairCfg( false, false ),   // =  <<= 
        TokenMgr::OpPairCfg( false, false ),   // =  >>= 
        TokenMgr::OpPairCfg( false, false )    // = ,
    },
    {   //  E_ADD_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // +=  + 
        TokenMgr::OpPairCfg( true, true ),     // +=  - 
        TokenMgr::OpPairCfg( false, false ),   // +=  * 
        TokenMgr::OpPairCfg( false, false ),   // +=  / 
        TokenMgr::OpPairCfg( false, false ),   // +=  % 
        TokenMgr::OpPairCfg( false, false ),   // +=  & 
        TokenMgr::OpPairCfg( false, false ),   // +=  | 
        TokenMgr::OpPairCfg( false, false ),   // +=  ^ 
        TokenMgr::OpPairCfg( true, true ),     // +=  ~ 
        TokenMgr::OpPairCfg( false, false ),   // +=  << 
        TokenMgr::OpPairCfg( false, false ),   // +=  >> 
        TokenMgr::OpPairCfg( true, true ),     // +=  ( 
        TokenMgr::OpPairCfg( false, false ),   // +=  ) 
        TokenMgr::OpPairCfg( false, false ),   // +=  = 
        TokenMgr::OpPairCfg( false, false ),   // +=  += 
        TokenMgr::OpPairCfg( false, false ),   // +=  -= 
        TokenMgr::OpPairCfg( false, false ),   // +=  *= 
        TokenMgr::OpPairCfg( false, false ),   // +=  /= 
        TokenMgr::OpPairCfg( false, false ),   // +=  %= 
        TokenMgr::OpPairCfg( false, false ),   // +=  &= 
        TokenMgr::OpPairCfg( false, false ),   // +=  |= 
        TokenMgr::OpPairCfg( false, false ),   // +=  ^= 
        TokenMgr::OpPairCfg( false, false ),   // +=  <<= 
        TokenMgr::OpPairCfg( false, false ),   // +=  >>= 
        TokenMgr::OpPairCfg( false, false )    // += ,
    },
    {   //  E_MINUS_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // -=  + 
        TokenMgr::OpPairCfg( true, true ),     // -=  - 
        TokenMgr::OpPairCfg( false, false ),   // -=  * 
        TokenMgr::OpPairCfg( false, false ),   // -=  / 
        TokenMgr::OpPairCfg( false, false ),   // -=  % 
        TokenMgr::OpPairCfg( false, false ),   // -=  & 
        TokenMgr::OpPairCfg( false, false ),   // -=  | 
        TokenMgr::OpPairCfg( false, false ),   // -=  ^ 
        TokenMgr::OpPairCfg( true, true ),     // -=  ~ 
        TokenMgr::OpPairCfg( false, false ),   // -=  << 
        TokenMgr::OpPairCfg( false, false ),   // -=  >> 
        TokenMgr::OpPairCfg( true, true ),     // -=  ( 
        TokenMgr::OpPairCfg( false, false ),   // -=  ) 
        TokenMgr::OpPairCfg( false, false ),   // -=  = 
        TokenMgr::OpPairCfg( false, false ),   // -=  += 
        TokenMgr::OpPairCfg( false, false ),   // -=  -= 
        TokenMgr::OpPairCfg( false, false ),   // -=  *= 
        TokenMgr::OpPairCfg( false, false ),   // -=  /= 
        TokenMgr::OpPairCfg( false, false ),   // -=  %= 
        TokenMgr::OpPairCfg( false, false ),   // -=  &= 
        TokenMgr::OpPairCfg( false, false ),   // -=  |= 
        TokenMgr::OpPairCfg( false, false ),   // -=  ^= 
        TokenMgr::OpPairCfg( false, false ),   // -=  <<= 
        TokenMgr::OpPairCfg( false, false ),   // -=  >>= 
        TokenMgr::OpPairCfg( false, false )    // -= ,
    },
    {   //  E_MULTIPLY_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // *=  + 
        TokenMgr::OpPairCfg( true, true ),     // *=  - 
        TokenMgr::OpPairCfg( false, false ),   // *=  * 
        TokenMgr::OpPairCfg( false, false ),   // *=  / 
        TokenMgr::OpPairCfg( false, false ),   // *=  % 
        TokenMgr::OpPairCfg( false, false ),   // *=  & 
        TokenMgr::OpPairCfg( false, false ),   // *=  | 
        TokenMgr::OpPairCfg( false, false ),   // *=  ^ 
        TokenMgr::OpPairCfg( true, true ),     // *=  ~ 
        TokenMgr::OpPairCfg( false, false ),   // *=  << 
        TokenMgr::OpPairCfg( false, false ),   // *=  >> 
        TokenMgr::OpPairCfg( true, true ),     // *=  ( 
        TokenMgr::OpPairCfg( false, false ),   // *=  ) 
        TokenMgr::OpPairCfg( false, false ),   // *=  = 
        TokenMgr::OpPairCfg( false, false ),   // *=  += 
        TokenMgr::OpPairCfg( false, false ),   // *=  -= 
        TokenMgr::OpPairCfg( false, false ),   // *=  *= 
        TokenMgr::OpPairCfg( false, false ),   // *=  /= 
        TokenMgr::OpPairCfg( false, false ),   // *=  %= 
        TokenMgr::OpPairCfg( false, false ),   // *=  &= 
        TokenMgr::OpPairCfg( false, false ),   // *=  |= 
        TokenMgr::OpPairCfg( false, false ),   // *=  ^= 
        TokenMgr::OpPairCfg( false, false ),   // *=  <<= 
        TokenMgr::OpPairCfg( false, false ),   // *=  >>= 
        TokenMgr::OpPairCfg( false, false )    // *= ,
    },
    {   //  E_DIVIDE_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // /=  + 
        TokenMgr::OpPairCfg( true, true ),     // /=  - 
        TokenMgr::OpPairCfg( false, false ),   // /=  * 
        TokenMgr::OpPairCfg( false, false ),   // /=  / 
        TokenMgr::OpPairCfg( false, false ),   // /=  % 
        TokenMgr::OpPairCfg( false, false ),   // /=  & 
        TokenMgr::OpPairCfg( false, false ),   // /=  | 
        TokenMgr::OpPairCfg( false, false ),   // /=  ^ 
        TokenMgr::OpPairCfg( true, true ),     // /=  ~ 
        TokenMgr::OpPairCfg( false, false ),   // /=  << 
        TokenMgr::OpPairCfg( false, false ),   // /=  >> 
        TokenMgr::OpPairCfg( true, true ),     // /=  ( 
        TokenMgr::OpPairCfg( false, false ),   // /=  ) 
        TokenMgr::OpPairCfg( false, false ),   // /=  = 
        TokenMgr::OpPairCfg( false, false ),   // /=  += 
        TokenMgr::OpPairCfg( false, false ),   // /=  -= 
        TokenMgr::OpPairCfg( false, false ),   // /=  *= 
        TokenMgr::OpPairCfg( false, false ),   // /=  /= 
        TokenMgr::OpPairCfg( false, false ),   // /=  %= 
        TokenMgr::OpPairCfg( false, false ),   // /=  &= 
        TokenMgr::OpPairCfg( false, false ),   // /=  |= 
        TokenMgr::OpPairCfg( false, false ),   // /=  ^= 
        TokenMgr::OpPairCfg( false, false ),   // /=  <<= 
        TokenMgr::OpPairCfg( false, false ),   // /=  >>= 
        TokenMgr::OpPairCfg( false, false )    // /= ,
    },
    {   //  E_MOD_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // %=  + 
        TokenMgr::OpPairCfg( true, true ),     // %=  - 
        TokenMgr::OpPairCfg( false, false ),   // %=  * 
        TokenMgr::OpPairCfg( false, false ),   // %=  / 
        TokenMgr::OpPairCfg( false, false ),   // %=  % 
        TokenMgr::OpPairCfg( false, false ),   // %=  & 
        TokenMgr::OpPairCfg( false, false ),   // %=  | 
        TokenMgr::OpPairCfg( false, false ),   // %=  ^ 
        TokenMgr::OpPairCfg( true, true ),     // %=  ~ 
        TokenMgr::OpPairCfg( false, false ),   // %=  << 
        TokenMgr::OpPairCfg( false, false ),   // %=  >> 
        TokenMgr::OpPairCfg( true, true ),     // %=  ( 
        TokenMgr::OpPairCfg( false, false ),   // %=  ) 
        TokenMgr::OpPairCfg( false, false ),   // %=  = 
        TokenMgr::OpPairCfg( false, false ),   // %=  += 
        TokenMgr::OpPairCfg( false, false ),   // %=  -= 
        TokenMgr::OpPairCfg( false, false ),   // %=  *= 
        TokenMgr::OpPairCfg( false, false ),   // %=  /= 
        TokenMgr::OpPairCfg( false, false ),   // %=  %= 
        TokenMgr::OpPairCfg( false, false ),   // %=  &= 
        TokenMgr::OpPairCfg( false, false ),   // %=  |= 
        TokenMgr::OpPairCfg( false, false ),   // %=  ^= 
        TokenMgr::OpPairCfg( false, false ),   // %=  <<= 
        TokenMgr::OpPairCfg( false, false ),   // %=  >>= 
        TokenMgr::OpPairCfg( false, false )    // %= ,
    },
    {   //  E_BIT_AND_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // &=  + 
        TokenMgr::OpPairCfg( true, true ),     // &=  - 
        TokenMgr::OpPairCfg( false, false ),   // &=  * 
        TokenMgr::OpPairCfg( false, false ),   // &=  / 
        TokenMgr::OpPairCfg( false, false ),   // &=  % 
        TokenMgr::OpPairCfg( false, false ),   // &=  & 
        TokenMgr::OpPairCfg( false, false ),   // &=  | 
        TokenMgr::OpPairCfg( false, false ),   // &=  ^ 
        TokenMgr::OpPairCfg( true, true ),     // &=  ~ 
        TokenMgr::OpPairCfg( false, false ),   // &=  << 
        TokenMgr::OpPairCfg( false, false ),   // &=  >> 
        TokenMgr::OpPairCfg( true, true ),     // &=  ( 
        TokenMgr::OpPairCfg( false, false ),   // &=  ) 
        TokenMgr::OpPairCfg( false, false ),   // &=  = 
        TokenMgr::OpPairCfg( false, false ),   // &=  += 
        TokenMgr::OpPairCfg( false, false ),   // &=  -= 
        TokenMgr::OpPairCfg( false, false ),   // &=  *= 
        TokenMgr::OpPairCfg( false, false ),   // &=  /= 
        TokenMgr::OpPairCfg( false, false ),   // &=  %= 
        TokenMgr::OpPairCfg( false, false ),   // &=  &= 
        TokenMgr::OpPairCfg( false, false ),   // &=  |= 
        TokenMgr::OpPairCfg( false, false ),   // &=  ^= 
        TokenMgr::OpPairCfg( false, false ),   // &=  <<= 
        TokenMgr::OpPairCfg( false, false ),   // &=  >>= 
        TokenMgr::OpPairCfg( false, false )    // &= ,
    },
    {   //  E_BIT_OR_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // |=  + 
        TokenMgr::OpPairCfg( true, true ),     // |=  - 
        TokenMgr::OpPairCfg( false, false ),   // |=  * 
        TokenMgr::OpPairCfg( false, false ),   // |=  / 
        TokenMgr::OpPairCfg( false, false ),   // |=  % 
        TokenMgr::OpPairCfg( false, false ),   // |=  & 
        TokenMgr::OpPairCfg( false, false ),   // |=  | 
        TokenMgr::OpPairCfg( false, false ),   // |=  ^ 
        TokenMgr::OpPairCfg( true, true ),     // |=  ~ 
        TokenMgr::OpPairCfg( false, false ),   // |=  << 
        TokenMgr::OpPairCfg( false, false ),   // |=  >> 
        TokenMgr::OpPairCfg( true, true ),     // |=  ( 
        TokenMgr::OpPairCfg( false, false ),   // |=  ) 
        TokenMgr::OpPairCfg( false, false ),   // |=  = 
        TokenMgr::OpPairCfg( false, false ),   // |=  += 
        TokenMgr::OpPairCfg( false, false ),   // |=  -= 
        TokenMgr::OpPairCfg( false, false ),   // |=  *= 
        TokenMgr::OpPairCfg( false, false ),   // |=  /= 
        TokenMgr::OpPairCfg( false, false ),   // |=  %= 
        TokenMgr::OpPairCfg( false, false ),   // |=  &= 
        TokenMgr::OpPairCfg( false, false ),   // |=  |= 
        TokenMgr::OpPairCfg( false, false ),   // |=  ^= 
        TokenMgr::OpPairCfg( false, false ),   // |=  <<= 
        TokenMgr::OpPairCfg( false, false ),   // |=  >>= 
        TokenMgr::OpPairCfg( false, false )    // |= ,
    },
    {   //  E_BIT_XOR_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // ^=  + 
        TokenMgr::OpPairCfg( true, true ),     // ^=  - 
        TokenMgr::OpPairCfg( false, false ),   // ^=  * 
        TokenMgr::OpPairCfg( false, false ),   // ^=  / 
        TokenMgr::OpPairCfg( false, false ),   // ^=  % 
        TokenMgr::OpPairCfg( false, false ),   // ^=  & 
        TokenMgr::OpPairCfg( false, false ),   // ^=  | 
        TokenMgr::OpPairCfg( false, false ),   // ^=  ^ 
        TokenMgr::OpPairCfg( true, true ),     // ^=  ~ 
        TokenMgr::OpPairCfg( false, false ),   // ^=  << 
        TokenMgr::OpPairCfg( false, false ),   // ^=  >> 
        TokenMgr::OpPairCfg( true, true ),     // ^=  ( 
        TokenMgr::OpPairCfg( false, false ),   // ^=  ) 
        TokenMgr::OpPairCfg( false, false ),   // ^=  = 
        TokenMgr::OpPairCfg( false, false ),   // ^=  += 
        TokenMgr::OpPairCfg( false, false ),   // ^=  -= 
        TokenMgr::OpPairCfg( false, false ),   // ^=  *= 
        TokenMgr::OpPairCfg( false, false ),   // ^=  /= 
        TokenMgr::OpPairCfg( false, false ),   // ^=  %= 
        TokenMgr::OpPairCfg( false, false ),   // ^=  &= 
        TokenMgr::OpPairCfg( false, false ),   // ^=  |= 
        TokenMgr::OpPairCfg( false, false ),   // ^=  ^= 
        TokenMgr::OpPairCfg( false, false ),   // ^=  <<= 
        TokenMgr::OpPairCfg( false, false ),   // ^=  >>= 
        TokenMgr::OpPairCfg( false, false )    // ^= ,
    },
    {   //  E_BIT_LEFT_SHIFT_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // <<=  + 
        TokenMgr::OpPairCfg( true, true ),     // <<=  - 
        TokenMgr::OpPairCfg( false, false ),   // <<=  * 
        TokenMgr::OpPairCfg( false, false ),   // <<=  / 
        TokenMgr::OpPairCfg( false, false ),   // <<=  % 
        TokenMgr::OpPairCfg( false, false ),   // <<=  & 
        TokenMgr::OpPairCfg( false, false ),   // <<=  | 
        TokenMgr::OpPairCfg( false, false ),   // <<=  ^ 
        TokenMgr::OpPairCfg( true, true ),     // <<=  ~ 
        TokenMgr::OpPairCfg( false, false ),   // <<=  << 
        TokenMgr::OpPairCfg( false, false ),   // <<=  >> 
        TokenMgr::OpPairCfg( true, true ),     // <<=  ( 
        TokenMgr::OpPairCfg( false, false ),   // <<=  ) 
        TokenMgr::OpPairCfg( false, false ),   // <<=  = 
        TokenMgr::OpPairCfg( false, false ),   // <<=  += 
        TokenMgr::OpPairCfg( false, false ),   // <<=  -= 
        TokenMgr::OpPairCfg( false, false ),   // <<=  *= 
        TokenMgr::OpPairCfg( false, false ),   // <<=  /= 
        TokenMgr::OpPairCfg( false, false ),   // <<=  %= 
        TokenMgr::OpPairCfg( false, false ),   // <<=  &= 
        TokenMgr::OpPairCfg( false, false ),   // <<=  |= 
        TokenMgr::OpPairCfg( false, false ),   // <<=  ^= 
        TokenMgr::OpPairCfg( false, false ),   // <<=  <<= 
        TokenMgr::OpPairCfg( false, false ),   // <<=  >>= 
        TokenMgr::OpPairCfg( false, false )    // <<= ,
    },
    {   //  E_BIT_RIGHT_SHIFT_ASSIGNMENT
        TokenMgr::OpPairCfg( true, true ),     // >>=  + 
        TokenMgr::OpPairCfg( true, true ),     // >>=  - 
        TokenMgr::OpPairCfg( false, false ),   // >>=  * 
        TokenMgr::OpPairCfg( false, false ),   // >>=  / 
        TokenMgr::OpPairCfg( false, false ),   // >>=  % 
        TokenMgr::OpPairCfg( false, false ),   // >>=  & 
        TokenMgr::OpPairCfg( false, false ),   // >>=  | 
        TokenMgr::OpPairCfg( false, false ),   // >>=  ^ 
        TokenMgr::OpPairCfg( true, true ),     // >>=  ~ 
        TokenMgr::OpPairCfg( false, false ),   // >>=  << 
        TokenMgr::OpPairCfg( false, false ),   // >>=  >> 
        TokenMgr::OpPairCfg( true, true ),     // >>=  ( 
        TokenMgr::OpPairCfg( false, false ),   // >>=  ) 
        TokenMgr::OpPairCfg( false, false ),   // >>=  = 
        TokenMgr::OpPairCfg( false, false ),   // >>=  += 
        TokenMgr::OpPairCfg( false, false ),   // >>=  -= 
        TokenMgr::OpPairCfg( false, false ),   // >>=  *= 
        TokenMgr::OpPairCfg( false, false ),   // >>=  /= 
        TokenMgr::OpPairCfg( false, false ),   // >>=  %= 
        TokenMgr::OpPairCfg( false, false ),   // >>=  &= 
        TokenMgr::OpPairCfg( false, false ),   // >>=  |= 
        TokenMgr::OpPairCfg( false, false ),   // >>=  ^= 
        TokenMgr::OpPairCfg( false, false ),   // >>=  <<= 
        TokenMgr::OpPairCfg( false, false ),   // >>=  >>= 
        TokenMgr::OpPairCfg( false, false )    // >>= ,
    },
    {   //  E_COMMA
        TokenMgr::OpPairCfg( true, true ),     // ,  + 
        TokenMgr::OpPairCfg( true, true ),     // ,  - 
        TokenMgr::OpPairCfg( false, false ),   // ,  * 
        TokenMgr::OpPairCfg( false, false ),   // ,  / 
        TokenMgr::OpPairCfg( false, false ),   // ,  % 
        TokenMgr::OpPairCfg( false, false ),   // ,  & 
        TokenMgr::OpPairCfg( false, false ),   // ,  | 
        TokenMgr::OpPairCfg( false, false ),   // ,  ^ 
        TokenMgr::OpPairCfg( true, true ),     // ,  ~ 
        TokenMgr::OpPairCfg( false, false ),   // ,  << 
        TokenMgr::OpPairCfg( false, false ),   // ,  >> 
        TokenMgr::OpPairCfg( true, true ),     // ,  (   such as  :   a = 3, ( 4+5);
        TokenMgr::OpPairCfg( false, false ),   // ,  ) 
        TokenMgr::OpPairCfg( false, false ),   // ,  = 
        TokenMgr::OpPairCfg( false, false ),   // ,  += 
        TokenMgr::OpPairCfg( false, false ),   // ,  -= 
        TokenMgr::OpPairCfg( false, false ),   // ,  *= 
        TokenMgr::OpPairCfg( false, false ),   // ,  /= 
        TokenMgr::OpPairCfg( false, false ),   // ,  %= 
        TokenMgr::OpPairCfg( false, false ),   // ,  &= 
        TokenMgr::OpPairCfg( false, false ),   // ,  |= 
        TokenMgr::OpPairCfg( false, false ),   // ,  ^= 
        TokenMgr::OpPairCfg( false, false ),   // ,  <<= 
        TokenMgr::OpPairCfg( false, false ),   // ,  >>= 
        TokenMgr::OpPairCfg( false, false )    // , ,
    }
};



// static ,  stand for   "unsigned long long int" : there are 4 keywords for the longest data type defination
const int TokenMgr::s_MAX_KEYWORDS_CNT = 4;

// static 
const unordered_map<string, E_DataType> TokenMgr::s_keywordsDataTypeMap{
    // 1 word
    { "char",                   E_TP_CHAR         },
    { "short",                  E_TP_S_SHORT      },
    { "int",                    E_TP_S_INT        },
    { "long",                   E_TP_S_LONG       },
    { "float",                  E_TP_FLOAT        },
    { "double",                 E_TP_DOUBLE       },
    { "signed",                 E_TP_S_INT        },
    { "unsigned",               E_TP_U_INT        },
    // 2 words
    { "unsigned char",          E_TP_U_CHAR       },
    { "signed char",            E_TP_S_CHAR       },
    { "unsigned short",         E_TP_U_SHORT      },
    { "signed short",           E_TP_S_SHORT      },
    { "unsigned int",           E_TP_U_INT        },
    { "signed int",             E_TP_S_INT        },
    { "unsigned long",          E_TP_U_LONG       },
    { "signed long",            E_TP_S_LONG       },
    { "long int",               E_TP_S_LONG       },
    { "long long",              E_TP_S_LONG_LONG  },
    // 3 words
    { "unsigned long int",      E_TP_U_LONG       },
    { "signed long int",        E_TP_S_LONG       },
    { "unsigned long long",     E_TP_U_LONG_LONG  },
    { "signed long long",       E_TP_S_LONG_LONG  },
    { "long long int",          E_TP_S_LONG_LONG  },
    // 4 words
    { "unsigned long long int", E_TP_U_LONG_LONG  },
    { "signed long long int",   E_TP_S_LONG_LONG  }
};



void TokenMgr::init()
{
    if ( s_gInstance != nullptr ) { return; }
    s_gInstance = new TokenMgr();
}

void TokenMgr::release()
{
    INNER_SAFE_DELETE(s_gInstance);
}

TokenMgr* TokenMgr::getInstance()
{
    return s_gInstance;
}




// static
bool TokenMgr::isCommentType(TokenBase* pToken)
{
    if ( pToken == nullptr ) {
        return false;
    }

    auto tp = pToken->getTokenType();
    return    ( tp == E_TOKEN_SINGLE_LINE_COMMENT 
            ||  tp == E_TOKEN_MULTI_LINE_COMMENT );
}


// static 
bool TokenMgr::isBlankType(TokenBase* pToken)
{
    return pToken!=nullptr &&  ( pToken->getTokenType() == E_TOKEN_BLANK );
}

// static 
bool TokenMgr::isIgnoredType(TokenBase* pToken)
{
    if ( pToken == nullptr ) {
        return false;
    }

    auto tp = pToken->getTokenType();
    return    ( tp == E_TOKEN_BLANK
            ||   ( tp == E_TOKEN_SINGLE_LINE_COMMENT 
            ||     tp == E_TOKEN_MULTI_LINE_COMMENT )
    );

}



TokenMgr::TokenMgr()
    : m_allTokenList()
    , m_validTokenList()
    , m_oneSentence()
    , m_openParenthesisList()
    , m_execodeIdx(0)
{
    m_allTokenList.clear();
    m_validTokenList.clear();

    m_oneSentence.clear();


    m_openParenthesisList.clear();
}

// virtual
TokenMgr::~TokenMgr()
{
    for( auto it = m_allTokenList.begin(); it != m_allTokenList.end(); ++it )
    {
        auto pToken = *it;
        INNER_SAFE_DELETE(pToken);
    }
    m_allTokenList.clear();

    m_validTokenList.clear();

    m_oneSentence.clear();
}



//
// Core Core Core : Key Logic
//
void TokenMgr::pushToken(TokenBase* pToken)
{
    using namespace charUtil;

    /*
    if ( pToken == nullptr ) {
        MyException e( E_THROW_NULL_PTR );

        string detail( __FILE__ );
        detail += (string(" TokenMgr::pushToken( nullptr ) @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }
    */

    // ****************************************************
    // the key logic ( critical )
    // ****************************************************
    auto pr = checkAdjacentTokensRelationship_IsValid( pToken );

    //
    // Always Push to All-Token-List,  No matter the relationship is valid or not  
    //    ( *** Must after *** previous check adjacent two tokens' relationship )
    //     Otherwise it will result in memory leak because of neglected token to be pushed into the memory pool
    //
    m_allTokenList.push_back(pToken);

    auto isValid = pr.first;
    if ( !isValid ) {
        auto previousToken = pr.second;
        MyException e(E_THROW_INVALID_TOKEN_RELATIONSHIP, pToken->getBeginPos() );

        string detailstr;
        if ( previousToken == nullptr ) {
            detailstr += "First Token ";
            detailstr += (SINGLE_QUOTO + pToken->getTokenContent() + SINGLE_QUOTO);

        } else {
            auto rightContent = pToken->getTokenContent();
            detailstr += (SINGLE_QUOTO + rightContent + SINGLE_QUOTO);

            detailstr += " After ";

            auto leftContent = previousToken->getTokenContent();
            detailstr += (SPACE_1 + SINGLE_QUOTO + leftContent + SINGLE_QUOTO);

            detailstr += " @";
            detailstr += pToken->getBeginPos().getPos(0);
        }

        detailstr += " is not allowed";
        e.setDetail(detailstr);
        throw e;
    }

    // treat  '('   ')'     ','
    // processParenthesisOrCommaFlag_IfNecessary(pToken); // set '('

    tracePushedTokenWarning(pToken);


     /******************************************************************************************************

    Key Code Here :

      Only push passed token into the     "m_oneSentence"       container until meet ';'
    Do <Not> process operator-stack and suffix-expression during the pushing step
    After all pushing till meet the ';' token   then build suffix expression by the statement's type

    *******************************************************************************************************/
    //      Skip :  Blank / Single-Comment / Multi-Comment   <===   such types can be ignored
    //      Operator or Sequence or    ;
    if ( !( TokenMgr::isIgnoredType(pToken) ) ) {
        m_validTokenList.push_back( pToken );

        if ( pToken->getTokenType() == E_TOKEN_SEMICOLON ) {
            // Core Core Core
            ++m_execodeIdx;
            executeCode_supportFunction();
        } else {
            m_oneSentence.push_back( pToken );
        }
    }

}


pair<bool,string>
TokenMgr::isLastValidTokenSemicolonOrEmpty()
{
    if ( m_validTokenList.empty() ) {
        return make_pair(true,"");
    }

    auto lastElement = m_validTokenList.back();
    auto tp = lastElement->getTokenType();
    auto content = lastElement->getTokenContent();
    auto isValid = (tp == E_TOKEN_SEMICOLON);
    return make_pair(isValid, content);
}



void TokenMgr::executeCode_supportFunction()
{
    if ( m_oneSentence.empty() ) {
        traceBlankStatement();
        // clearTmpGenTokenPool();
        return;
    }

    bool isFirstKeyWord = is1stTokenTypeKeyWord();
    if ( isFirstKeyWord ) {
        int sentenceLen = static_cast<int>( m_oneSentence.size() );
        int restIdx = -1;
        string strTypeKeyWords = m_oneSentence.front()->getTokenContent(); // set the type as the 1st token's content
        for( auto idx = 1; idx < sentenceLen; ++idx ) {
            auto token = m_oneSentence.at(idx);
            if (  token!=nullptr &&  token->isKeyword()  ) {
                strTypeKeyWords += " ";
                strTypeKeyWords += token->getTokenContent();

                if( (idx+1) > TokenMgr::s_MAX_KEYWORDS_CNT ) {
                    MyException e(E_THROW_TOO_MANY_KEYWORDS, m_oneSentence.at(idx)->getBeginPos() );
                    e.setDetail( strTypeKeyWords );
                    throw e;
                }
            } else {
                restIdx = idx;
                break;
            }
        }


        if ( restIdx == -1 ) {
            MyException e(E_THROW_ALL_TOKENS_ARE_KEYWORD, m_oneSentence.back()->getBeginPos() );
            throw e;
        }

        // get the corresponding data type
        auto mapedDtIt = s_keywordsDataTypeMap.find(strTypeKeyWords);
        if ( mapedDtIt == s_keywordsDataTypeMap.end() ) {
            MyException e(E_THROW_SENTENCE_UNKNOWN_DATA_TYPE , m_oneSentence.at(restIdx-1)->getBeginPos() );
            e.setDetail( string("\"") +  strTypeKeyWords + string("\"") );
            throw e;
        }


        int openParenthesisCnt = 0; // for storing    '('    list
        std::list< pair<int,int> > varbileDefGrp;
        /*
                                first is the beginIdx
                                second is the endIdx


                        beginIdx             endIdx       beginIdx             endIdx
                            |                  |             |                   |
                int         a   = (  3 + 4  )  ,             b =                 4;
        */
        varbileDefGrp.push_back( make_pair(-1, -1)  );

        auto meetError = false;
        TokenBase* errorToken = nullptr;

        for( auto idx = restIdx; idx < sentenceLen; ++idx ) {
            auto token = m_oneSentence.at(idx);
            auto needAdd = true;
            if ( token != nullptr ) {
                if ( token->getTokenType() == E_TOKEN_OPERATOR ) {
                    auto opType = token->getOperatorType();

                    if ( opType == E_OPEN_PARENTHESIS ) {
                        ++openParenthesisCnt;

                        /********************************************************************************************************************************************
                        if ( previousToken!=nullptr  ) {
                            if ( previousToken->isVarible() ) {
                                ++openParenthesisCnt;
                            } else {
                                // token->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE );
                                ++openParenthesisCnt;
                            }
                        } else {
                            //  previousToken == nullptr
                            // token->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE );
                            ++openParenthesisCnt;
                        }
                        *********************************************************************************************************************************************/
                    } else if ( opType == E_CLOSE_PARENTHESIS ) {
                        if ( openParenthesisCnt == 0  ) {
                            // no matched '('
                            errorToken = token;
                            meetError = true;
                            break;
                        }

                        --openParenthesisCnt;
                    } else if ( opType == E_COMMA ) {
                        if ( openParenthesisCnt == 0  ) {
                            // token->setContextRoleForOp( E_OP_COMMA_DEFINATION_SEP );
                            // start a new group to push new token expressions
                            needAdd = false;
                            varbileDefGrp.push_back( make_pair(-1,-1) ); // create a new pair tag
                            // Do not push seperator ','    into  varbileDefGrp.back() 
                        } 
                    }
                } 
            } else {
                // token == nullptr
                needAdd = false;
            }

            if ( needAdd ) {
                if ( varbileDefGrp.back().first == -1 && varbileDefGrp.back().second == -1 ) {
                    varbileDefGrp.back() = make_pair(idx,idx);
                } else {
                    varbileDefGrp.back().second = idx;
                }
            }
        }


        if ( meetError ) {
            varbileDefGrp.clear();
            MyException e(E_THROW_NO_MATCHED_OPEN_PARENTHESIS, errorToken->getBeginPos() );
            throw e;
        }

        // valid logic , 
        for( const auto& idxPr : varbileDefGrp ) {
            if (     (idxPr.first>=0  &&  idxPr.first  < sentenceLen)   
                  && (idxPr.second>=0 &&  idxPr.second < sentenceLen) ) {
                std::vector< TokenBase* > grpTokens;
                for( auto idx = idxPr.first; idx <= idxPr.second; ++idx ) {
                     grpTokens.push_back( m_oneSentence.at(idx) );
                }

                auto evaluator = ExpEvaluation::createANewEvaluator();
                if ( evaluator != nullptr ) {
                    evaluator->doNewVaribleDefEvaluation(mapedDtIt->second,  grpTokens, 0);
                }
            }
        }
    } else {
        // sentence's  first token is not <Keyword>
        auto evaluator = ExpEvaluation::createANewEvaluator();
        if ( evaluator != nullptr ) {
            auto needCheckVaribleIsExisted = true;
            evaluator->doNormalExpEvaluation( m_oneSentence, 0, needCheckVaribleIsExisted );
        }
    }

    m_oneSentence.clear();
    // after evaluation , clear memory
    ExpEvaluation::release();
}














bool TokenMgr::is1stTokenTypeKeyWord()
{
    auto headtoken = m_oneSentence.front();
    //                                                           here  : keyWord must be  type-key-word : int/long/ ... 
    return headtoken!=nullptr && headtoken->getTokenType() == E_TOKEN_EXPRESSION  &&  headtoken->isKeyword();
}












// *********************************************************************************************
// *** Core Core Core *** Function 
// *********************************************************************************************
//
//======================================================================================================================================================
//
//      : This function is check two adjacent token's relationship is valid or not
//
//======================================================================================================================================================
//
//   Key Logic    
//
pair<bool,TokenBase*> TokenMgr::checkAdjacentTokensRelationship_IsValid(TokenBase* toBePushed)
{
    auto avaliable = false;
    TokenBase* previousToken = nullptr;

    auto toBePushedTp = toBePushed->getTokenType();
    /***************************************************
         pair.first  : Skip Space && Comment , Valid one
         pair.second : the Closest One
    ****************************************************/
    auto pr = getPreviousToken();
    auto previousValidToken = pr.first;
    auto previousClosestToken = pr.second;

    switch ( toBePushedTp )
    {
    case E_TOKEN_BLANK:
    case E_TOKEN_SINGLE_LINE_COMMENT:
    case E_TOKEN_MULTI_LINE_COMMENT:
        {
            avaliable = true;
            previousToken = previousValidToken;
        }
        break;
    case E_TOKEN_EXPRESSION:
        {
            avaliable = process_SequenceWithPriorToken( toBePushed, previousValidToken);
            previousToken = previousValidToken;
        }
        break;
    case E_TOKEN_OPERATOR:
        {
            avaliable = process_OperatorWithPriorToken( toBePushed, previousValidToken, previousClosestToken);
            previousToken = previousValidToken;
        }
        break;
    case E_TOKEN_SEMICOLON:
        {
            avaliable = process_SemicolonWithPriorToken( toBePushed, previousValidToken);
            previousToken = previousValidToken;
        }
        break;
    default:
        break;
    }

    return make_pair(avaliable, previousToken);

}





// first  : skip space && comment , valid one
// second : the closest one
pair<TokenBase*,TokenBase*> TokenMgr::getPreviousToken()
{
    if ( m_allTokenList.empty() ) {
        return make_pair(nullptr,nullptr);
    }

    
    TokenBase* pTheClosestToken = m_allTokenList.back(); // the vector has at least one token , set the last token as the closest token
    TokenBase* pNoneBlankCommentToken = nullptr;
    for( auto rit = m_allTokenList.rbegin(); rit != m_allTokenList.rend(); ++rit )
    {
        auto pToken = *rit;
        if (  !TokenMgr::isIgnoredType(  pToken )  ) {
            pNoneBlankCommentToken = pToken;
            break;
        }
    }

    return make_pair(pNoneBlankCommentToken, pTheClosestToken);
}


void TokenMgr::tracePushedTokenWarning(TokenBase* pToken)
{
    using namespace charUtil;

    auto strWarning = pToken->getWarningContent();
    if (  CmdOptions::needPrintParseRuntimeWarning()    &&   !strWarning.empty() ) {
        auto tp = pToken->getTokenType();
        cerr << SC_WARNING_TITLE;
        if ( !(tp == E_TOKEN_SINGLE_LINE_COMMENT || tp == E_TOKEN_MULTI_LINE_COMMENT ) ) {
            auto content = pToken->getTokenContent();
            cerr << "\"" << content << "\"" << endl;
        }

        cerr << strWarning 
             << endl
             << endl;
    }
}




void TokenMgr::traceBlankStatement()
{
    using namespace charUtil;

    if ( CmdOptions::needPrintParseRuntimeWarning() &&  CmdOptions::needTreatBlankStatementAsWarning()  ) {
        cerr << SC_WARNING_TITLE;
        cerr << " Blank Statement is skipped ! . such as    '; ;' " << endl;
    }

}





//
// Scan all token inside the whole list, and decide whether it is full of comment(s) or blank(s)
//
bool TokenMgr::isAllTokensTrivial()
{
    // if ( m_allTokenList.empty() ) {
    //     return true;
    // }

    // bool bSkipFlag = true;
    for( auto it = m_validTokenList.cbegin();  it != m_validTokenList.cend(); ++it ) {
        if ( !TokenMgr::isIgnoredType( *it ) ) {
            // bSkipFlag = false;
            return false;
        }
    }

    return true;
}

bool TokenMgr::process_SemicolonWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken)
{
    if ( priorToken == nullptr ) {
        return true;
    }

    if (        priorToken->isKeyword() 
         ||   ( priorToken->getTokenType() == E_TOKEN_OPERATOR && priorToken->getOperatorType() != E_CLOSE_PARENTHESIS )  ) {
        return false;
    }

    return true;
}

bool TokenMgr::process_SequenceWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken)
{
    if ( priorToken == nullptr ) {
        return true;
    }

    auto preType = priorToken->getTokenType();
    if ( preType == E_TOKEN_SEMICOLON ) {
        return true;
    } else if ( preType == E_TOKEN_EXPRESSION  ) {
        if ( priorToken->isKeyword() && ( toBePushed->isVarible() || toBePushed->isKeyword() ) ) {
            return true;
        } else {
            return false;
        }
    } else {
        // preType == E_TOKEN_OPERATOR
        if (  toBePushed->isKeyword() ) {
            if ( priorToken->getOperatorType() == E_OPEN_PARENTHESIS ) {
                return true;
            } else {
                return false;
            }
        } else if ( priorToken->getOperatorType() == E_CLOSE_PARENTHESIS ) {
            // force type cast for new feature support
            /*
                 (int)3
                 (int)3.14
                 (int)a
                 (int)sin(3.14
                 ()char
            */
            return !toBePushed->isKeyword();
            // return false;
        } else {
            return true;
        }
    }
}

bool TokenMgr::process_OperatorWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken, TokenBase* priorClosestToken)
{
    auto opType = toBePushed->getOperatorType();
    if ( priorToken == nullptr ||  (priorToken->getTokenType() == E_TOKEN_SEMICOLON) ) {
        if (   ( opType == E_ADD   || opType == E_POSITIVE )
            || ( opType == E_MINUS || opType == E_NEGATIVE )
            || opType == E_BIT_NOT
            || opType == E_OPEN_PARENTHESIS ) {
            return true;
        } else {
            return false;
        }
    }

    auto preType = priorToken->getTokenType();
    if ( preType == E_TOKEN_EXPRESSION ) {
        if ( priorToken->isVarible() ) {
            if (  opType == E_BIT_NOT ) {
                return false;
            } else if ( opType == E_OPEN_PARENTHESIS ) {
                // e.g.   sin( ... )  for function call syntax support
                return true;
            } else {
                return true;
            }
        } else if ( priorToken->isKeyword() ) {
            // in order to support 
            // e.g.
            //       int a = (int)3.14f;        
            //                   ^
            return (opType == E_CLOSE_PARENTHESIS);
        } else {
            // int / float :  fixed literal number
            if (       opType == E_BIT_NOT 
                    || opType == E_OPEN_PARENTHESIS 
                    || (opType >= E_ASSIGNMENT && opType <= E_BIT_RIGHT_SHIFT_ASSIGNMENT) ) 
            {
                return false;
            } else {
                return true;
            }
        }
    }


    if ( preType == E_TOKEN_OPERATOR ) {
        auto foundMatched = false;
        auto closeFlag = false;
        auto sepFlag   = false;

        int leftOpType  = static_cast<int>( priorToken->getOperatorType() );
        int rightOpType = static_cast<int>( opType );

        if (   (leftOpType  >= static_cast<int>(E_ADD) &&  leftOpType  <= static_cast<int>(E_COMMA) ) 
            && (rightOpType >= static_cast<int>(E_ADD) &&  rightOpType <= static_cast<int>(E_COMMA)  )  ) 
        {

            closeFlag = TokenMgr::s_OpPairCfgTable[leftOpType][rightOpType].closeAvaliable;
            sepFlag   = TokenMgr::s_OpPairCfgTable[leftOpType][rightOpType].seperateAvaliable;
            foundMatched = true;
        }

        auto avaliable = false;
        if ( foundMatched ) {
            if ( closeFlag && sepFlag ) {
                avaliable = true;
            } else if ( !closeFlag && !sepFlag ) {
                avaliable = false;
            } else {
                if ( !closeFlag && sepFlag ) {
                    if ( priorToken == priorClosestToken ) {
                        avaliable = false;
                    } else {
                        avaliable = true;
                    }
                } 
                /* else {
                    // never execute code to here
                } 
                */
            }
        }

        return avaliable;
    }

    return false;
}



// static 
E_DataType TokenMgr::getDataTypeByString(const std::string typeName)
{
    auto mapedDtIt = s_keywordsDataTypeMap.find(typeName);
    if ( mapedDtIt == s_keywordsDataTypeMap.end() ) {
        return E_TP_UNKNOWN;
    }

    return mapedDtIt->second;
}




