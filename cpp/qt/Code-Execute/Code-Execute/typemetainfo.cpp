#include "typemetainfo.h"

numberType::numberType() 
	: m_Type(numberEnumNS::TYPE_ID::E_TYPE_MAX) 
	, m_OccupyMemory(0LL)
{ }


// virtual 
void uCharType::init() // override;
{
	m_Type = numberEnumNS::TYPE_ID::E_UNSIGNED_CHAR;
	m_OccupyMemory = sizeof(unsigned char);
}

// virtual 
void sCharType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_SIGNED_CHAR;
	m_OccupyMemory = sizeof(signed char);
}


// virtual 
void uShortType::init() // override 
{
	m_Type = numberEnumNS::TYPE_ID::E_UNSIGNED_SHORT;
	m_OccupyMemory = sizeof(unsigned short);
}


// virtual 
void sShortType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_SIGNED_SHORT;
	m_OccupyMemory = sizeof(signed short);
}

// virtual 
void uIntType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_UNSIGNED_INT;
	m_OccupyMemory = sizeof(unsigned int);
}


// virtual 
void sIntType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_SIGNED_INT;
	m_OccupyMemory = sizeof(signed int);
}

// virtual 
void uLongType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_UNSIGNED_LONG;
	m_OccupyMemory = sizeof(unsigned long);
}

// virtual 
void sLongType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_SIGNED_LONG;
	m_OccupyMemory = sizeof(signed long);
}


// virtual 
void uLongLongType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_UNSIGNED_LONG_LONG;
	m_OccupyMemory = sizeof(unsigned long long);
}

// virtual 
void sLongLongType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_SIGNED_LONG_LONG;
	m_OccupyMemory = sizeof(signed long long);
}


// virtual 
void floatType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_FLOAT;
	m_OccupyMemory = sizeof(float);
}

// virtual 
void doubleType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_DOUBLE;
	m_OccupyMemory = sizeof(double);
}

// virtual 
void longDoubleType::init() // override
{
	m_Type = numberEnumNS::TYPE_ID::E_LONG_DOUBLE;
	m_OccupyMemory = sizeof(long double);
}




numberTypeFactory::numberTypeFactory()
	: m_allTypeCollection{}
{
	m_allTypeCollection.clear();

	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_UNSIGNED_CHAR,      new uCharType)      );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_SIGNED_CHAR,        new sCharType)      );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_UNSIGNED_SHORT,     new uShortType)     );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_SIGNED_SHORT,       new sShortType)     );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_UNSIGNED_INT,       new uIntType)       );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_SIGNED_INT,         new sIntType)       );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_UNSIGNED_LONG,      new uLongType)      );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_SIGNED_LONG,        new sLongType)      );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_UNSIGNED_LONG_LONG, new uLongLongType)  );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_SIGNED_LONG_LONG,   new sLongLongType)  );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_FLOAT,              new floatType)      );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_DOUBLE,             new doubleType)     );
	m_allTypeCollection.insert( std::make_pair(numberEnumNS::TYPE_ID::E_LONG_DOUBLE,        new longDoubleType) );
	
	for ( auto pType : m_allTypeCollection ) {
        pType.second->init();
	}
}

// virtual
numberTypeFactory::~numberTypeFactory()
{
	for ( auto& pEle : m_allTypeCollection )
	{
        delete pEle.second;
        pEle.second = nullptr;
	}
	m_allTypeCollection.clear();
}

const numberType* numberTypeFactory::getMetaInfo(numberEnumNS::TYPE_ID typeId) const
{
	auto foundIt =  m_allTypeCollection.find(typeId);
	if ( foundIt == m_allTypeCollection.end() ) {
		return nullptr;
	} 
	
	return foundIt->second;
}

