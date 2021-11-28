#ifndef FLOAT_PARSER_H
#define FLOAT_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

#include "parsedCharInfo.h"

class FloatParser : public TokenParserBase
{
public:
	FloatParser(E_PaserType tp);
	virtual ~FloatParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo) override;

	virtual void reset() override;
	virtual bool isEnd(ParsedCharInfo* pInfo) override;
	virtual bool isTokenValid() override;
protected:
	int m_dotCnt;      // .
	int m_eECnt;       // e/E
	int m_fFCnt;       // f/F
	int m_numberCnt;   // 0-9
	int m_positieCnt;  // +
	int m_negativeCnt; // -

	void innerReset();

	bool is_eE(char ch);
	bool is_fF(char ch);
	bool is_dot(char ch);
	bool is_positiveSign(char ch);
	bool is_negativeSign(char ch);
	bool is_number(char ch);

	void summary_eE_fF_dot_number();
};

#endif
