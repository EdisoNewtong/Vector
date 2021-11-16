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
	bool isIntegerFamily();

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
protected:
	static std::unordered_map<E_DataType, TypeBaseInfo*> s_typeFamily;
};


#endif 
