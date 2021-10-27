#ifndef TOKEN_INFO_H
#define TOKEN_INFO_H

#include <string>
#include "commonEnum.h"


class TokenInfo
{
public:
	TokenInfo(E_TokenType tp, E_TokenType subtype);
	virtual ~TokenInfo();

	E_TokenType getType();
	E_TokenType getSubType();
	std::string getDetail();
	void setDetail(const std::string& token);
protected:
	E_TokenType m_type;
	E_TokenType m_subType;
	std::string m_strSequence;
};

#endif
