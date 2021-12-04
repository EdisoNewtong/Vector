#include "typeUtil.h"

using namespace std;

TypeBaseInfo::TypeBaseInfo(E_DataType dtype)
	: m_level(0)
	, m_bytes(0)
	, m_type(dtype)
{
	/*
		Besides the minimal bit counts, the C++ Standard guarantees that
	    1 == sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long) <= sizeof(long long).
	*/

	if (        m_type == E_TP_U_CHAR       || m_type == E_TP_S_CHAR )       {
		m_level = 1;
		m_bytes = sizeof(char);
	} else if ( m_type == E_TP_U_SHORT      || m_type == E_TP_S_SHORT )      {
		m_level = 2;
		m_bytes = sizeof(short);
	} else if ( m_type == E_TP_U_INT        || m_type == E_TP_S_INT )        {
		m_level = 3;
		m_bytes = sizeof(int);
	} else if ( m_type == E_TP_U_LONG       || m_type == E_TP_S_LONG )       {
		m_level = 4;
		m_bytes = sizeof(long);
	} else if ( m_type == E_TP_U_LONG_LONG  || m_type == E_TP_S_LONG_LONG )  {
		m_level = 5;
		m_bytes = sizeof(long long);
	} else if ( m_type == E_TP_FLOAT )                                       {
		m_level = 6;
		m_bytes = sizeof(float);
	} else if ( m_type == E_TP_DOUBLE )                                      {
		m_level = 7;
		m_bytes = sizeof(double);
	} else if ( m_type == E_TP_LONG_DOUBLE )                                 {
		m_level = 8;
		m_bytes = sizeof(long double);
	}
}

//
// in 'byte' unit
//
size_t TypeBaseInfo::getMemorySize()
{
	return m_bytes;
}

unsigned int TypeBaseInfo::getLevel()
{
	return m_level;
}

E_DataType TypeBaseInfo::getType()
{
	return m_type;
}



bool TypeBaseInfo::isFloatFamily()
{
	return m_type == E_TP_FLOAT || m_type == E_TP_DOUBLE ||  m_type == E_TP_LONG_DOUBLE;
}

bool TypeBaseInfo::isIntegerFamily()
{
	return m_type >= E_TP_U_CHAR && m_type <= E_TP_S_LONG_LONG;
}

bool TypeBaseInfo::isSmallIntegerType()
{
	return m_type >= E_TP_U_CHAR && m_type <= E_TP_S_SHORT;
}









//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


#ifdef USE_INT_INSTEAD_OF_ENUM
    // static 
    std::unordered_map<int, TypeBaseInfo*> TypeUtil::s_typeFamily;
#else
    // static 
    std::unordered_map<E_DataType, TypeBaseInfo*> TypeUtil::s_typeFamily;
#endif


// static
void TypeUtil::init()
{
	s_typeFamily.clear();
	s_typeFamily.insert( make_pair(enumCvt(E_TP_U_CHAR), new TypeBaseInfo(E_TP_U_CHAR) ) );
	s_typeFamily.insert( make_pair(enumCvt(E_TP_S_CHAR), new TypeBaseInfo(E_TP_S_CHAR) ) );

	s_typeFamily.insert( make_pair(enumCvt(E_TP_U_SHORT), new TypeBaseInfo(E_TP_U_SHORT) ) );
	s_typeFamily.insert( make_pair(enumCvt(E_TP_S_SHORT), new TypeBaseInfo(E_TP_S_SHORT) ) );

	s_typeFamily.insert( make_pair(enumCvt(E_TP_U_INT), new TypeBaseInfo(E_TP_U_INT) ) );
	s_typeFamily.insert( make_pair(enumCvt(E_TP_S_INT), new TypeBaseInfo(E_TP_S_INT) ) );

	s_typeFamily.insert( make_pair(enumCvt(E_TP_U_LONG), new TypeBaseInfo(E_TP_U_LONG) ) );
	s_typeFamily.insert( make_pair(enumCvt(E_TP_S_LONG), new TypeBaseInfo(E_TP_S_LONG) ) );

	s_typeFamily.insert( make_pair(enumCvt(E_TP_U_LONG_LONG), new TypeBaseInfo(E_TP_U_LONG_LONG) ) );
	s_typeFamily.insert( make_pair(enumCvt(E_TP_S_LONG_LONG), new TypeBaseInfo(E_TP_S_LONG_LONG) ) );

	s_typeFamily.insert( make_pair(enumCvt(E_TP_FLOAT), new TypeBaseInfo(E_TP_FLOAT) ) );
	s_typeFamily.insert( make_pair(enumCvt(E_TP_DOUBLE), new TypeBaseInfo(E_TP_DOUBLE) ) );
	s_typeFamily.insert( make_pair(enumCvt(E_TP_LONG_DOUBLE), new TypeBaseInfo(E_TP_LONG_DOUBLE) ) );
}


// static 
void TypeUtil::finalize()
{
	for( auto it = s_typeFamily.begin(); it != s_typeFamily.end(); ++it )
	{
		if ( it->second != nullptr ) {
			delete it->second;
			it->second = nullptr;
		}
	}
	s_typeFamily.clear();
}


// static 
TypeBaseInfo* TypeUtil::getTypeInfo(E_DataType dtype)
{

#ifdef USE_INT_INSTEAD_OF_ENUM
	auto it = s_typeFamily.find( enumCvt(dtype) );
#else
	auto it = s_typeFamily.find( dtype );
#endif

	if ( it != s_typeFamily.end() ) {
		return it->second;
	} else {
		return nullptr;
	}
}


E_DataType TypeUtil::doIntegerPromotion(E_DataType oldtp, bool& changedFlag)
{
	auto pBaseInfo = getTypeInfo(oldtp);
	if ( pBaseInfo == nullptr ) {
		changedFlag = false;
		return oldtp;
	}

	// pBaseInfo != nullptr
	if ( pBaseInfo->isSmallIntegerType()  ) {
		auto newtp = oldtp;
		changedFlag = true;
		if ( oldtp == E_TP_S_CHAR || oldtp == E_TP_S_SHORT ) {
			newtp =  E_TP_S_INT; // signed int
		} else {
			// unsigned char /  unsigned short
			newtp = E_TP_S_INT;
		}
		return newtp;
	} else {
		changedFlag = false;
		return oldtp;
	}

}


//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
