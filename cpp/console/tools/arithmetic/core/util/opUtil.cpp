#include "opUtil.h"
#include "myException.h"
using namespace std;


/*

____________________________________________________________________________________________________

C++ Operator Precedence
____________________________________________________________________________________________________

The following table lists the precedence and associativity of C++ operators. Operators are listed top to bottom, in descending precedence.


|------------|-----------------------------------------|--------------------------------------------------------------|--------------------------|
| Precedence | Operator                                |                 Description                                  |  Associativity           |
|------------|-----------------------------------------|--------------------------------------------------------------|--------------------------|
|     1      |  ::                                     |  Scope resolution                                            |  Left-to-right           |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |                        
|     2      |  a++   a--                              |  Suffix/postfix increment and decrement                      |                          | 
|            |  type()                                 |  type{}Functional cast                                       |                          | 
|            |  a()                                    |  Function call                                               |                          | 
|            |  a[]                                    |  Subscript                                                   |                          | 
|            |  .   ->                                 |  Member access                                               |                          | 
|------------|-----------------------------------------|--------------------------------------------------------------|--------------------------|
|     3      |  ++a   --a                              |  Prefix increment and decrement                              |  Right-to-left           |
|            |  +a   -a                                |  Unary plus and minus                                        |                          |
|            |  !   ~                                  |  Logical NOT and bitwise NOT                                 |                          |
|            |  (type)                                 |  C-style cast                                                |                          |
|            |  *a                                     |  Indirection (dereference)                                   |                          |
|            |  &a                                     |  Address-of                                                  |                          |
|            |  sizeof                                 |  Size-of[note 1]                                             |                          |
|            |  co_await                               |  await-expression (C++20)                                    |                          |
|            |  new   new[]                            |  Dynamic memory allocation                                   |                          |
|            |  delete   delete[]                      |  Dynamic memory deallocation                                 |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|--------------------------|
|    4       |   .   *   ->*                           |  Pointer-to-member                                           |  Left-to-right           |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    5       |   a*b   a/b   a%b                       |  Multiplication, division, and remainder                     |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    6       |   a+b   a-b                             |  Addition and subtraction                                    |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    7       |   <<   >>                               |  Bitwise left shift and right shift                          |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    8       |     <=>                                 |  Three-way comparison operator (since C++20)                 |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    9       |   <   <=                                |  For relational operators < and <= respectively              |                          |
|            |   >   >=                                |  For relational operators > and >= respectively              |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    10      |   ==   !=                               |  For relational operators = and != respectively              |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    11      |      &                                  |  Bitwise AND                                                 |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    12      |      ^                                  |  Bitwise XOR (exclusive or)                                  |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    13      |      |                                  |  Bitwise OR (inclusive or)                                   |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    14      |     &&                                  |  Logical AND                                                 |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|                          |
|    15      |     ||                                  |  Logical OR                                                  |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|--------------------------|
|    16      |    a ? b :c                             |  Ternary conditional[note 2]                                 |  Right-to-left           |
|            |    throw                                |  operator                                                    |                          |
|            |    co_yield                             |  yield-expression (C++20)                                    |                          |
|            |       =                                 |  Direct assignment (provided by default for C++ classes)     |                          |
|            |    +=   -=                              |  Compound assignment by sum and difference                   |                          |
|            |    *=   /=   %=                         |  Compound assignment by product, quotient, and remainder     |                          |
|            |    <<=   >>=                            |  Compound assignment by bitwise left shift and right shift   |                          |
|            |    &=   ^=   |=                         |  Compound assignment by bitwise AND, XOR, and OR             |                          |
|------------|-----------------------------------------|--------------------------------------------------------------|--------------------------|
|    17      |     ,                                   |  Comma                                                       |  Left-to-right           |
|------------|-----------------------------------------|--------------------------------------------------------------|--------------------------|

*/



#define BINARY_OP_MASK      (0U << 15)
#define UNARY_OP_MASK       (1U << 15)

#define ASS_LEFT_2_RIGHT    (0U << 14)
#define ASS_RIGHT_2_LEFT    (1U << 14)

#define PRIORITY_MASK(val)  (val << 8U) 


E_OperatorType OpInfo::getOpType()
{
    return static_cast<enum E_OperatorType>( m_meta & 0xFFU );
}

unsigned int   OpInfo::getPriority()
{
    return (m_meta >> 8U) & 0x3FU;
}

bool           OpInfo::isRight2Left()
{
    return ( ( (m_meta >> 14U) & 0x1U) == 0x1U );
}

bool           OpInfo::isUnaryOp()
{
    return ( ( (m_meta >> 15U) & 0x1U) == 0x1U );
}



/*
####################################################################################################

   Operator            Precedence                  Description
---------------------------------------------------------------------------------------
    ( )                 2                          Right-to-Left
---------------------------------------------------------------------------------------
    +a   -a             3                          Right-to-Left
    ~a                  3
---------------------------------------------------------------------------------------
    a*b   a/b   a%b     5                          Left-to-Right
---------------------------------------------------------------------------------------
    a+b   a-b           6                          Left-to-Right
---------------------------------------------------------------------------------------
    a<<b  a>>b          7                          Left-to-Right
---------------------------------------------------------------------------------------
    a&b                 11                         Left-to-Right
---------------------------------------------------------------------------------------
    a^b                 12                         Left-to-Right
---------------------------------------------------------------------------------------
    a|b                 13                         Left-to-Right
---------------------------------------------------------------------------------------
    a=b                 16                         Right-to-Left

####################################################################################################
*/

// static 
unordered_map<OpUtil::OpType, OpInfo> OpUtil::s_operatorMap{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*                                            ( Bin 0/Un:1     |      LR:0 RL:1   |  Priority          |                      Op Type     )     */
    make_pair(enumCvt(E_ADD),               OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(6U)  | static_cast<unsigned int>(E_ADD)               ) ),
     make_pair(enumCvt(E_POSITIVE),         OpInfo(  UNARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(3U)  | static_cast<unsigned int>(E_POSITIVE)          ) ),
    make_pair(enumCvt(E_MINUS),             OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(6U)  | static_cast<unsigned int>(E_MINUS)             ) ),
     make_pair(enumCvt(E_NEGATIVE),         OpInfo(  UNARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(3U)  | static_cast<unsigned int>(E_NEGATIVE)          ) ),
    make_pair(enumCvt(E_MULTIPLY),          OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(5U)  | static_cast<unsigned int>(E_MULTIPLY)          ) ),
    make_pair(enumCvt(E_DIVIDE),            OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(5U)  | static_cast<unsigned int>(E_DIVIDE)            ) ),
    make_pair(enumCvt(E_MOD),               OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(5U)  | static_cast<unsigned int>(E_MOD)            ) ),
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    make_pair(enumCvt(E_BIT_AND),           OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(11U) | static_cast<unsigned int>(E_BIT_AND)           ) ),
    make_pair(enumCvt(E_BIT_OR),            OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(13U) | static_cast<unsigned int>(E_BIT_OR)            ) ),
    make_pair(enumCvt(E_BIT_XOR),           OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(12U) | static_cast<unsigned int>(E_BIT_XOR)           ) ),
    make_pair(enumCvt(E_BIT_NOT),           OpInfo(  UNARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(3U)  | static_cast<unsigned int>(E_BIT_NOT)           ) ),
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    make_pair(enumCvt(E_BIT_LEFT_SHIFT),    OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(7U)  | static_cast<unsigned int>(E_BIT_LEFT_SHIFT)    ) ),
    make_pair(enumCvt(E_BIT_RIGHT_SHIFT),   OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(7U)  | static_cast<unsigned int>(E_BIT_RIGHT_SHIFT)   ) ),
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    make_pair(enumCvt(E_OPEN_PARENTHESIS),  OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(2U)  | static_cast<unsigned int>(E_OPEN_PARENTHESIS)  ) ),
    make_pair(enumCvt(E_CLOSE_PARENTHESIS), OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(2U)  | static_cast<unsigned int>(E_CLOSE_PARENTHESIS) ) ),
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    make_pair(enumCvt(E_ASSIGNMENT),        OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_ASSIGNMENT)        ) ),
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    make_pair(enumCvt(E_ADD_ASSIGNMENT),             OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_ADD_ASSIGNMENT)        ) ),
    make_pair(enumCvt(E_MINUS_ASSIGNMENT),           OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_MINUS_ASSIGNMENT)    ) ),
    make_pair(enumCvt(E_MULTIPLY_ASSIGNMENT),        OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_MULTIPLY_ASSIGNMENT)    ) ),
    make_pair(enumCvt(E_DIVIDE_ASSIGNMENT),          OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_DIVIDE_ASSIGNMENT)    ) ),
    make_pair(enumCvt(E_MOD_ASSIGNMENT),             OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_MOD_ASSIGNMENT)    ) ),
    make_pair(enumCvt(E_BIT_AND_ASSIGNMENT),         OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_BIT_AND_ASSIGNMENT)    ) ),
    make_pair(enumCvt(E_BIT_OR_ASSIGNMENT),          OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_BIT_OR_ASSIGNMENT)    ) ),
    make_pair(enumCvt(E_BIT_XOR_ASSIGNMENT),         OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_BIT_XOR_ASSIGNMENT)    ) ),
    make_pair(enumCvt(E_BIT_LEFT_SHIFT_ASSIGNMENT),  OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_BIT_LEFT_SHIFT_ASSIGNMENT)    ) ),
    make_pair(enumCvt(E_BIT_RIGHT_SHIFT_ASSIGNMENT), OpInfo( BINARY_OP_MASK | ASS_RIGHT_2_LEFT | PRIORITY_MASK(16U) | static_cast<unsigned int>(E_BIT_RIGHT_SHIFT_ASSIGNMENT)    ) ),
	// new feature for function call
    make_pair(enumCvt(E_COMMA), OpInfo( BINARY_OP_MASK | ASS_LEFT_2_RIGHT | PRIORITY_MASK(17U) | static_cast<unsigned int>(E_COMMA)    ) )
};




OpInfo OpUtil::getOpInfo(E_OperatorType dp)
{
    auto it = s_operatorMap.find(dp);
    if ( it == s_operatorMap.end() ) {
        MyException e(E_THROW_INVALID_OPERATOR);
        e.setDetail( string("OperatorType = ") + to_string(static_cast<int>(dp)) );
        throw e;
    } 
    
    return it->second;
}
