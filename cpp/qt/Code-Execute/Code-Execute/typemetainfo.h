#ifndef TYPE_META_INFO_H
#define TYPE_META_INFO_H

#include <unordered_map>

/*


//////////////////////////////////////////////////
// Positive/Negative Operator
//////////////////////////////////////////////////
   +/-   1 operator num



//////////////////////////////////////////////////
// Mathmatic Operation
//////////////////////////////////////////////////
   +     +=   // ++   ++(int)
   -     -=   // --   --(int)
   *     *=
   /     /=
   %     %=


//////////////////////////////////////////////////
// Bitwise Operation
//////////////////////////////////////////////////
   ~          
   &     &=
   |     |=
   ^     ^=
   <<    <<=
   >>    >>=





*/

namespace numberEnumNS
{
	enum class TYPE_ID : unsigned int
	{
		// char
		E_UNSIGNED_CHAR = 0,
		E_SIGNED_CHAR,   
		// short
		E_UNSIGNED_SHORT,
		E_SIGNED_SHORT,
		// int
		E_UNSIGNED_INT,
		E_SIGNED_INT,

		// long 
		E_UNSIGNED_LONG,
		E_SIGNED_LONG,

		// long long
		E_UNSIGNED_LONG_LONG,
		E_SIGNED_LONG_LONG,

		// float / double     number
		E_FLOAT,
		E_DOUBLE,
		E_LONG_DOUBLE,

		E_TYPE_MAX
	};
} // end  numberEnumNS

class numberType
{
public:
	numberType();
	virtual ~numberType() = default;
	virtual void init() = 0;
	inline numberEnumNS::TYPE_ID getType() { return m_Type; }
public:
	numberEnumNS::TYPE_ID         m_Type;
	size_t                        m_OccupyMemory;
};

class uCharType : public numberType
{
public:
	virtual void init() override;
};

class sCharType : public numberType
{
public:
	virtual void init() override;
};

class uShortType : public numberType
{
public:
	virtual void init() override;
};

class sShortType : public numberType
{
public:
	virtual void init() override;
};

class uIntType : public numberType
{
public:
	virtual void init() override;
};

class sIntType : public numberType
{
public:
	virtual void init() override;
};


class uLongType : public numberType
{
public:
	virtual void init() override;
};

class sLongType : public numberType
{
public:
	virtual void init() override;
};

class uLongLongType : public numberType
{
public:
	virtual void init() override;
};

class sLongLongType : public numberType
{
public:
	virtual void init() override;
};


class floatType : public numberType
{
public:
	virtual void init() override;
};


class doubleType : public numberType
{
public:
	virtual void init() override;
};


class longDoubleType : public numberType
{
public:
	virtual void init() override;
};

class numberTypeFactory
{
public:
	numberTypeFactory();
	virtual ~numberTypeFactory();
	const numberType* getMetaInfo(numberEnumNS::TYPE_ID typeId) const;
protected:
    std::unordered_map<numberEnumNS::TYPE_ID, numberType*>  m_allTypeCollection;
};

#endif
