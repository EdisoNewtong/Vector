#ifndef OP_UTIL_H
#define OP_UTIL_H

#include <unordered_map>
#include "commonEnum.h"

struct OpInfo
{
    OpInfo(unsigned int meta) : m_meta(meta) { }

    //--------------------------------------------------
    //    16 bits is enough
    //--------------------------------------------------
    // |           |    1-8    | 8 bits    : OpType    
    // |      9-14 |           | 6 bits    : Priority    ( whose priority is higher with the lower value )
    // |    15     |           | 1 bit     : Association ( Left2Right : 0 , Right2Left : 1 )
    // | 16        |           | 1 bit     : Op Property ( BinaryOp   : 0 , UnaryOp    : 1 )
    //--------------------------------------------------
    // |           |  Op-Type  |
    // | xxxx xxxx | xxxx xxxx |
    unsigned int m_meta;

    E_OperatorType getOpType();
    unsigned int   getPriority();
    bool           isRight2Left();
    bool           isUnaryOp();

};

class OpUtil
{
public:
    static OpInfo getOpInfo(E_OperatorType dp);
protected:


#ifdef USE_INT_INSTEAD_OF_ENUM
    typedef int OpType;
#else
    typedef E_OperatorType OpType;
#endif


    static std::unordered_map<OpType, OpInfo> s_operatorMap;
    
};


#endif
