#ifndef TOKEN_INFO_H
#define TOKEN_INFO_H

#include <string>
#include "commonEnum.h"
#include "parsedCharInfo.h"


class TokenInfo
{
public:
	TokenInfo(E_TokenType tp, E_TokenType subtype);
	virtual ~TokenInfo();

	E_TokenType getType();
	E_TokenType getSubType();
	void        resetSubType(E_TokenType tp);

	std::string getDetail();
	void setDetail(const std::string& token);

	void setPosInfo(const PosInfo& begPos, const PosInfo& endPos);

	const PosInfo& getBeginPos() const;
	const PosInfo& getEndPos() const;
protected:
	E_TokenType m_type;
	E_TokenType m_subType;
	std::string m_strSequence;

	PosInfo m_beginPos;
	PosInfo m_endPos;
};

#endif
