#ifndef TYPE_UTIL_H
#define TYPE_UTIL_H


#include <cstdlib>
#include <unordered_map>
#include "commonEnum.h"



class TypeBaseInfo
{
public:
	TypeBaseInfo(E_DataType dtype);

	size_t getMemorySize(); // in 'byte' unit
	unsigned int getLevel();
	E_DataType getType();

	// util function
	bool isFloatFamily();

    // char /short
	bool isSmallIntegerType();  // char /short
	bool isIntegerFamily();     // char ~ long long
protected:
	unsigned int m_level;
	size_t   m_bytes;
	E_DataType m_type;
};



class TypeUtil
{
public:
	static void init();
	static void finalize();
	static TypeBaseInfo* getTypeInfo(E_DataType dtype);
	E_DataType doIntegerPromotion(E_DataType dtype, bool& changedFlag);
protected:

#ifdef USE_INT_INSTEAD_OF_ENUM
	static std::unordered_map<int, TypeBaseInfo*> s_typeFamily;
#else
	static std::unordered_map<E_DataType, TypeBaseInfo*> s_typeFamily;
#endif
};


#endif 
