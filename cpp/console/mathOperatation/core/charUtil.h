#ifndef CHAR_UTIL_H
#define CHAR_UTIL_H

#include <unordered_map>

class OperatorBaseInfo
{
public:
	OperatorBaseInfo(char ch, unsigned int meta);
	char getChar();
	unsigned int getPriority();
	bool isBinaryOp();
	bool isLeft2Right();
protected:
	//    32   bits int is enough
	// | Associativlity | Unary or Binary | Priority |
	// |  1 bit         |  1 bit          |  6 bits  |
	char         m_ch;
	unsigned int m_metaInfo;
};

class CharBaseInfo
{
public:
	CharBaseInfo(char ch, OperatorBaseInfo* pOpInfo);
	virtual ~CharBaseInfo();

	char getCh();
	unsigned int getCategory();

	bool isAlpha();
	bool isNumber();
	bool isUnderLine();
	bool isVaribleCharSet();

	bool isDot();

	bool isOpType();

	bool isSemicolon();

	bool isBlank();

	OperatorBaseInfo* getOpInfo();
protected:
	char m_ch;
	unsigned int  m_category;
	OperatorBaseInfo* m_pOpInfo;
};


class CharUtil
{
public:
	static void init();
	static void finalize();
	static CharBaseInfo* getCharBaseInfo(char ch);
protected:
	static std::unordered_map<char,CharBaseInfo*> s_allCharSetMap;
};

#endif 
