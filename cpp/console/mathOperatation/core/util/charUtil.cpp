#include "charUtil.h"
using namespace std;

//
// static member init
//
const unsigned int OperatorBaseInfo::S_UNARY_OP_FLAG   = (1U << 6U);
const unsigned int OperatorBaseInfo::S_BINARY_OP_FLAG  = (2U << 6U);
const unsigned int OperatorBaseInfo::S_Left2Right_FLAG = (1U << 8U);
const unsigned int OperatorBaseInfo::S_Right2Left_FLAG = (2U << 8U);


////////////////////////////////////////////////////////////////////////////////////////////////////
//
OperatorBaseInfo::OperatorBaseInfo(char ch, unsigned int meta)
	: m_ch(ch)
	, m_metaInfo(meta)
{

}


char OperatorBaseInfo::getChar()
{
	return m_ch;
}


unsigned int OperatorBaseInfo::getPriority()
{
	//                    11 1111 =>  3F
	return (m_metaInfo & 0x3FU);
}

bool OperatorBaseInfo::isBinaryOp()
{
	//  xx123456
	return ( ( (m_metaInfo >> 6) & 0x3U) == 0x2U);
}

bool OperatorBaseInfo::isLeft2Right()
{
	//  xx12345678
	return ( ( (m_metaInfo >> 8) & 0x3U) == 0x1U);
}

//
////////////////////////////////////////////////////////////////////////////////////////////////////












////////////////////////////////////////////////////////////////////////////////////////////////////
//
CharBaseInfo::CharBaseInfo(char ch,OperatorBaseInfo* pOpInfo)
	: m_ch(ch)
	, m_category(E_CHTYPE_OTHER)
	, m_pOpInfo(pOpInfo)
{
	
	if ( isAlpha() ) {
		m_category = E_CHTYPE_ALPHA;
	} else if ( isNumber() ) {
		m_category = E_CHTYPE_NUMBER;
	} else if ( isOpType() ) {
		m_category = E_CHTYPE_OPERATOR;
	} else if ( isBlank() ) {
		m_category = E_CHTYPE_BLANK;
	} else {
		m_category = E_CHTYPE_OTHER;
	}
}


// virtual 
CharBaseInfo::~CharBaseInfo()
{
	if ( m_pOpInfo != nullptr ) {
		delete m_pOpInfo;
		m_pOpInfo = nullptr;
	}
}

char CharBaseInfo::getCh()
{
	return m_ch;
}

E_CharType CharBaseInfo::getCategory()
{
	return m_category;
}

bool CharBaseInfo::isAlpha()
{
	return (m_ch >= 'a' && m_ch <= 'z') || (m_ch >= 'A' && m_ch <= 'Z');
}

bool CharBaseInfo::isNumber()
{
	return (m_ch >= '0' && m_ch <= '9');
}

bool CharBaseInfo::isOctNumber()
{
	return (m_ch >= '0' && m_ch <= '7');
}

bool CharBaseInfo::isHexNumber()
{
	return    (m_ch >= '0' && m_ch <= '9')
	       || (m_ch >= 'a' && m_ch <= 'f')
		   || (m_ch >= 'A' && m_ch <= 'F');
}

bool CharBaseInfo::isUnderLine()
{
	return m_ch == '_';
}

bool CharBaseInfo::isVaribleCharSet()
{
	return isAlpha() || isNumber() || isUnderLine();
}


bool CharBaseInfo::isVaribleHeadChar()
{
	return isAlpha() || isUnderLine();
}


bool CharBaseInfo::isDot()
{
	return m_ch == '.';
}

bool CharBaseInfo::isOpType()
{
	return m_pOpInfo != nullptr;
}

bool CharBaseInfo::isSemicolon()
{
	return m_ch == ';';
}

bool CharBaseInfo::isBlank()
{
	return    (m_ch == ' ') 
		   || (m_ch == '\t') 
		   || (m_ch == '\r') 
		   || (m_ch == '\n');
}

OperatorBaseInfo* CharBaseInfo::getOpInfo()
{
	return m_pOpInfo;
}

//
////////////////////////////////////////////////////////////////////////////////////////////////////














////////////////////////////////////////////////////////////////////////////////////////////////////
//

// static
unordered_map<char,CharBaseInfo*> CharUtil::s_allCharSetMap;

// static 
CharBaseInfo* CharUtil::s_positiveCharInfo = nullptr;

// static 
CharBaseInfo* CharUtil::s_negativeCharInfo = nullptr;

// static 
void CharUtil::init()
{
	char the_ch = 0;

	// Alpha Part
	for ( char ch = 'a'; ch <= 'z'; ++ch ) {
		s_allCharSetMap.insert( make_pair(ch, new CharBaseInfo(ch, nullptr) ) );
	}

	for ( char ch = 'A'; ch <= 'Z'; ++ch ) {
		s_allCharSetMap.insert( make_pair(ch, new CharBaseInfo(ch, nullptr) ) );
	}

	// Number Part
	for ( char ch = '0'; ch <= '9'; ++ch ) {
		s_allCharSetMap.insert( make_pair(ch, new CharBaseInfo(ch, nullptr) ) );
	}

	/*
	   Operator            Precedence                  Description
	---------------------------------------------------------------------------------------
	    ( )                 2                          Left-to-Right
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
	---------------------------------------------------------------------------------------

	*/

	///////////////////////////////////////////////////////////////////////////////////////
	//
	// Operator Part 
	//
	///////////////////////////////////////////////////////////////////////////////////////
	unsigned int meta = 0U;

	the_ch = '+';           
	meta = OperatorBaseInfo::S_Left2Right_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 6U;
	OperatorBaseInfo* additionInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, additionInfo) ) );

	the_ch = '-';
    meta = OperatorBaseInfo::S_Left2Right_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 6U;
	OperatorBaseInfo* substractionInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, substractionInfo) ) );

	the_ch = '*';
    meta = OperatorBaseInfo::S_Left2Right_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 5U;
	OperatorBaseInfo* multiplyInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, multiplyInfo) ) );

	the_ch = '/';
    meta = OperatorBaseInfo::S_Left2Right_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 5U;
	OperatorBaseInfo* divideInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, divideInfo) ) );

	the_ch = '%';
    meta = OperatorBaseInfo::S_Left2Right_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 5U;
	OperatorBaseInfo* modInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, modInfo) ) );

	the_ch = '&';
    meta = OperatorBaseInfo::S_Left2Right_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 11U;
	OperatorBaseInfo* bitAndInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitAndInfo) ) );

	the_ch = '^';
    meta = OperatorBaseInfo::S_Left2Right_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 12U;
	OperatorBaseInfo* bitXOrInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitXOrInfo) ) );

	the_ch = '|';
    meta = OperatorBaseInfo::S_Left2Right_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 13U;
	OperatorBaseInfo* bitOrInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitOrInfo) ) );

	the_ch = '~';
    meta = OperatorBaseInfo::S_Right2Left_FLAG | OperatorBaseInfo::S_UNARY_OP_FLAG | 3U;
	OperatorBaseInfo* bitNotInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitNotInfo) ) );

	the_ch = '<'; // <<
    meta = OperatorBaseInfo::S_Left2Right_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 7U;
	OperatorBaseInfo* bitLeftShiftInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitLeftShiftInfo) ) );

	the_ch = '>'; // >>
    meta = OperatorBaseInfo::S_Left2Right_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 7U;
	OperatorBaseInfo* bitRightShiftInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitRightShiftInfo) ) );

	the_ch = '(';
    meta = 0U | 0U | 2U;
	OperatorBaseInfo* openParentheseInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, openParentheseInfo) ) );

	the_ch = ')';
    meta = 0U | 0U | 2U;
	OperatorBaseInfo* closeParentheseInfo = new OperatorBaseInfo( the_ch, meta);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, closeParentheseInfo) ) );

	the_ch = '=';
    meta = OperatorBaseInfo::S_Right2Left_FLAG | OperatorBaseInfo::S_BINARY_OP_FLAG | 16U;
	OperatorBaseInfo* equalInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, equalInfo) ) );

	// +a
	meta = OperatorBaseInfo::S_Right2Left_FLAG | OperatorBaseInfo::S_UNARY_OP_FLAG | 3U;
	OperatorBaseInfo* positiveOpInfo = new OperatorBaseInfo('+' , 0);
	CharUtil::s_positiveCharInfo = new CharBaseInfo('+', positiveOpInfo);

	// -a
	meta = OperatorBaseInfo::S_Right2Left_FLAG | OperatorBaseInfo::S_UNARY_OP_FLAG | 3U;
	OperatorBaseInfo* negativeOpInfo = new OperatorBaseInfo('-', meta);
	CharUtil::s_negativeCharInfo = new CharBaseInfo('-', negativeOpInfo);

	///////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////
	the_ch = '_';
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, nullptr) ) );

	the_ch = '.';
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, nullptr) ) );

	the_ch = ';';
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, nullptr) ) );

	//
	// Blank Part
	//
	the_ch = ' ';
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, nullptr) ) );

	the_ch = '\t';
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, nullptr) ) );

	the_ch = '\r';
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, nullptr) ) );

	the_ch = '\n';
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, nullptr) ) );


}

// static 
void CharUtil::finalize()
{
	for( auto it = s_allCharSetMap.begin(); it != s_allCharSetMap.end(); ++it )
	{
		if ( it->second != nullptr ) {
			delete it->second;
			it->second = nullptr;
		}
	}

	s_allCharSetMap.clear();

	if ( CharUtil::s_positiveCharInfo != nullptr ) {
		delete  CharUtil::s_positiveCharInfo;
		CharUtil::s_positiveCharInfo = nullptr;
	}

	if ( CharUtil::s_negativeCharInfo != nullptr ) {
		delete  CharUtil::s_negativeCharInfo;
		CharUtil::s_negativeCharInfo = nullptr;
	}
}

// static 
std::unordered_map<char,CharBaseInfo*>* CharUtil::getAllCharSet()
{
	return &s_allCharSetMap;
}

	
CharBaseInfo* CharUtil::getCharBaseInfo(char ch)
{
	auto it =  s_allCharSetMap.find(ch);
	if ( it != s_allCharSetMap.end() ) {
		return it->second;
	} else {
		return nullptr;
	}
}


// static 
CharBaseInfo* CharUtil::getPositiveCharInfo() // +a
{
	return s_positiveCharInfo;
}

// static 
CharBaseInfo* CharUtil::getNegativeCharInfo() // -a
{
	return s_negativeCharInfo;
}


//
////////////////////////////////////////////////////////////////////////////////////////////////////
