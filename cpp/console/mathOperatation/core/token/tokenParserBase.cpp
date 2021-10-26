#include "tokenParserBase.h"
#include "parserException.h"

using namespace std;

TokenParserBase::TokenParserBase(E_PaserType tp)
	: m_alreadyTravelsaledString("")
	, m_token("")
	, m_type(tp)
{
	m_AllAvalibleCharacters.clear();


	//
	// Char Set
	//
	m_CharSet.clear();
	for ( char ch = 'a'; ch <= 'z'; ++ch ) {
		m_CharSet.insert(  make_pair(ch, CharInfo(E_LETTER, E_CAT_ALPHA) ) );
	}

	for ( char ch = 'A'; ch <= 'Z'; ++ch ) {
		m_CharSet.insert(  make_pair(ch, CharInfo(E_LETTER, E_CAT_ALPHA) ) );
	}

	for ( char ch = '0'; ch <= '9'; ++ch ) {
		m_CharSet.insert(  make_pair(ch, CharInfo(E_NUMBER, E_CAT_NUMBER) ) );
	}
	m_CharSet.insert(  make_pair('.', CharInfo(E_DOT, E_CAT_OTHERS) ) );
	m_CharSet.insert(  make_pair('_', CharInfo(E_UNDERLINE, E_CAT_UNDER_LINE) ) );

	m_CharSet.insert(  make_pair('+', CharInfo(E_ADD, E_CAT_OPERATOR) ) );
	m_CharSet.insert(  make_pair('-', CharInfo(E_MINUS, E_CAT_OPERATOR) ) );
	m_CharSet.insert(  make_pair('*', CharInfo(E_MULTIPLY, E_CAT_OPERATOR) ) );
	m_CharSet.insert(  make_pair('/', CharInfo(E_DIVIDE, E_CAT_OPERATOR) ) );
	m_CharSet.insert(  make_pair('%', CharInfo(E_MOD, E_CAT_OPERATOR) ) );

	m_CharSet.insert(  make_pair('&', CharInfo(E_BIT_AND, E_CAT_OPERATOR) ) );
	m_CharSet.insert(  make_pair('|', CharInfo(E_BIT_OR, E_CAT_OPERATOR) ) );
	m_CharSet.insert(  make_pair('^', CharInfo(E_BIT_XOR, E_CAT_OPERATOR) ) );
	m_CharSet.insert(  make_pair('~', CharInfo(E_BIT_NOT, E_CAT_OPERATOR) ) );

	m_CharSet.insert(  make_pair('<', CharInfo(E_LESS_THAN, E_CAT_OPERATOR) ) );
	m_CharSet.insert(  make_pair('>', CharInfo(E_GREATER_THAN, E_CAT_OPERATOR) ) );

	m_CharSet.insert(  make_pair('(', CharInfo(E_OPEN_PARENTHESES,  E_CAT_PARENTHESES) ) );
	m_CharSet.insert(  make_pair(')', CharInfo(E_CLOSE_PARENTHESES, E_CAT_PARENTHESES) ) );

	m_CharSet.insert(  make_pair(';', CharInfo(E_SEMICOLON, E_CAT_OTHERS) ) );
	m_CharSet.insert(  make_pair('=', CharInfo(E_ASSIGN, E_CAT_ASSIGNMENT ) ) );

	m_CharSet.insert(  make_pair(' ',  CharInfo(E_SPACE, E_CAT_SEPERATOR) ) );
	m_CharSet.insert(  make_pair('\t', CharInfo(E_TAB, E_CAT_SEPERATOR) ) );
	m_CharSet.insert(  make_pair('\r', CharInfo(E_NEW_LINE_R, E_CAT_SEPERATOR ) ) );
	m_CharSet.insert(  make_pair('\n', CharInfo(E_NEW_LINE_N, E_CAT_SEPERATOR) ) );

}


// virtual 
TokenParserBase::~TokenParserBase()
{
}


// virtual 
void TokenParserBase::init()
{

}


pair< std::unordered_map<char, CharInfo>::iterator, bool> TokenParserBase::commonCheck(char ch, ParserInfo* pInfo)
{
	auto pr = isValidChar(ch);
	if ( !pr.second ) {
		ParserExpection	e(E_ExceptionCode::E_UNKNOWN_CHAR);

		string info;
		if ( ch >= 0 ) {
			// >= 32
			if ( ch < 32 ) {
				if ( ch == '\t' ) {
					info += "'\\t'";
				} else if ( ch == '\r' ) {
					info += "'\\r'";
				} else if ( ch == '\n' ) {
					info += "'\\n'";
				} else {
					info += " ? , code = ";  
					info += std::to_string( static_cast<int>(ch) );
				}
			} else {
				if ( ch == 32 ) {
					info += "' '";
				} else {
					info += "'";
					info += ch;
					info += "'";
				}
			}
		} else {
			// < 0
			info += " ? , code = ";  
			info += std::to_string( static_cast<int>(ch) );
		}

		info += "  | index = ";
		info += std::to_string(pInfo->nCharIdx);
		info += " @Line ";
		info += std::to_string(pInfo->nLine);
		info += ":";
		info += std::to_string(pInfo->nCol);

		e.setDetail(info);

		throw e;
	}

	return pr; 
}

// virtual 
E_PaserType  TokenParserBase::appendContent(char ch, ParserInfo* pInfo)
{
	auto pr = commonCheck(ch,pInfo);

	m_alreadyTravelsaledString += ch;
	CharInfo chDetail = (pr.first)->second;
	E_Char_Category cat = chDetail.cat;
	if ( cat == E_CAT_SEPERATOR ) {
		return E_P_BLANK;
	} else if ( cat == E_CAT_OPERATOR ) {
		if ( pInfo->hasPreviousChar ) {
			if ( ch == '/' ) {
				// current = '/'
				if ( pInfo->previousChar == '/' ) {
					return E_P_SINGLE_LINE_COMMENT;
				} 
			} else {
				// current = '*'
				//  START    "/*"
				if ( pInfo->previousChar == '/' ) {
					return E_P_MULTI_LINE_COMMENT;
				} 
			}
		}

		return E_P_OPERATOR;
	}
	
	return E_P_UNDETERMIND;	
}



pair< std::unordered_map<char, CharInfo>::iterator, bool> TokenParserBase::isValidChar(char ch)
{
	auto it = m_CharSet.find(ch);
    auto b = it != m_CharSet.end();
	return std::make_pair(it,b);
}



void TokenParserBase::transferToken(TokenParserBase* pBase)
{
	this->m_token = pBase->m_alreadyTravelsaledString;
	m_alreadyTravelsaledString.clear();
}


E_PaserType TokenParserBase::getType()
{
	return m_type;
}
