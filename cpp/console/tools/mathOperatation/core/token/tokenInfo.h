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


    void setDataType1st(E_DataType tp);
    void doIntergerPromotion(E_DataType tp);
    void doImplyDataTypeConvertion(E_DataType tp);

    bool hasDoIntergerPromotion();
    bool hasDoImplyDataTypeConvertion();

    E_DataType getDataType();
protected:
	E_TokenType m_type;
	E_TokenType m_subType;
	
	std::string m_strSequence;
    int         m_uUCnt;      // 'u/U' Count , such as   3u or 3U
    int         m_lLCnt;      // 'l/L' Count , such as   3l or 3L or 3ll or 3LL
    int         m_fCnt;       // 'f'   Count , such as   3.0f

	PosInfo m_beginPos;
	PosInfo m_endPos;


    E_DataType m_dataTypeOriginal;
    E_DataType m_dataTypeIntPromotion;
    E_DataType m_dataTypeConvertion;

    E_DataType m_dataType;

    bool        m_hasDoIntergerPromotion;
    bool        m_hasDoDataTypeConvertion;
};

#endif
