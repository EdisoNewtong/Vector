#ifndef CHAR_UTIL_H
#define CHAR_UTIL_H

#include <unordered_map>
#include "commonEnum.h"

class OperatorBaseInfo
{
public:
	OperatorBaseInfo(char ch, unsigned int meta);
	char getChar();
	unsigned int getPriority();
	bool isBinaryOp();          // 0:None   1:unary op         2:binary op   
	bool isLeft2Right();        // 0:None   1:Left-to-Right    2:Right-to-Left
protected:
	//    16   bits int is enough  ( C++ Standard ensure that     sizeof(int) >= 16
	// | Associativlity | Unary or Binary | Priority |
	// |  2 bit         |  2 bit          |  6 bits  |
	char         m_ch;
	unsigned int m_metaInfo;
public:
	static const unsigned int S_UNARY_OP_FLAG;
	static const unsigned int S_BINARY_OP_FLAG;

	static const unsigned int S_Left2Right_FLAG;
	static const unsigned int S_Right2Left_FLAG;
};

class CharBaseInfo
{
public:
	CharBaseInfo(char ch, OperatorBaseInfo* pOpInfo);
	virtual ~CharBaseInfo();

	char getCh();
	E_CharType getCategory();

	bool isAlpha();
	bool isNumber();
	bool isUnderLine();
	bool isVaribleCharSet();
	bool isVaribleHeadChar();

	bool isDot();

	bool isOpType();

	bool isSemicolon();

	bool isBlank();

	OperatorBaseInfo* getOpInfo();
protected:
	char m_ch;
	E_CharType  m_category;
	OperatorBaseInfo* m_pOpInfo;
};


class CharUtil
{
public:
	static void init();
	static void finalize();
	static std::unordered_map<char,CharBaseInfo*>* getAllCharSet();
	static CharBaseInfo* getCharBaseInfo(char ch);

	static CharBaseInfo* getPositiveCharInfo(); // +    e.g.    +123
	static CharBaseInfo* getNegativeCharInfo(); // -    e.g.    -234
protected:
	static std::unordered_map<char,CharBaseInfo*> s_allCharSetMap;

	static CharBaseInfo* s_positiveCharInfo;
	static CharBaseInfo* s_negativeCharInfo;
};

#endif 
