#include <iostream>
#include <cassert>
#include "floatParser.h"

using namespace std;

FloatParser::FloatParser(E_PaserType tp)
	: TokenParserBase(tp)
	, m_dotCnt(0)
	, m_eECnt(0)
	, m_fFCnt(0)
	, m_numberCnt(0)
	, m_positieCnt(0)
	, m_negativeCnt(0)
{

}



//
// virtual 
//
FloatParser::~FloatParser()
{
}




// virtual 
void FloatParser::init() // override
{
	// 1.23e-2f    1.23e+2f

	for ( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}
	m_AllAvalibleCharacters.insert( make_pair('.', CharUtil::getCharBaseInfo('.') ) );

	m_AllAvalibleCharacters.insert( make_pair('e', CharUtil::getCharBaseInfo('e') ) );
	m_AllAvalibleCharacters.insert( make_pair('E', CharUtil::getCharBaseInfo('E') ) );

	m_AllAvalibleCharacters.insert( make_pair('f', CharUtil::getCharBaseInfo('f') ) );
	m_AllAvalibleCharacters.insert( make_pair('F', CharUtil::getCharBaseInfo('F') ) );

	// e.g.     2e+5f     2e-5f
	m_AllAvalibleCharacters.insert( make_pair('+', CharUtil::getCharBaseInfo('+') ) );
	m_AllAvalibleCharacters.insert( make_pair('-', CharUtil::getCharBaseInfo('-') ) );

	m_tokenType = E_TOKEN_FLOAT_NUMBER;
}


// virtual 
E_PaserType  FloatParser::appendContent(ParsedCharInfo* pInfo) // override
{
	summary_eE_fF_dot_number();

	char curCh = pInfo->currentChar;
	auto inSideCharInfo = getInsideCharSetBaseInfo( curCh );
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	if ( inSideCharInfo == nullptr ) {
		/*
		    1.2+
			   ^
		*/
		m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
		return E_P_DEFAULT;
	} else {
        // inSideCharInfo != nullptr
		char firstCh = 0;
		if ( sz == 1 ) {
			firstCh = m_alreadyTravelsaledString.at(0);
			if ( !is_dot(firstCh)  ) {
				cout << "[ERROR] In E_P_FLOAT Parser, First char != '0' , m_alreadyTravelsaledString = \"" << m_alreadyTravelsaledString << "\"" << endl;
				assert(false);
			}

			if ( inSideCharInfo->isNumber() ) {
				m_alreadyTravelsaledString += curCh;
				m_endInfo = pInfo->position;
			} else {
				// TODO : throw
			}
		} else if ( sz == 2 ) {
			// [0-9].      [0-9]e   or [0-9]E    
			if ( m_dotCnt == 1 ) {
				// [0-9].
				if ( is_number(curCh)  || is_eE(curCh) ) {
					// 1.2    or   1.e
					m_alreadyTravelsaledString += curCh;
				} else if ( is_fF(curCh)  ) {
					m_alreadyTravelsaledString += curCh;
					m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;
					return E_P_DEFAULT;
				} else if ( is_dot(curCh ) ) {
					// TODO : throw   multi   2 '.'s
				} else {
					//  +/- ,    + : addition ,   - : substract
					m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
					return E_P_DEFAULT;
				}
			} else if ( m_eECnt == 1 ) {
				// [0-9]e   or   [0-9]E
				if ( is_number(curCh) ) {
					m_alreadyTravelsaledString += curCh;
				} else if ( is_positiveSign(curCh) || is_negativeSign(curCh) ) {
					// 2e+      or    2e-
					m_alreadyTravelsaledString += curCh;
				} else if ( is_dot(curCh) ) {
					// TODO : throw  "e."    "E."  is not allowed
				} else if( is_fF(curCh) ) {
					// TODO : throw  "1eF"    "1EF"  is not allowed
				} else if ( is_eE(curCh) ) {
					// TODO : "1eE"  or  "1EE" is not allowed
				}
			}
		} else {
			// sz >= 3
			auto lastCh = m_alreadyTravelsaledString.back();
			if ( m_eECnt > 0 ) {
				if ( m_dotCnt > 0 ) {
					/*
					      e.g    like    1.2e...
					*/
					if ( is_eE(lastCh) ) { // last is e
						if ( is_number(curCh) || is_positiveSign(curCh) || is_negativeSign(curCh) ) {
							m_alreadyTravelsaledString += curCh;
						} else {
							// "."    e/E   f/F
							// TODO : throw
						}
					} else if ( is_dot(lastCh) ) {
						/*
						     last is Not  e/E
							 1234.
						*/
						if ( is_number(curCh) || is_eE(curCh) ) {
							// 123.4 or   123.e
							m_alreadyTravelsaledString += curCh;
						} else if ( is_fF(curCh) ) {
							m_alreadyTravelsaledString += curCh;
							m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;
							return E_P_DEFAULT;
						} else if ( is_positiveSign(curCh) || is_negativeSign(curCh) ) {
							// + : additive    ,  - : substract
							m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
							return E_P_DEFAULT;
						} else {
							// "."
							// TODO : throw
						} 
						
					} else if ( is_positiveSign(lastCh) || is_negativeSign(lastCh) ) {
						// 2e+   or   2e-
						if ( is_number(curCh) ) {
							m_alreadyTravelsaledString += curCh;
						} else {
							//       "."  e/E   f/F    +-
							// TODO : throw    
						}
					} else if ( is_number(lastCh) ) {
						if ( is_number(curCh) ) {
							m_alreadyTravelsaledString += curCh;
						} else if ( is_fF(curCh) ) {
							m_alreadyTravelsaledString += curCh;
							m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;
							return E_P_DEFAULT;
						} else {
							// TODO : throw
						}
					}
				} else {
					//   0 "."(s)   ,  with only 1 e/E
					if ( is_number(lastCh) ) {
						if ( is_number(curCh ) ) {
							m_alreadyTravelsaledString += curCh;
						} else if ( is_fF(curCh) ) {
							m_alreadyTravelsaledString += curCh;
							m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;
							return E_P_DEFAULT;
						} else {
							// "." ,    e/E   ,   +/-
							// TODO : throw   
						}
					} else if ( is_positiveSign(lastCh) ||  is_negativeSign(lastCh) ) {
						if ( is_number(curCh) ) {
							m_alreadyTravelsaledString += curCh;
						} else {
							// "."     e/E    f/F    +/-
							// TODO : throw
						}
					} else if ( is_eE(lastCh) ) {
						if ( is_number(curCh) ||  is_positiveSign(curCh) || is_negativeSign(curCh) ) {
							m_alreadyTravelsaledString += curCh;
						} else {
							//   "."   e/E   f/F
							//   TODO : throw
						}
					}
				}
			} else {
				// 0 e/E(s) , with 1 "." only
				if ( is_dot(lastCh) ) {
					if ( is_number(curCh) || is_eE(curCh) ) {
						// 1.1  or   1.e
						m_alreadyTravelsaledString += curCh;
					} else if ( is_dot(curCh) ) {
						// 1..
						// TODO : throw
					} else if ( is_fF(curCh) ) {
						// 1.f
						m_alreadyTravelsaledString += curCh;
						m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;
						return E_P_DEFAULT;
					} else if ( is_positiveSign(curCh) || is_negativeSign(curCh) ) {
						// +/-   e.g.     1.-0.2   ==>  0.8     1.+0.2 = 1.2
						m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
						return E_P_DEFAULT;
					}
				} else if ( is_number(lastCh)  )  {
					if ( is_number(curCh) || is_eE(curCh) ) {
						// 1.23   1.2e
						m_alreadyTravelsaledString += curCh;
					} else if ( is_dot(curCh) ) {
						// 1.2.
						// TODO : throw
					} else if ( is_fF(curCh) ) {
						m_alreadyTravelsaledString += curCh;
						m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;
						return E_P_DEFAULT;
					} else if ( is_positiveSign(curCh) || is_negativeSign(curCh) ) {
						// 1.2-   1.2+
						m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
						return E_P_DEFAULT;
					}
				}
			}
		}
	}

	return m_type;	
}


// virtual 
void FloatParser::reset() // override;
{
	TokenParserBase::reset();
	 
	innerReset();
}


bool FloatParser::is_eE(char ch)
{
	return ch == 'e' || ch == 'E';
}

bool FloatParser::is_fF(char ch)
{
	return ch == 'f' || ch == 'F';
}

bool FloatParser::is_dot(char ch)
{
	return ch == '.';
}

bool FloatParser::is_positiveSign(char ch)
{
	return ch == '+';
}

bool FloatParser::is_negativeSign(char ch)
{
	return ch == '-';
}


bool FloatParser::is_number(char ch)
{
	return ch>='0' && ch<='9';
}

void FloatParser::summary_eE_fF_dot_number()
{
	innerReset();

	for( const auto& ch : m_alreadyTravelsaledString )
	{
		if ( is_eE(ch) ) {
			++m_eECnt;
		} else if ( is_fF(ch) ) {
			++m_fFCnt;
		} else if( is_dot(ch) ) {
			++m_dotCnt;
		} else if ( is_number(ch) ) {
			++m_numberCnt;
		} else if ( is_positiveSign(ch) ) {
			++m_positieCnt;
		} else if ( is_negativeSign(ch) ){
			++m_negativeCnt;
		}
	}
}


// virtual 
bool FloatParser::isEnd(ParsedCharInfo* pInfo) // override;
{
	if ( m_alreadyTravelsaledString.empty() ) {
		return false;
	}

	summary_eE_fF_dot_number();
	char lastCh = m_alreadyTravelsaledString.back();
	if ( is_fF(lastCh)  ) {
		// 1f is valid   1.2f ,   1e2f are both valid
		return m_numberCnt>0;
	} else {
		// no f/F @ suffix
		return     (m_numberCnt > 0) 
			    && (m_dotCnt == 1 || m_eECnt== 1);
	}
	
}

void FloatParser::innerReset()
{
	m_dotCnt = 0;
	m_eECnt = 0;
	m_fFCnt = 0;
	m_numberCnt = 0;

	m_positieCnt = 0;
	m_negativeCnt = 0;
}


