#ifndef DECIMAL_PARSER_H
#define DECIMAL_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

#include "parsedCharInfo.h"

class DecimalParser : public TokenParserBase
{
public:
	DecimalParser(E_PaserType tp);
	virtual ~DecimalParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo) override;

	virtual void reset() override;
protected:
	unsigned short m_uCnt;
	unsigned short m_UCnt;
	unsigned short m_lCnt;
	unsigned short m_LCnt;

	bool is_dot(char ch);
	bool is_eE(char ch);
	bool is_xX(char ch);
	bool is_uU_lL(char ch);
	bool is_fF(char ch);

	// u/U   l/L
	bool isSuffixExisted();

	void update_uU_lLCnt(char ch);
	
};

#endif

