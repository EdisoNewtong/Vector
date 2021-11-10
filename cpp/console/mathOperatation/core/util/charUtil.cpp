#include "charUtil.h"
using namespace std;

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

	//
	// Operator Part , TODO
	//
	the_ch = '+';
	OperatorBaseInfo* additionInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, additionInfo) ) );

	the_ch = '-';
	OperatorBaseInfo* substractionInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, substractionInfo) ) );

	the_ch = '*';
	OperatorBaseInfo* multiplyInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, multiplyInfo) ) );

	the_ch = '/';
	OperatorBaseInfo* divideInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, divideInfo) ) );

	the_ch = '%';
	OperatorBaseInfo* modInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, modInfo) ) );

	the_ch = '&';
	OperatorBaseInfo* bitAndInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitAndInfo) ) );

	the_ch = '|';
	OperatorBaseInfo* bitOrInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitOrInfo) ) );

	the_ch = '^';
	OperatorBaseInfo* bitXOrInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitXOrInfo) ) );

	the_ch = '~';
	OperatorBaseInfo* bitNotInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitNotInfo) ) );

	the_ch = '<';
	OperatorBaseInfo* bitLeftShiftInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitLeftShiftInfo) ) );

	the_ch = '>';
	OperatorBaseInfo* bitRightShiftInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, bitRightShiftInfo) ) );

	the_ch = '(';
	OperatorBaseInfo* openParentheseInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, openParentheseInfo) ) );

	the_ch = ')';
	OperatorBaseInfo* closeParentheseInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, closeParentheseInfo) ) );

	the_ch = '=';
	OperatorBaseInfo* equalInfo = new OperatorBaseInfo( the_ch, 0);
	s_allCharSetMap.insert( make_pair(the_ch, new CharBaseInfo(the_ch, equalInfo) ) );

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


	// TODO
	CharUtil::s_positiveCharInfo = new CharBaseInfo('+', nullptr);
	CharUtil::s_positiveCharInfo = new CharBaseInfo('-', nullptr);
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
CharBaseInfo* CharUtil::getPositiveCharInfo() // +
{
	return s_positiveCharInfo;
}

// static 
CharBaseInfo* CharUtil::getNegativeCharInfo() // -
{
	return s_negativeCharInfo;
}


//
////////////////////////////////////////////////////////////////////////////////////////////////////
