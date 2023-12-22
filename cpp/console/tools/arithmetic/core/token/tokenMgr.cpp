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
        TokenMgr::OpPairCfg( false, false ),   // )  ~ 
        TokenMgr::OpPairCfg( true, true ),     // )  << 
        TokenMgr::OpPairCfg( true, true ),     // )  >> 
        TokenMgr::OpPairCfg( false, false ),   // )  ( 
        TokenMgr::OpPairCfg( true, true ),     // )  ) 
        // TokenMgr::OpPairCfg( false, false ),     // )  =      (a) = 3;      is valid
        // (a) = 3;  or  (a)=3;      is valid
        TokenMgr::OpPairCfg( true, true ),     // )  = 
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


// static 
std::list<TokenBase*> TokenMgr::s_generatedTmpTokenPool{};

void TokenMgr::init()
{
    if ( s_gInstance != nullptr ) { return; }
    s_gInstance = new TokenMgr();
}

void TokenMgr::release()
{
    clearTmpGenTokenPool();
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
    , m_opertorStack()
    , m_suffixExpression()
    , m_callstackFuncList()
    , m_oneArgument4ACertainFunc()
    , m_openParenthesisList()
	, m_opertorStack4OneArgument()
	, m_suffixExpression4OneArgument()
    , m_execodeIdx(0)
{
    m_allTokenList.clear();
    m_validTokenList.clear();

    m_oneSentence.clear();
    m_opertorStack.clear();
    m_suffixExpression.clear();

    m_callstackFuncList.clear();
    m_oneArgument4ACertainFunc.clear();
    m_openParenthesisList.clear();
	m_opertorStack4OneArgument.clear();
	m_suffixExpression4OneArgument.clear();
}

// virtual
TokenMgr::~TokenMgr()
{
    for( auto it = m_allTokenList.begin(); it != m_allTokenList.end(); ++it )
    {
        auto pToken = *it;
        if ( pToken != nullptr ) {
            delete pToken;
            pToken = nullptr;
        }
    }
    m_allTokenList.clear();

    m_validTokenList.clear();

    m_oneSentence.clear();
    m_opertorStack.clear();
    m_suffixExpression.clear();


    m_callstackFuncList.clear();
    clearOneArgumentsPool();
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

    // processPreviousFunctionRelated_IfNecessary(pr.second , iSpFlag); // set 'func'   and '('
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
            executeCode();
            ++m_execodeIdx;
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



void TokenMgr::executeCode_New()
{
    if ( m_oneSentence.empty() ) {
        // traceBlankStatement();
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


        int openParenthesisCnt = 0;
        std::list< pair<int,int> > varbileDefGrp;
        varbileDefGrp.push_back( make_pair(-1, -1)  );

        TokenBase* previousToken = nullptr;
        auto meetError = false;
        for( auto idx = restIdx; idx < sentenceLen; ++idx ) {
            auto token = m_oneSentence.at(idx);
            auto needAdd = true;
            if ( token != nullptr ) {
                if ( token->getTokenType() == E_TOKEN_OPERATOR ) {
                    auto opType = token->getOperatorType();

                    if ( opType == E_OPEN_PARENTHESIS ) {
                        if ( previousToken!=nullptr  ) {
                            if ( previousToken->isVarible() ) {
                                // TODO : check is function name existed ?
                                // token->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_FUNCTION_START );
                                ++openParenthesisCnt;
                            } else if ( previousToken->isFixedLiteral() ) {
                                //   3(...  or   07(...    or   0x4(     or   3.14(...    or  3.14f(
                                meetError = true;
                                break;
                            } else {
                                // token->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE );
                                ++openParenthesisCnt;
                            }
                        } else {
                            //  previousToken == nullptr
                            // token->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE );
                            ++openParenthesisCnt;
                        }

                    } else if ( opType == E_CLOSE_PARENTHESIS ) {
                        if ( openParenthesisCnt == 0  ) {
                            // no matched '('
                            meetError = true;
                            break;
                        }

                        --openParenthesisCnt;
                    } else if ( opType == E_COMMA ) {
                        if ( openParenthesisCnt == 0  ) {
                            // token->setContextRoleForOp( E_OP_COMMA_DEFINATION_SEP );
                            // start a new group to push new token expressions
                            needAdd = false;
                            varbileDefGrp.push_back( make_pair(-1,-1) ); // create a new pair 
                            // Do not push it into  varbileDefGrp.back() 
                        } 
                    }
                } 
            } else {
                // token == nullptr;
                needAdd = false;
            }

            if ( needAdd ) {
                if ( varbileDefGrp.back().first == -1 && varbileDefGrp.back().second == -1 ) {
                    varbileDefGrp.back() = make_pair(idx,idx);
                } else {
                    varbileDefGrp.back().second = idx;
                }
            }

            previousToken = token;
        }


        if ( meetError ) {
            varbileDefGrp.clear();
            // TODO throw a certain exception
        }

        

        // valid logic , 
        for( const auto& idxPr : varbileDefGrp ) {
            if (     (idxPr.first>=0  &&  idxPr.first  < sentenceLen)   
                  && (idxPr.second>=0 &&  idxPr.second < sentenceLen) ) {
                std::vector< TokenBase* > grpTokens;
                for( auto idx = idxPr.first; idx <= idxPr.second; ++idx ) {
                     grpTokens.push_back( m_oneSentence.at(idx) );
                }

                // TODO : 
                // define a new varible : mapedDtIt.second;
                auto evaluator = ExpEvaluation::createANewEvaluator();
                if ( evaluator != nullptr ) {
                    evaluator->doNewVaribleDefEvaluation(mapedDtIt->second,  grpTokens, 0);
                }
            }
        }
    } else {
        // sentence's  first token is not <Keyword>
        // TODO :
        auto evaluator = ExpEvaluation::createANewEvaluator();
        if ( evaluator != nullptr ) {
            evaluator->doNormalExpEvaluation( m_oneSentence, 0 );
        }
    }

}


//////////////////////////////////////////////////////////////////////
//
// Core Core Core
//
//////////////////////////////////////////////////////////////////////
void TokenMgr::executeCode()
{
    printOperatorStack();
    m_opertorStack.clear();
    printSuffixExpression(1);
    m_suffixExpression.clear();


    m_callstackFuncList.clear();
    clearOneArgumentsPool();
    m_openParenthesisList.clear();
	m_opertorStack4OneArgument.clear();
	m_suffixExpression4OneArgument.clear();

    //   m_oneSentence is a list without  ';'
    if ( m_oneSentence.empty() ) {
        traceBlankStatement();
        clearTmpGenTokenPool();
        return;
    }
    

    // Sequence / operator   Only
    int vecSz = static_cast<int>( m_oneSentence.size() );

    /*
    --------------------------------------------------
        Only 3 kinds of format are all allowed
    --------------------------------------------------

        1. DataType    varible                   (;)    type-id = 1    |  e.g. :   int index;
        2. DataType    varible = expression      (;)    type=id = 2    |  e.g. :   int index = 1+a;
        3. Normal expression or tmp expression   (;)    such as  a = 3;  / a += 3;  /       a + b * c;  ( the previous statement is a kind of tmp expression)

        ///////////////////////////////////////////////////////////////////////
        //
        // Core Core Core : build suffix-expression after '='
        //
        ///////////////////////////////////////////////////////////////////////

    */
    int sentenceType = 0;
    bool isFirstKeyWord = is1stTokenTypeKeyWord();
    E_DataType defDt = E_TP_UNKNOWN;
    string varname;
    int varibleIdx = 0;

    if ( isFirstKeyWord ) {
        // such as  
        //          int a
        //          long int a
        //          unsigned long long int a
        /*
            require extra dataType keyword
                                   varible Name
                                   =  or  ;
        */
        int firstNotKeyWordIdx = -1;
        int firstNotKeyWordType = -1;
        int additionalSuccessiveKeyWordsCnt = 0;
        string strDataType = m_oneSentence.at(0)->getTokenContent();
        for( auto idx = 1; idx < vecSz; ++idx )
        {
            TokenBase* pToken = m_oneSentence.at(idx);
            auto tokenType = pToken->getTokenType();
            if ( tokenType == E_TOKEN_EXPRESSION  ) {
                if ( pToken->isKeyword() ) {
                    strDataType += " ";
                    strDataType += pToken->getTokenContent();

                    ++additionalSuccessiveKeyWordsCnt;
                } else if ( pToken->isVarible() ) {
                    varibleIdx = idx;
                    firstNotKeyWordIdx = idx;
                    firstNotKeyWordType = 0; // 0 :  first Un-Keyword token is a name of varible 
                    break;
                } else {
                    firstNotKeyWordIdx = idx;
                    firstNotKeyWordType = 1;     // 1 :  first Un-Keyword token is fixed number literal such as  3   3.14   3f
                    break;
                }
            } else {
                firstNotKeyWordIdx = idx;
                firstNotKeyWordType = 2;     // 2 :  first Un-Keyword token is Operator Type
                break;
            }
        }



        if ( firstNotKeyWordIdx == -1 ) {
            MyException e(E_THROW_ALL_TOKENS_ARE_KEYWORD, m_oneSentence.front()->getBeginPos() );
            throw e;
        } else {
            if ( firstNotKeyWordType == 0 ) {
                // Varible Type
                int keyWordCnt = additionalSuccessiveKeyWordsCnt + 1;
                if ( keyWordCnt > TokenMgr::s_MAX_KEYWORDS_CNT ) {
                    MyException e(E_THROW_TOO_MANY_KEYWORDS, m_oneSentence.at(firstNotKeyWordIdx-1)->getEndPos() );
                    e.setDetail( strDataType );
                    throw e;
                } else {
                    // keyWordCnt <= TokenMgr::s_MAX_KEYWORDS_CNT
                    auto mapedDtIt = s_keywordsDataTypeMap.find(strDataType);
                    if ( mapedDtIt == s_keywordsDataTypeMap.end() ) {
                        MyException e(E_THROW_SENTENCE_UNKNOWN_DATA_TYPE , m_oneSentence.at(firstNotKeyWordIdx)->getBeginPos() );
                        e.setDetail( string("\"") +  strDataType + string("\"") );
                        throw e;
                    }

                    defDt = mapedDtIt->second;
                    varname = m_oneSentence.at( firstNotKeyWordIdx )->getTokenContent();
                }

                // <keyword0>  <keyword1> <keywordLast>     varible
                if ( firstNotKeyWordIdx == (vecSz-1) ) {
                    // e.g.    int a;
                    sentenceType = 1;
                } else {
                    // int a ?
                    TokenBase* nextToken = nullptr;
                    if (                                 (firstNotKeyWordIdx+1 < vecSz)
                         && (nextToken = m_oneSentence.at(firstNotKeyWordIdx+1)) != nullptr 
                         && (nextToken->getTokenType() == E_TOKEN_OPERATOR &&  nextToken->getOperatorType() == E_ASSIGNMENT)  ) { 
                        // <keyword> ...    <varible> = 
                        // such as    int a = ...
                        sentenceType = 2;
                    } else {
                        if ( nextToken == nullptr ) {
                            MyException e(E_THROW_SENTENCE_DEFINITION_HAS_MISSED_AN_ASSIGNMENT_OPERATOR);
                            throw e;
                        } else {
                            MyException e(E_THROW_SENTENCE_DEFINITION_HAS_MISSED_AN_ASSIGNMENT_OPERATOR ,  nextToken->getBeginPos()   );
                            throw e;
                        }
                    }
                }
            } else if ( firstNotKeyWordType == 1 || firstNotKeyWordType == 2 ) {
                // firstNotKeyWordType == 1 , not allowed       such as.  :   int 3  /   int 3.14 
                // firstNotKeyWordType == 2 , not allowed       such as.  :   int +
                MyException e(E_THROW_SENTENCE_DEFINITION_PREFIX_IS_NOT_VARIBLE, m_oneSentence.at(firstNotKeyWordIdx)->getBeginPos()  );
                throw e;
            }
        }
    } else {
        //  assigment expression  /  temp expression 
        //  a = 3;                /   3+4  / a+3     /   (a) = 4;
        sentenceType = 3;
    }


    VaribleInfo* pVaribleInfo = nullptr;
    TokenBase* sentenceVarElement = nullptr;
    if ( sentenceType == -1 ) {
        MyException e(E_THROW_SENTENCE_DEFINITION_PREFIX_IS_NOT_VARIBLE, m_oneSentence.at( varibleIdx )->getBeginPos()  );
        throw e;
    }
    
    //   int a;            ||      int a = ... ;
    if ( sentenceType == 1 ||  sentenceType == 2 ) {
        sentenceVarElement = m_oneSentence.at(varibleIdx);
        pVaribleInfo = VariblePool::getPool()->create_a_new_varible(defDt, varname, sentenceVarElement->getBeginPos().line );
        if ( pVaribleInfo != nullptr  ) {
            // Random a value and assign it to new alloced varible's
            VariblePool::getPool()->randomVaribleValue( varname );

            if ( sentenceVarElement != nullptr ) { 
                sentenceVarElement->setDataType( pVaribleInfo->dataVal.type );
                sentenceVarElement->setRealValue( pVaribleInfo->dataVal );
            } else {
                MyException e(E_THROW_VARIBLE_IS_MISSING);
                e.setDetail( string("Index ") + to_string( varibleIdx ) + string(" ") + m_oneSentence.at( varibleIdx )->getTokenContent() + string(" is not a varible") );
                throw e;
            }
        }

        // e.g.   int a;
    } 

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Has   = or ?=
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool hasAssignmentOrVariadicAssignment = false;
    for( auto idx = 0; idx < vecSz; ++idx )
    {
        TokenBase* pToken = m_oneSentence.at(idx);
        auto tokenType = pToken->getTokenType();

        if ( tokenType == E_TOKEN_OPERATOR  ) {  
            E_OperatorType opType = pToken->getOperatorType();
            // has  =   or    ?= ( += -=    ...  >>= )
            if ( opType >= E_ASSIGNMENT &&  opType <= E_BIT_RIGHT_SHIFT_ASSIGNMENT ) {
                hasAssignmentOrVariadicAssignment = true;
                break;
            }
        }
    }

    if ( sentenceType != 1 ) {
        // sentenceType == 2  :    e.g.   int a = ...;
        // sentenceType == 3  :    e.g.       a = ...;      or    a+b
        buildSuffixExpression(sentenceType, (sentenceType==2 ? varibleIdx : 0) );
        checkSuffixExpressionValid();
        popAllOperatorStack();
        printSuffixExpression(2);

        // Core Core Core
        // Should Open flag "PROCESS_TMP_EXPRESSION_WITHOUT_ASSIGNMENT" as true
        if ( hasAssignmentOrVariadicAssignment || CmdOptions::needProcessTmpExpressionWithoutAssignment()   ) {
            evaluateSuffixExpression();
        }
        printSuffixExpression(3);
        m_suffixExpression.clear(); // clear the only 1 element
    } 

    // After Execute , clear
    m_oneSentence.clear();
    clearTmpGenTokenPool();



    m_callstackFuncList.clear();
    clearOneArgumentsPool();
    m_openParenthesisList.clear();
	m_opertorStack4OneArgument.clear();
	m_suffixExpression4OneArgument.clear();

}





/************************************************************
 *                  Core Core Core                          *
*************************************************************/
void TokenMgr::buildSuffixExpression(int sentenceType, int varibleIdx)
{
    int sententSz = static_cast<int>( m_oneSentence.size() );

    for( int idx = varibleIdx; idx < sententSz; ++idx )
    {
        auto pToken = m_oneSentence.at(idx);
        auto tokenType = pToken->getTokenType();
        if ( tokenType == E_TOKEN_EXPRESSION ) {
            // fixed literal / varible / KeyWord
            if ( pToken->isKeyword() ) {
                MyException e(E_THROW_CANNOT_PUSH_TOKEN_KEYWORD, pToken->getBeginPos() );
                throw e;
            } else if ( pToken->isVarible() ) {
                string pVisitVarName = pToken->getTokenContent();

                VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( pVisitVarName );
                if ( pVisitedVaribleInfo == nullptr ) {
                    MyException e(E_THROW_VARIBLE_NOT_DEFINED , pToken->getBeginPos());
                    e.setDetail( pVisitVarName );
                    throw e;
                }

                auto indexCheck = false;
                if ( sentenceType == 4 ) {
                    indexCheck = true;
                } else {
                    indexCheck = (idx > varibleIdx);
                }

                (void)indexCheck;
                /*
                if ( indexCheck &&  !(pVisitedVaribleInfo->isInitialed) ) {
                    if ( CmdOptions::needTreatUninitializedVaribleAsError()  ) {
                        MyException e(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED, pToken->getBeginPos() );
                        throw e;
                    } else {
                        traceUnInitializedVarWhenUsed(pToken);
                    }
                }
                */

                pToken->setDataType( pVisitedVaribleInfo->dataVal.type );
                pToken->setRealValue( pVisitedVaribleInfo->dataVal );
            }

            m_suffixExpression.push_back( pToken );
            traceSuffixExpression( pToken , true);
        } else {
            // must be a type of operator
            TokenBase* previousToken;
            auto previousIdx = (idx - 1);
            if ( previousIdx < varibleIdx ) {
                previousToken = nullptr;
            } else {
                previousToken = m_oneSentence.at(previousIdx);
            }
            processOperatorStack(previousToken, pToken);
        }
    }
}


void TokenMgr::checkSuffixExpressionValid()
{
    TokenBase* errorToken = nullptr;
    for( auto it = m_suffixExpression.begin(); it != m_suffixExpression.end(); ++it )
    {
        auto pToken = *it;
        auto tp = pToken->getTokenType();

        if ( tp == E_TOKEN_EXPRESSION ) {
            if ( !(pToken->isVarible() || pToken->isFixedLiteral()) ) {
                errorToken = pToken;
                break;
            }
        } else if ( tp == E_TOKEN_OPERATOR ) {
            auto opTp = pToken->getOperatorType();
            if ( opTp == E_OPERATOR_UNKNOWN || opTp == E_OPEN_PARENTHESIS || opTp == E_CLOSE_PARENTHESIS ) {
                errorToken = pToken;
                break;
            }
        } else {
            errorToken = pToken;
            break;
        }
    }


    if ( errorToken != nullptr ) {
        // auto errorTp = errorToken->getTokenType();
        MyException e(E_THROW_INVALID_SUFFIX_EXPRESSION, errorToken->getBeginPos() );
        // e.setDetail( EnumUtil::enumName(errorTp)  + string(" , ") +  errorToken->getBeginPos().getPos() );
        throw e;
    }
}


bool TokenMgr::is1stTokenTypeKeyWord()
{
    auto headtoken = m_oneSentence.front();
    //                                                           here  : keyWord must be  type-key-word : int/long/ ... 
    return headtoken!=nullptr && headtoken->getTokenType() == E_TOKEN_EXPRESSION  &&  headtoken->isKeyword();
}







void TokenMgr::processOperatorStack(TokenBase* previousToken, TokenBase* pToken)
{
    // change operator if original operator is '+' / '-'
    // '+' Addition    -->     '+' Positive
    // '-' Minus       -->     '-' Negative
    auto opType = pToken->getOperatorType();
    if ( opType == E_ADD   || opType == E_MINUS ) {
        E_OperatorType changedTp = opType;

        auto isAdd = (opType == E_ADD);
        if ( previousToken == nullptr ) {
            // Positive  or Negative
            changedTp = (isAdd ? E_POSITIVE : E_NEGATIVE);
            pToken->setOpType(changedTp);

            tracePositiveNegativeFlag( previousToken, opType );

            opType = changedTp;

        } else {
            auto previousTp = previousToken->getTokenType();
            if ( previousTp == E_TOKEN_OPERATOR   &&   previousToken->getOperatorType() != E_CLOSE_PARENTHESIS ) {
                changedTp = (isAdd ? E_POSITIVE : E_NEGATIVE);

                tracePositiveNegativeFlag( previousToken, opType );

                pToken->setOpType(changedTp);


                opType = changedTp;
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Core Core Core
    //
    //    Generate Suffix Expression
    if ( opType == E_CLOSE_PARENTHESIS ) {
        if ( !hasPreviousExistOpenParenthesis() ) {
            MyException e(E_THROW_NO_MATCHED_OPEN_PARENTHESIS, pToken->getBeginPos() );
            // e.setDetail( pToken->getBeginPos().getPos() );
            throw e;
        } else {
            //  toPushed token  is ')' , 
            //    Do not push ')'      
            //    Pop the previous closest operator till   '('
            popUntilOpenParenthesis();
        }
    } else {

        // operator is not ')'
        if ( m_opertorStack.empty() ) { // stack is empty
            m_opertorStack.push_back( pToken );
            traceOperatorStack( pToken, true);
            return;
        }

        // else :
        //      list is not empty
        auto toPushedOpMeta    = OpUtil::getOpInfo(opType);
        auto toPushed_p = toPushedOpMeta.getPriority();
        // auto current_rightAss = toPushedOpMeta.isRight2Left();
        auto forwardIt = m_opertorStack.begin();

        auto pushedFlag = false;
        for( auto rit = m_opertorStack.rbegin(); rit != m_opertorStack.rend(); )
        {
            auto pVisitToken = *rit;
            auto visitOpType = pVisitToken->getOperatorType();
            auto cmpOpMeta = OpUtil::getOpInfo( visitOpType );
            auto cmp_p = cmpOpMeta.getPriority();
            auto cmp_rightAss = toPushedOpMeta.isRight2Left();

            if ( toPushed_p < cmp_p ) {
                m_opertorStack.insert( rit.base(), pToken );
                traceOperatorStack( pToken, true );

                pushedFlag = true;
            } else if ( toPushed_p == cmp_p ) {
                if ( opType == E_OPEN_PARENTHESIS ) { // '(' is right-to-Left Associativity
                    m_opertorStack.insert( rit.base(), pToken );
                    traceOperatorStack( pToken, true );
                } else {
                    if ( cmp_rightAss ) {
                        // Associativity is   right 2 left
                        forwardIt = m_opertorStack.insert( rit.base(), pToken );
                        traceOperatorStack( pToken, true );
                        rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                    } else {
                        // Associativity is   left 2 right , 
                        //   1. pop only one : the current
                        //   2. copy current op into  m_suffixExpression  ( move the token from opStack --> suffixExpression  which is metioned @ step1 )
                        //   3. insert the same priority operator

                        forwardIt = m_opertorStack.erase( (++rit).base() ); // 1
                        rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                        // traceOperatorStack( pVisitToken, false );

                        m_suffixExpression.push_back( pVisitToken ); // 1
                        // traceSuffixExpression( pVisitToken, true );
                        traceOpMove2SuffixExpression( pVisitToken );


                        m_opertorStack.insert( forwardIt, pToken ); // 3
                        rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                        traceOperatorStack( pToken, true );
                    }
                }

                pushedFlag = true;
            } else {
                // toPushed_p > cmp_p
                if ( visitOpType == E_OPEN_PARENTHESIS ) {
                    forwardIt = m_opertorStack.insert( rit.base(), pToken );
                    rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                    traceOperatorStack( pToken, true );

                    pushedFlag = true;
                } else {
                    // traceOperatorStack( pVisitToken, false );
                    forwardIt = m_opertorStack.erase( (++rit).base() );
                    rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                    m_suffixExpression.push_back( pVisitToken );
                    // traceSuffixExpression( pVisitToken, true );

                    traceOpMove2SuffixExpression(pVisitToken);
                }
            }

            if ( pushedFlag ) {
                break;
            }
        }


        if ( !pushedFlag ) {
            m_opertorStack.push_back( pToken );
            traceOperatorStack( pToken, true );
        }
    }
}


void TokenMgr::evaluateSuffixExpression()
{
    int operatorProcessCnt = 0; // count for do binaryOp/UnaryOp Cnt
    int previousExpCnt = 0;
    for( auto it = m_suffixExpression.begin(); it != m_suffixExpression.end(); ) 
    {
        auto currentElement = *it;
        auto currentTokenType = currentElement->getTokenType();
        if ( currentTokenType != E_TOKEN_OPERATOR ) {
            ++it;
            ++previousExpCnt;
        } else {
            // is an operator
            auto opType = currentElement->getOperatorType();
            if ( opType == E_OPERATOR_UNKNOWN || opType == E_OPEN_PARENTHESIS || opType == E_CLOSE_PARENTHESIS ) {
                MyException e(E_THROW_INVALID_OPERATOR , currentElement->getBeginPos() );
                string detailstr = "inside suffix expression : ";
                detailstr += EnumUtil::enumName( opType );
                detailstr += (string(", ") + currentElement->getBeginPos().getPos() );
                e.setDetail( detailstr );
                throw e;
            }

            auto opMeta = OpUtil::getOpInfo( opType );
            auto isbinaryOp = !opMeta.isUnaryOp();
            if ( isbinaryOp ) {
                if ( previousExpCnt < 2 ) {
                    MyException e(E_THROW_SUFFIXEXPR_BINARY_OP_MISS_TWO_OPERANDS, currentElement->getBeginPos() );
                    // e.setDetail( currentElement->getBeginPos().getPos() );
                    throw e;
                }

                auto nextIt = next(it);

                TokenBase* leftOperand  = nullptr;
                TokenBase* rightOperand = nullptr;

                it = prev(it,1);
                rightOperand = *it;

                it = prev(it,1);
                leftOperand = *it;

                if (   CmdOptions::needTraceUninitializedVaribleWhenEvaluatingExpression() ) {
                    // opType != E_ASSIGNMENT
                    // check Left Operand
                    auto needCheckLeft = (opType != E_ASSIGNMENT);
                    if ( needCheckLeft ) {
                        if ( leftOperand!=nullptr &&  leftOperand->getTokenType() == E_TOKEN_EXPRESSION    &&   leftOperand->isVarible() ) {
                            string varName = leftOperand->getTokenContent();
                            VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( varName );
                            if ( pVisitedVaribleInfo != nullptr  &&   !pVisitedVaribleInfo->isInitialed ) {
                                if ( CmdOptions::needTreatUninitializedVaribleAsError()  ) {
                                    MyException e(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED, leftOperand->getBeginPos() );
                                    throw e;
                                } else {
                                    traceUnInitializedVarWhenUsed(leftOperand);
                                }
                            }
                        }
                    }

                    //
                    // Not matter whether the operator is '=' or not , always check Right operand 
                    // 
                    if ( rightOperand!=nullptr &&  rightOperand->getTokenType() == E_TOKEN_EXPRESSION    &&   rightOperand->isVarible() ) {
                        string varName = rightOperand->getTokenContent();
                        VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( varName );
                        if ( pVisitedVaribleInfo != nullptr  &&   !pVisitedVaribleInfo->isInitialed ) {
                            if ( CmdOptions::needTreatUninitializedVaribleAsError()  ) {
                                MyException e(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED, rightOperand->getBeginPos() );
                                throw e;
                            } else {
                                traceUnInitializedVarWhenUsed(rightOperand);
                            }
                        }
                    }
                }

                auto genTmpExp = doBinaryOp(leftOperand, currentElement, rightOperand);
                ++operatorProcessCnt;

                it = m_suffixExpression.erase(it, nextIt);
                it = m_suffixExpression.insert(it, genTmpExp);

                // previousExpCnt -= 3;
                // previousExpCnt += 1;
                previousExpCnt -= 2;
            } else {
                if ( previousExpCnt < 1 ) {
                    MyException e(E_THROW_SUFFIXEXPR_UNARY_OP_MISS_ONE_OPERAND, currentElement->getBeginPos());
                    // e.setDetail( currentElement->getBeginPos().getPos() );
                    throw e;
                }

                auto nextIt = next(it);
                TokenBase* rightOperand = nullptr;

                it = prev(it,1);
                rightOperand = *it;

                if ( CmdOptions::needTraceUninitializedVaribleWhenEvaluatingExpression() ) {
                    // check the only right Operand for Unary operator expression
                    if ( rightOperand!=nullptr &&  rightOperand->getTokenType() == E_TOKEN_EXPRESSION    &&   rightOperand->isVarible() ) {
                        string varName = rightOperand->getTokenContent();
                        VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( varName );
                        if ( pVisitedVaribleInfo != nullptr  &&   !pVisitedVaribleInfo->isInitialed ) {
                            if ( CmdOptions::needTreatUninitializedVaribleAsError()  ) {
                                MyException e(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED, rightOperand->getBeginPos() );
                                throw e;
                            } else {
                                traceUnInitializedVarWhenUsed(rightOperand);
                            }
                        }
                    }
                }

                auto genTmpExp = doUnaryOp(currentElement, rightOperand);
                ++operatorProcessCnt;

                it = m_suffixExpression.erase(it, nextIt);
                it = m_suffixExpression.insert(it, genTmpExp);

                // previousExpCnt -= 2;
                // previousExpCnt += 1;
                --previousExpCnt;
            }

        }
    }

    // special condition if tmp expression has no operator
    if (  operatorProcessCnt == 0 ) {
        for( auto it = m_suffixExpression.begin(); it != m_suffixExpression.end(); ++it ) 
        {
            auto currentElement = *it;
            auto currentTokenType = currentElement->getTokenType();
            if ( currentTokenType == E_TOKEN_EXPRESSION  ) {
                auto content = currentElement->getTokenContent();
                auto dataVal = currentElement->getRealValue();
                traceTmpOpResult(content , dataVal);
            } else {
                MyException e(E_THROW_CODE_CANNOT_REACH_HERE, currentElement->getBeginPos()  );
                e.setDetail(" When operatorStack is empty or binary/unary operator's count == 0 ");
                throw e;
            }
        }
    }
}


TokenBase* TokenMgr::doBinaryOp(TokenBase* left, TokenBase* op, TokenBase* right)
{
    TokenBase* genTmpExp = nullptr;

    switch( op->getOperatorType() )
    {
    case E_ADD:
        genTmpExp = doAdd(left,right);
        break;
    case E_MINUS:
        genTmpExp = doMinus(left,right);
        break;
    case E_MULTIPLY:
        genTmpExp = doMultiply(left,right);
        break;
    case E_DIVIDE:
        genTmpExp = doDivide(left,right);
        break;
    case E_MOD:
        genTmpExp = doMod(left,right);
        break;
    case E_BIT_AND:
        genTmpExp = doBitAnd(left,right);
        break;
    case E_BIT_OR:
        genTmpExp = doBitOr(left,right);
        break;
    case E_BIT_XOR:
        genTmpExp = doBitXor(left,right);
        break;
    case E_BIT_LEFT_SHIFT:
        genTmpExp = doBitLeftShift(left,right);
        break;
    case E_BIT_RIGHT_SHIFT:
        genTmpExp = doBitRightShift(left,right);
        break;
    case E_ASSIGNMENT:
        genTmpExp = doAssignment(left,right);
        break;
    case E_ADD_ASSIGNMENT:                 // +=
        genTmpExp = do_Add_Assignment(left, right);
        break;
    case E_MINUS_ASSIGNMENT:               // -=
        genTmpExp = do_Minus_Assignment(left, right);
        break;
    case E_MULTIPLY_ASSIGNMENT:            // *=
        genTmpExp = do_Multiply_Assignment(left, right);
        break;
    case E_DIVIDE_ASSIGNMENT:              // /=
        genTmpExp = do_Divide_Assignment(left, right);
        break;
    case E_MOD_ASSIGNMENT:                 // %=
        genTmpExp = do_Mod_Assignment(left, right);
        break;
    case E_BIT_AND_ASSIGNMENT:             // &=
        genTmpExp = do_Bit_And_Assignment(left, right);
        break;
    case E_BIT_OR_ASSIGNMENT:              // |=
        genTmpExp = do_Bit_Or_Assignment(left, right);
        break;
    case E_BIT_XOR_ASSIGNMENT:             // ^=
        genTmpExp = do_Bit_Xor_Assignment(left, right);
        break;
    case E_BIT_LEFT_SHIFT_ASSIGNMENT:      // <<=
        genTmpExp = do_Bit_LeftShift_Assignment(left, right);
        break;
    case E_BIT_RIGHT_SHIFT_ASSIGNMENT:     // >>=
        genTmpExp = do_Bit_RightShift_Assignment(left, right);
        break;
    default:
        break;
    }

    return genTmpExp;
}

TokenBase* TokenMgr::doUnaryOp(TokenBase* op, TokenBase* right)
{
    TokenBase* genTmpExp = nullptr;
    switch( op->getOperatorType() )
    {
    case E_POSITIVE:
        genTmpExp = doPositive(op,right);
        break;
    case E_NEGATIVE:
        genTmpExp = doNegative(op,right);
        break;
    case E_BIT_NOT:
        genTmpExp = doBitNot(op,right);
        break;
    default:
        break;
    }

    return genTmpExp;
}



void TokenMgr::popUntilOpenParenthesis()
{
    list<TokenBase*> movedTokenList;
    auto meetOpenParenthese = false;
    for( auto rit = m_opertorStack.rbegin(); rit != m_opertorStack.rend(); )
    {
        TokenBase* pElement = *rit;
        auto isopenParenthesis = ( pElement->getOperatorType() == E_OPEN_PARENTHESIS );

        movedTokenList.push_back( pElement );
        // traceOperatorStack( pElement, false );
        auto fit = m_opertorStack.erase( (++rit).base() );
        rit = reverse_iterator< decltype(fit) >( fit );

        if ( isopenParenthesis ) {
            // meet the matched    '(' ,   erase (drop) it *** without *** push it into suffix expression
            meetOpenParenthese = true;
            break;
        } else {
            m_suffixExpression.push_back( pElement );
            // traceSuffixExpression( pElement , true);
        }
    }

    traceSomeTokensFromOpMove2SuffixExpression( movedTokenList, meetOpenParenthese );

}





E_DataType TokenMgr::checkPrefixKeyWordsAndGetDataType(int varIdx, string& varname, bool hasTokenEqual )
{
    using namespace charUtil;

    E_DataType dt = E_TP_UNKNOWN;

    auto isValid = true;
    // auto errorIdx = -1;
    // auto lastIdx = vecSz - 1;
    auto keywordsCnt = 0;
    string keywordsSeq;
    for( auto idx = 0; idx < varIdx; ++idx )
    {
        auto pToken = m_oneSentence.at(idx);
        auto content = pToken->getTokenContent();

        if ( !(pToken->getTokenType() == E_TOKEN_EXPRESSION  && pToken->isKeyword() ) ) {
            // errorIdx = idx;
            isValid = false;
            break;
        } else {
            ++keywordsCnt;
            keywordsSeq += content;
            if ( idx != (varIdx-1) ) {
                keywordsSeq += SPACE_1;
            }
        }
    }

    if ( !isValid ) {
        string strWithVarible;
        for( auto idx = 0; idx <= varIdx; ++idx ) { strWithVarible += m_oneSentence.at(idx)->getTokenContent(); }

        E_ExceptionType errorCode = (hasTokenEqual  ? E_THROW_SENTENCE_PREFIX_TOKENS_BEFORE_EQUAL_ARE_INVALID 
                                                    : E_THROW_SENTENCE_PREFIX_TOKENS_ARE_INVALID);
        MyException e(errorCode);
        string errorMsg = ( SINGLE_QUOTO + strWithVarible + SINGLE_QUOTO + " @line: " + to_string( m_oneSentence.at(varIdx)->getBeginPos().line ) ); 
        e.setDetail(errorMsg);
        throw e;
    }

    if ( keywordsCnt > s_MAX_KEYWORDS_CNT ) {
        MyException e(E_THROW_SENTENCE_DEFINITION_TOO_MANY_KEYWORDS, m_oneSentence.at(0)->getBeginPos() );
        throw e;
    }

    auto varElement = m_oneSentence.at(varIdx);
    if ( !(varElement->getTokenType() == E_TOKEN_EXPRESSION    &&   varElement->isVarible()) ) {
        MyException e(E_THROW_SENTENCE_DEFINITION_PREFIX_IS_NOT_VARIBLE , varElement->getBeginPos() );
        throw e;
    }
    varname = varElement->getTokenContent();

    auto it = s_keywordsDataTypeMap.find(keywordsSeq);
    if ( it == s_keywordsDataTypeMap.end() ) {
        MyException e(E_THROW_SENTENCE_UNKNOWN_DATA_TYPE, varElement->getBeginPos() );
        e.setDetail( string("\"") + keywordsSeq + string("\"") );
        throw e;
    }

    dt = it->second;
    return dt;
}


bool TokenMgr::hasPreviousExistOpenParenthesis()
{
    auto foundLeft = false;
    
    for( auto rit = m_opertorStack.rbegin(); rit != m_opertorStack.rend(); ++rit )
    {
        TokenBase* pElement = *rit;
        if ( pElement->getOperatorType() == E_OPEN_PARENTHESIS )  {
            foundLeft = true;
            break;
        }
    }

    return foundLeft;
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













//####################################################################################################
//
//      Binary Op
TokenBase* TokenMgr::doAdd(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_ADD + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    E_DataType retDt = leftVal.type;
    DataValue sumRet = leftVal + rightVal;
    traceTmpOpResult(finalExpr, sumRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( sumRet );

    return ret;
}

TokenBase* TokenMgr::doMinus(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_MINUS + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    E_DataType retDt = leftVal.type;
    DataValue substractRet = leftVal - rightVal;
    traceTmpOpResult(finalExpr, substractRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setBeginPos( left->getBeginPos() );
    ret->setEndPos(   right->getBeginPos() );
    ret->setRealValue( substractRet );

    return ret;
}

TokenBase* TokenMgr::doMultiply(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_MULTIPLY  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    E_DataType retDt = leftVal.type;
    DataValue multiRet = leftVal * rightVal;
    traceTmpOpResult(finalExpr, multiRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( multiRet  );

    return ret;
}

TokenBase* TokenMgr::doDivide(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_DIVIDE  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);


    E_DataType retDt = leftVal.type;

    // calc 
    DataValue divideRet;
    try {
        divideRet  = (leftVal / rightVal);
    } catch ( MyException& e ) {
        // auto detailstr = e.getDetail();
        // if ( detailstr.empty() ) {
        //     e.setDetail( rightContent + " is zero Value ( in Divide ) @"  +  right->getBeginPos().getPos() );
        // } 
        e.setDetail( rightContent + " is zero Value ( in Divide ) @"  +  right->getBeginPos().getPos(0) );
        throw;
    }
    traceTmpOpResult(finalExpr, divideRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( divideRet );

    return ret;
}

TokenBase* TokenMgr::doMod(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_MOD  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_MODULO_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_MODULO_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }


    E_DataType retDt = leftVal.type;

    // calc 
    DataValue modRet;
    try {
        modRet  = (leftVal % rightVal);
    } catch ( MyException& e ) {
        if ( e.getDetail().empty() ) {
            e.setDetail( rightContent + " is zero Value ( in Modulo ) @" + right->getBeginPos().getPos(0) );
        }
        throw;
    }

    traceTmpOpResult(finalExpr, modRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( modRet );

    return ret;
}

TokenBase* TokenMgr::doBitAnd(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_AND  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITAND_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITAND_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    E_DataType retDt = leftVal.type;

    // calc 
    DataValue bitAndRet = (leftVal & rightVal);
    traceTmpOpResult(finalExpr, bitAndRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( bitAndRet );

    return ret;

}

TokenBase* TokenMgr::doBitOr(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_OR  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITOR_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITOR_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }


    E_DataType retDt = leftVal.type;

    // calc 
    DataValue bitOrRet = (leftVal | rightVal);
    traceTmpOpResult(finalExpr, bitOrRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( bitOrRet );

    return ret;
}

TokenBase* TokenMgr::doBitXor(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_XOR  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITXOR_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITXOR_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    

    E_DataType retDt = leftVal.type;

    // calc 
    DataValue bitXOrRet = (leftVal ^ rightVal);
    traceTmpOpResult(finalExpr, bitXOrRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( bitXOrRet );

    return ret;
}

TokenBase* TokenMgr::doBitLeftShift(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_LEFT_SHIFT  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();

    E_DataType leftPromotionDt = operatorPrepairDataTypeConversion2(&leftVal, &rightVal);


    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_LEFTSHIFT_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_LEFTSHIFT_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    tracebitShiftWarning(true, left, right);


    // E_DataType retDt = leftVal->type;

    // calc 
    DataValue bitLeftShiftRet = (leftVal << rightVal);
    bitLeftShiftRet.downerCast( leftPromotionDt );

    traceTmpOpResult(finalExpr, bitLeftShiftRet);

    TokenBase* ret = generateTmpExpression(leftPromotionDt, finalExpr, left, right);
    ret->setRealValue( bitLeftShiftRet );

    return ret;
}

TokenBase* TokenMgr::doBitRightShift(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_RIGHT_SHIFT   + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();

    E_DataType leftPromotionDt = operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_RIGHTSHIFT_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_RIGHTSHIFT_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    tracebitShiftWarning(false, left, right);
    

    // calc 
    DataValue bitRightShiftRet = (leftVal >> rightVal);
    bitRightShiftRet.downerCast( leftPromotionDt );
    traceTmpOpResult(finalExpr, bitRightShiftRet);

    TokenBase* ret = generateTmpExpression(leftPromotionDt, finalExpr, left, right);
    ret->setRealValue( bitRightShiftRet );

    return ret;
}

TokenBase* TokenMgr::doAssignment(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    VaribleInfo* toBeAssignmentVar = nullptr;
    auto content = left->getTokenContent();
    if (    !( left->isVarible() ) 
        ||  ( ( toBeAssignmentVar = VariblePool::getPool()->getVaribleByName(content)) == nullptr )  ) {
        MyException e(E_THROW_VARIBLE_NOT_DEFINED, left->getBeginPos() );
        e.setDetail( content );
        throw e;
    }

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_ASSIGNMENT + rightContent;


    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();

    traceAssignOverFlow(left, right);

    //
    // Core Core Core :
    // Can't use  assignment statement like the following :
    //       Because the left operand and the right operand has different data type
    //
    //    leftVal = rightVal;
    //
    leftVal.doAssignment(rightVal);
    left->setRealValue( leftVal );

    // update varible' value
    toBeAssignmentVar->dataVal.doAssignment( leftVal );
    toBeAssignmentVar->isInitialed = true; // set as initialed

    traceTmpOpResult(finalExpr, leftVal);

    TokenBase* ret = generateTmpExpression( leftVal.type , finalExpr, left, right);
    ret->setRealValue( leftVal );

    return ret;
}


TokenBase* TokenMgr::do_Add_Assignment(TokenBase* left, TokenBase* right)
{
    return doAssignment( left,  doAdd(left, right) );
}

TokenBase* TokenMgr::do_Minus_Assignment(TokenBase* left, TokenBase* right)
{
    return doAssignment( left,  doMinus(left, right) );
}

TokenBase* TokenMgr::do_Multiply_Assignment(TokenBase* left, TokenBase* right)
{
    return doAssignment( left,  doMultiply(left, right) );
}

TokenBase* TokenMgr::do_Divide_Assignment(TokenBase* left, TokenBase* right)
{
    return doAssignment( left,  doDivide(left, right) );
}

TokenBase* TokenMgr::do_Mod_Assignment(TokenBase* left, TokenBase* right)
{
    return doAssignment( left,  doMod(left, right) );
}

TokenBase* TokenMgr::do_Bit_And_Assignment(TokenBase* left, TokenBase* right)
{
    return doAssignment( left,  doBitAnd(left, right) );
}

TokenBase* TokenMgr::do_Bit_Or_Assignment(TokenBase* left, TokenBase* right)
{
    return doAssignment( left,  doBitOr(left, right) );
}

TokenBase* TokenMgr::do_Bit_Xor_Assignment(TokenBase* left, TokenBase* right)
{
    return doAssignment( left,  doBitXor(left, right) );
}

TokenBase* TokenMgr::do_Bit_LeftShift_Assignment(TokenBase* left, TokenBase* right)
{
    return doAssignment( left,  doBitLeftShift(left, right) );
}

TokenBase* TokenMgr::do_Bit_RightShift_Assignment(TokenBase* left, TokenBase* right)
{
    return doAssignment( left,  doBitRightShift(left, right) );
}



//####################################################################################################
//
//      Unary Op
TokenBase* TokenMgr::doPositive(TokenBase* op, TokenBase* right)
{
    using namespace charUtil;

    string rightContent = right->getExpressionContent();
    string finalExpr = SC_OP_POSITIVE_BEGIN + rightContent;

    DataValue rightVal = right->getRealValue();
    E_DataType rightPromotionDt = operatorPrepairDataTypeConversion1(&rightVal);

    // calc 
    DataValue positiveRet = +rightVal;
    traceTmpOpResult(finalExpr, positiveRet);

    TokenBase* ret = generateTmpExpression(rightPromotionDt, finalExpr, op, right);
    if ( right->isFixedLiteral()  ) {
        auto base = right->getFixedLiteralBase();
        ret->setBaseAsFixedLiteral( base );
    }
    ret->setRealValue( positiveRet );

    return ret;
}

TokenBase* TokenMgr::doNegative(TokenBase* op, TokenBase* right)
{
    using namespace charUtil;

    string rightContent = right->getExpressionContent();
    string finalExpr = SC_OP_NEGATIVE_BEGIN + rightContent;

    DataValue rightVal = right->getRealValue(); 
    traceNegativeOperation(right);

    E_DataType rightPromotionDt = operatorPrepairDataTypeConversion1(&rightVal);

    // calc 
    DataValue negativeRet = -(rightVal);
    traceTmpOpResult(finalExpr, negativeRet);

    TokenBase* ret = generateTmpExpression(rightPromotionDt, finalExpr, op, right);
    if ( right->isFixedLiteral()  ) {
        auto base = right->getFixedLiteralBase();
        ret->setBaseAsFixedLiteral( base );
    }
    ret->setRealValue( negativeRet  );

    return ret;

}

TokenBase* TokenMgr::doBitNot(TokenBase* op, TokenBase* right)
{
    using namespace charUtil;

    string rightContent = right->getExpressionContent();
    string finalExpr = SC_OP_BIT_NOT_BEGIN + rightContent;

    DataValue rightVal = right->getRealValue();
    E_DataType rightPromotionDt = operatorPrepairDataTypeConversion1(&rightVal);

    if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITNOT_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    } 


    // calc 
    DataValue bitNotRet   =    ~rightVal;
    traceTmpOpResult(finalExpr, bitNotRet);

    TokenBase* ret = generateTmpExpression(rightPromotionDt, finalExpr, op, right);
    ret->setRealValue( bitNotRet );

    return ret;
}


// static 
TokenBase* TokenMgr::generateTmpExpression(E_DataType dt, const string& expression, TokenBase* begtoken, TokenBase* endtoken)
{
    TokenBase* pTmpExpression = new TokenBase(dt);
    pTmpExpression->setAsTmpExpression();
    pTmpExpression->setTokenContent( expression );
    pTmpExpression->setBeginPos( begtoken->getBeginPos() );
    pTmpExpression->setEndPos( endtoken->getEndPos() );

    s_generatedTmpTokenPool.push_back( pTmpExpression );
    return pTmpExpression;
}


// static 
void TokenMgr::clearTmpGenTokenPool()
{
    for( auto it = s_generatedTmpTokenPool.begin(); it != s_generatedTmpTokenPool.end(); ++it )
    {
        if ( *it != nullptr ) {
            delete *it;
            *it = nullptr;
        }
    }
    s_generatedTmpTokenPool.clear();
}


E_DataType TokenMgr::operatorPrepairDataTypeConversion1(DataValue* pRightVal)
{
    auto retType = pRightVal->type;
    auto rightpr = DataTypeUtil::needDoIntegerPromotion( pRightVal->type );
    if ( rightpr.first ) {
        pRightVal->doIntergerPromotion( rightpr.second );
        // set new data Type after finishing interger Promotion 
        retType = rightpr.second;
    } 

    return retType;

}


E_DataType TokenMgr::operatorPrepairDataTypeConversion2(DataValue* pLeftVal, DataValue* pRightVal)
{
    auto setFlag = false;
    auto retType = pLeftVal->type;
    auto leftpr = DataTypeUtil::needDoIntegerPromotion( pLeftVal->type );
    if ( leftpr.first ) {
        pLeftVal->doIntergerPromotion( leftpr.second );
        // set new data Type after finishing interger Promotion 
        setFlag = true;
        retType = leftpr.second;
    }

    auto rightpr = DataTypeUtil::needDoIntegerPromotion( pRightVal->type );
    if ( rightpr.first ) {
        pRightVal->doIntergerPromotion( rightpr.second );
    }

    auto cvtPrpr = DataTypeUtil::needDoConvertion( pLeftVal->type, pRightVal->type );
    auto retDt = cvtPrpr.first.second;
    if ( cvtPrpr.first.first ) {
        pLeftVal->doConvertion( retDt );
    }

    if ( cvtPrpr.second.first ) {
        pRightVal->doConvertion( retDt );
    }

    if ( setFlag ) {
        retType = retDt;
    }

    return retType;

}

void TokenMgr::printOperatorStack()
{
    if ( CmdOptions::needPrintOperatorStackAll()  ) {
        if ( m_opertorStack.empty() ) {
            cerr << m_execodeIdx << ". Operator Stack  : <Empty> " << endl;
        } else {
            cerr << m_execodeIdx <<". Operator Stack  : " <<  m_opertorStack.size() << " Element(s)" << endl;

            auto idx = 0;
            auto it = m_opertorStack.begin();
            while ( it != m_opertorStack.end() ) {
                auto pToken = *it;
                auto content = pToken->getTokenContent();
                cerr << idx << ". " << content << endl;

                ++it;
                ++idx;
            }
            cerr << endl;
        }
    }
}




void TokenMgr::printSuffixExpression(int tag)
{
    auto printFlag = false;
    if ( tag == 1 ) {
        printFlag = CmdOptions::needPrintSuffixExpressionBefore();
    } else if ( tag == 2 )  {
        printFlag = CmdOptions::needPrintSuffixExpressionAfterBuild();
    } else if ( tag == 3 ) {
        printFlag = CmdOptions::needPrintSuffixExpressionAfterEvaluate();
    }

    if ( printFlag ) {
        if ( m_suffixExpression.empty() ) {
            cerr << m_execodeIdx << "-" << tag << " . Suffix Expression List  : <Empty> " << endl;
        } else {
            cerr << m_execodeIdx << "-" << tag << ". Suffix Expression List  : " <<  m_suffixExpression.size() << " Element(s)" << endl;

            auto idx = 0;
            auto it = m_suffixExpression.begin();
            while ( it != m_suffixExpression.end() ) {
                auto pToken = *it;
                auto content = pToken->getTokenContent();
                cerr << idx << ". " << content << endl;

                ++it;
                ++idx;
            }
            cerr << endl;
        }

    }


}



void TokenMgr::popAllOperatorStack()
{
    list<TokenBase*> popedTokenList;

    for( auto rit = m_opertorStack.rbegin(); rit != m_opertorStack.rend(); ++rit )
    {
        auto pToken = *rit;
        // traceOperatorStack(pToken, false);
        m_suffixExpression.push_back( pToken );
        // traceSuffixExpression(pToken, true);
        popedTokenList.push_back( pToken );
    }
    m_opertorStack.clear();
    traceSomeTokensFromOpMove2SuffixExpression( popedTokenList , false);
}




void TokenMgr::traceOperatorStack(TokenBase* pToken, bool push)
{
    using namespace charUtil;

    if ( CmdOptions::needTraceOperatorStackSuffixExpressionChange()  ) {
        if ( push ) {
            cerr << "OpStack->Push " << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << endl;
        } else {
            cerr << "OpStack->Pop "  << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << endl;
        }

    }

}

void TokenMgr::traceSuffixExpression(TokenBase* pToken, bool push)
{
    using namespace charUtil;

    if ( CmdOptions::needTraceOperatorStackSuffixExpressionChange()  ) {
        if ( push ) {
            cerr << "SuffixExpr->Push " << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << endl;
        } else {
            cerr << "SuffixExpr->Pop "  << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << endl;
        }
    }
}

// move from OperatorStack to  -->   SuffixExpression
// pop from OpStack , push it into SuffixExpression list
void TokenMgr::traceOpMove2SuffixExpression(TokenBase* pToken)
{
    using namespace charUtil;

    if ( CmdOptions::needTraceOperatorStackSuffixExpressionChange()  ) {
        auto tokenContent = pToken->getTokenContent();
        cerr << "OpStack->Pop "        <<  SINGLE_QUOTO << tokenContent << SINGLE_QUOTO << " , "
             << "SuffixExpr->Push " <<  SINGLE_QUOTO << tokenContent << SINGLE_QUOTO 
             << endl;
    }
}



void TokenMgr::traceSomeTokensFromOpMove2SuffixExpression(const list<TokenBase*>& lst, bool specialFlag)
{
    if ( CmdOptions::needTraceOperatorStackSuffixExpressionChange()  ) {
        using namespace charUtil;
        if ( lst.empty() ) { return; }

        int cnt     = 0;
        string strOpList;
        string strSuffixList;
        auto it = lst.begin();

        int sz = static_cast<int>( lst.size() );
        while ( true )
        {
            auto tokenContent = (*it)->getTokenContent();
            string strElement = (SINGLE_QUOTO + tokenContent + SINGLE_QUOTO);
            strOpList += strElement;
            if ( specialFlag ) {
                if ( (*it)->getOperatorType() != E_OPEN_PARENTHESIS ) {
                    strSuffixList += strElement;
                }
            } else {
                strSuffixList += strElement;
            }
            ++it;
            ++cnt;

            if ( it == lst.end() ) {
                break;
            } else {
                // add op comma to opStr
                strOpList += ", ";
                // add comma to suffix list
                if ( specialFlag )  {
                    if ( cnt < (sz-1) ) {
                        strSuffixList += ", ";
                    }
                } else {
                    strSuffixList += ", ";
                }
            }
        }

        cerr << "OpStack->Pop  "    << cnt << (cnt == 1 ? " element : " : " elements : ") << strOpList << endl;
        if ( specialFlag ) { 
            --cnt; 
        }
        cerr << "SuffixExpr->Push " << cnt << (cnt == 1 ? " element : " : " elements : ") << strSuffixList << endl;
    }
}



void TokenMgr::tracePositiveNegativeFlag(TokenBase* pToken, E_OperatorType op)
{
    if ( CmdOptions::needTracePositiveNegativePropertyChange() ) {
        auto isAdd = (op == E_ADD);
        if ( pToken == nullptr ) {
            cerr << "chOp : nullptr , so " << (isAdd ? " add -> positive" : " minus -> negative ") << endl;
        } else {
            auto content = pToken->getTokenContent();
            cerr << "ChOp :  \"" << content << "\" , so  " << (isAdd ? " add -> positive" : " minus -> negative ") << endl;
        }
    }

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


void TokenMgr::tracebitShiftWarning(bool isLeftBitShift, TokenBase* left,  TokenBase* right)
{
    using namespace charUtil;

    // Core Core Core : config this flag
    static const bool SC_B_SHOULD_ALWAYS_CHECK_RIGHT_VALUE = true;

    if (  CmdOptions::needPrintParseRuntimeWarning()  ) {
        auto hasprintFlag = false;
        auto hasLeftWarningFlag = false;
        auto hasRightWarningFlag = false;

        TypeBaseInfo leftTpInfo(  left->getRealValue().type );
        TypeBaseInfo rightTpInfo( right->getRealValue().type );
        DataValue rightVal = right->getRealValue();

        auto leftusFlag = leftTpInfo.isUnsignedType();
        if ( CmdOptions::needTreatSignedIntergerBitShiftAsWarning() &&  left->isVarible() &&  !leftusFlag ) {
            cerr << SC_WARNING_TITLE;
            cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
            cerr << "leftOperand is not unsigned type : "  << EnumUtil::enumName(leftTpInfo.getType()) << " : " << left->getTokenContent() << SPACE_1 << "@" << left->getBeginPos().getPos(0) << endl;
            hasprintFlag = true;
            hasLeftWarningFlag = true;
        }

        auto checkRightFlag =  SC_B_SHOULD_ALWAYS_CHECK_RIGHT_VALUE || right->isVarible();

        if ( !checkRightFlag )  {
            return;
        }

        int leftallbits = leftTpInfo.getBits();
        auto rightusFlag = rightTpInfo.isUnsignedType();
        if ( !rightusFlag ) {
            // with  +/-
            if ( rightVal.isNegative() ) {
                if ( !hasprintFlag ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
                    hasprintFlag = true;
                }

                cerr << "rightOperand's value <= 0 , type = "  << EnumUtil::enumName(rightTpInfo.getType()) << " : " << right->getTokenContent() << " = " << rightVal.getPrintValue(0) << endl;
                hasRightWarningFlag = true;
            } else {
                // >=0
                if ( rightVal.isGreaterEqualBitsWidth(leftallbits) ) {
                    if ( !hasprintFlag ) {
                        cerr << SC_WARNING_TITLE;
                        cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
                        hasprintFlag = true;
                    }

                    cerr << "rightOperand's value >= "  << leftallbits << " (Bits-Width) , " << right->getTokenContent() << " = " << rightVal.getPrintValue(0) << endl;
                    hasRightWarningFlag = true;
                }
            }
        } else {
            // >=0
            if ( rightVal.isGreaterEqualBitsWidth(leftallbits) ) {
                if ( !hasprintFlag ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
                    hasprintFlag = true;
                }

                cerr << "rightOperand's value > "  << leftallbits << " (Bits-Width) , " << right->getTokenContent() << " = " << rightVal.getPrintValue(0) << endl;
                hasRightWarningFlag = true;
            }
        }


        if ( hasLeftWarningFlag || hasRightWarningFlag ) {
            cerr << endl;
        }
    }
}


void TokenMgr::traceNegativeOperation(TokenBase* right)
{
    using namespace charUtil;

    if ( CmdOptions::needPrintParseRuntimeWarning()  ) {
        if ( right->isVarible()  ) {
            DataValue rightVal = right->getRealValue();

            TypeBaseInfo tpInfo( rightVal.type );
            if ( tpInfo.isIntegerFamily() ) {

                if ( tpInfo.isUnsignedType() ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " operator - (negative) on a unsigned varible \""<< right->getTokenContent() << "\"  doesn't take effect on a number whose value is always >= 0 " << endl;
                } else if ( rightVal.isMinimumNegativeNumber() ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " operator - (negative) on signed varible \""<< right->getTokenContent() << "\"  doesn't take effect on a number ( minimum negative number ) to making a result as a +number > 0" << endl;
                }
            }
        }
    }

}


void TokenMgr::traceTmpOpResult(const std::string& expr, DataValue& retValue)
{
    if ( CmdOptions::needTraceTmpExpressionProcess() ) {
        TypeBaseInfo retTpInfo(retValue.type);
        cerr << "[INFO] : " << expr << " => " << EnumUtil::enumName( retTpInfo.getType() ) << " , value = " << retValue.getPrintValue(0) << endl;
    }
}





void TokenMgr::traceUnInitializedVarWhenUsed(TokenBase* pToken)
{

    using namespace charUtil;

    if ( CmdOptions::needPrintParseRuntimeWarning() ) {
        cerr << SC_WARNING_TITLE;
        cerr << " varible named " << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << " is not initialized before used " << endl;
    }

}



void TokenMgr::traceBlankStatement()
{
    using namespace charUtil;

    if ( CmdOptions::needPrintParseRuntimeWarning() &&  CmdOptions::needTreatBlankStatementAsWarning()  ) {
        cerr << SC_WARNING_TITLE;
        cerr << " Blank Statement is skipped ! . such as    ; ; " << endl;
    }

}



void TokenMgr::traceAssignOverFlow(TokenBase* leftVar, TokenBase* rightFixedInt)
{
    using namespace charUtil;

    if ( CmdOptions::needCheckFixedLiteralIntRangeWhenAssign()   &&   rightFixedInt->isFixedLiteral() ) {
        TypeBaseInfo  leftVarTpInfo(  leftVar->getDataType() );
        TypeBaseInfo rightFixTpInfo( rightFixedInt->getDataType() );
        if (          leftVarTpInfo.isIntegerFamily()
              &&     rightFixTpInfo.isIntegerFamily()  ) 
        {
            if ( rightFixTpInfo.getLevel() > leftVarTpInfo.getLevel() ) {
                DataValue rVal = rightFixedInt->getRealValue();
                string strMinVal("???_MinVal");
                string strMaxVal("???_MaxVal");
                if ( rVal.isIntOutOfRange( leftVar->getDataType(), strMinVal, strMaxVal ) ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " Sentence : " << SINGLE_QUOTO << EnumUtil::enumName(leftVar->getDataType()) << SPACE_1 << leftVar->getTokenContent() << " = "
                         << rightFixedInt->getTokenContent() << SINGLE_QUOTO << " is out of range ( " << strMinVal << " ~ " << strMaxVal << " ) " 
                         << "@line : " << rightFixedInt->getBeginPos().line
                         << endl;
                }
            }
        }
        
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
        if ( toBePushed->isKeyword() ) {
			if ( priorToken->getOperatorType() == E_OPEN_PARENTHESIS ) {
				// TODO , if support type force convertion ?
				// e.g.      (int)3.14f;
				return false;
			} else {
				return false;
			}
        } else if ( priorToken->getOperatorType() == E_CLOSE_PARENTHESIS ) {
            return false;
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
            return false;
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


/*
void TokenMgr::processPreviousFunctionRelated_IfNecessary(TokenBase* pFuncObj)
{
    if ( iFlag == 1 ) {
        // Function Setting
        if ( pFuncObj != nullptr ) {
            pFuncObj->setAsFunction();

            // m_callstackFuncList.push_back( pFuncObj );
            // allocNewArgumentExpr();
        }
    } 
}
*/

void TokenMgr::allocNewArgumentExpr()
{
    auto newArguments_OnePart = new std::list<TokenBase*>();
    m_oneArgument4ACertainFunc.push_back( newArguments_OnePart );
}


// Core : Key Logic 
void TokenMgr::processParenthesisOrCommaFlag_IfNecessary(TokenBase* pTokenToBePushed, int tokenIndex,int iFlag)
{
    if ( pTokenToBePushed!=nullptr  &&  pTokenToBePushed->getTokenType() == E_TOKEN_OPERATOR ) {
        auto opType = pTokenToBePushed->getOperatorType();
        if ( opType == E_OPEN_PARENTHESIS ) {
            if ( iFlag == 1 ) {
                pTokenToBePushed->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_FUNCTION_START );
            } else {
                pTokenToBePushed->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE );
            }
            m_openParenthesisList.push_back( make_pair(pTokenToBePushed,tokenIndex) );
        } else if ( opType == E_CLOSE_PARENTHESIS ) {
            if ( m_openParenthesisList.empty() ) {
                MyException e(E_THROW_NO_MATCHED_OPEN_PARENTHESIS, pTokenToBePushed->getBeginPos() );
                throw e;
            } else {
                // "()" list is not empty
                auto tailopen = m_openParenthesisList.back().first;
                auto openParenthesisFlag = tailopen->getContextRoleForOp();
                if ( openParenthesisFlag == E_OP_FLAG_OPEN_PARENTHESIS_FUNCTION_START ) {
                    // function end ,check call stack , and pop function object and matched '('
                    if ( m_callstackFuncList.empty() ) {
                        // TODO , logic error , throw exception
                    } else {
                        m_callstackFuncList.pop_back();
                        m_openParenthesisList.pop_back(); // pop the matched  '('
                        // TODO
                        // function argument is end
                        //  m_oneArgument4ACertainFunc ->   copy to   argument list
                    }
                } else {
                    //  openParenthesisFlag == E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE
                    m_openParenthesisList.pop_back();
                }
            }
        } else if ( opType == E_COMMA ) {
            if ( m_openParenthesisList.empty() ) {
                if ( !m_oneSentence.empty() ) {
                    auto firstToken = m_oneSentence.front();
                    if ( firstToken!=nullptr ) {
                        if ( firstToken->isKeyword() ) {
                            pTokenToBePushed->setContextRoleForOp( E_OP_COMMA_DEFINATION_SEP );
                        } else {
                            pTokenToBePushed->setContextRoleForOp( E_OP_COMMA_NORMAL_FOR_COMMA_EXPRESSION );
                        }
                    }
                } 
                /* 
                else {
                    it will throw exception after checkAdjacentTokensRelationship_IsValid() 
                } 
                */
            } else {
                auto lastOpenParenthesis = m_openParenthesisList.back().first;
                auto theRole = lastOpenParenthesis->getContextRoleForOp();
                if ( theRole == E_OpAnotherRoleFlag::E_OP_FLAG_OPEN_PARENTHESIS_FUNCTION_START ) {
                    pTokenToBePushed->setContextRoleForOp( E_OP_COMMA_FUNCTION_ARG_SEP );
                } else {
                    pTokenToBePushed->setContextRoleForOp( E_OP_COMMA_NORMAL_FOR_COMMA_EXPRESSION );
                }
            }

        }
    }
}


void TokenMgr::clearOneArgumentsPool()
{
    for ( auto it = m_oneArgument4ACertainFunc.begin(); it !=  m_oneArgument4ACertainFunc.end(); ++it ) {
        if ( *it != nullptr ) {
            (*it)->clear();
            delete (*it);
            *it = nullptr;
        }
    }
    m_oneArgument4ACertainFunc.clear();
}  

