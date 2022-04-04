#include "decimalParser.h"
#include "parserException.h"

#include "stringNumber.h"

using namespace std;

DecimalParser::DecimalParser(E_PaserType tp)
	: TokenParserBase(tp)
	, m_uCnt(0)
	, m_UCnt(0)
	, m_lCnt(0)
	, m_LCnt(0)
{
}

//
// virtual 
//
DecimalParser::~DecimalParser()
{
}

// virtual 
void DecimalParser::init() // override
{
	// m_AllAvalibleCharacters.insert( make_pair('+', CharUtil::getCharBaseInfo('+') ) );
	// m_AllAvalibleCharacters.insert( make_pair('-', CharUtil::getCharBaseInfo('-') ) );
	
	for ( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	m_AllAvalibleCharacters.insert( make_pair('x', CharUtil::getCharBaseInfo('x') ) );
	m_AllAvalibleCharacters.insert( make_pair('X', CharUtil::getCharBaseInfo('X') ) );

	m_AllAvalibleCharacters.insert( make_pair('.', CharUtil::getCharBaseInfo('.') ) );
	m_AllAvalibleCharacters.insert( make_pair('e', CharUtil::getCharBaseInfo('e') ) );
	m_AllAvalibleCharacters.insert( make_pair('E', CharUtil::getCharBaseInfo('E') ) );

	m_AllAvalibleCharacters.insert( make_pair('u', CharUtil::getCharBaseInfo('u') ) );
	m_AllAvalibleCharacters.insert( make_pair('U', CharUtil::getCharBaseInfo('U') ) );
	m_AllAvalibleCharacters.insert( make_pair('l', CharUtil::getCharBaseInfo('l') ) );
	m_AllAvalibleCharacters.insert( make_pair('L', CharUtil::getCharBaseInfo('L') ) );

	// very special string :
	//     1f   123f   334f  is not valid by the c/c++ compiler
	// m_AllAvalibleCharacters.insert( make_pair('f', CharUtil::getCharBaseInfo('f') ) );
	// m_AllAvalibleCharacters.insert( make_pair('F', CharUtil::getCharBaseInfo('F') ) );

	m_tokenType = E_TOKEN_INTEGER_NUMBER;
	m_tokenSubType = E_TOKEN_DECIMAL_NUMBER;

    m_exceptionCode = E_DECIMAL_INVALID_FORMAT;
	m_parserName = "DecimalParser";
}


// virtual 
E_PaserType  DecimalParser::appendContent(ParsedCharInfo* pInfo) // override
{
	char curCh = pInfo->currentChar;
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	auto inSideCharInfo = getInsideCharSetBaseInfo( curCh );
	if ( inSideCharInfo == nullptr ) {
		/*
		     123+ 
			    ^
		*/
		m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
		return E_P_DEFAULT;
	} else {
		//  inSideCharInfo != nullptr
		if ( sz == 1 ) {
			auto firstCh = m_alreadyTravelsaledString.at(0);

			////////////////////////////////////////////////////////////////////
			//
			if ( inSideCharInfo->isNumber() ) {
				m_alreadyTravelsaledString += curCh;
				m_endInfo = pInfo->position;
				if ( firstCh == '0' ) {
					m_switchFlag = E_TOKEN_CONVERT_TO_OTHER;
					return E_P_OCTAL;
				} else {
					// [1-9] + [0-9]   
					// e.g.   "2" + "1" = "21"     Valid
				}
			} else if ( is_dot(curCh) || is_eE(curCh)  ) {
				// 0.   [0-9].      or    [0-9]e    or  [0-9]E
				//      [0-9]f      [0-9]F
				m_alreadyTravelsaledString += curCh;
				m_endInfo = pInfo->position;
				// m_endInfo = pInfo->position;
				m_switchFlag = E_TOKEN_CONVERT_TO_OTHER;
				return E_P_FLOAT;
			} else if ( is_xX(curCh) ) {
				// 0x or 0X
				m_alreadyTravelsaledString += curCh;
				m_endInfo = pInfo->position;
				if ( firstCh == '0' ) {
					m_switchFlag = E_TOKEN_CONVERT_TO_OTHER;
					return E_P_HEX;
				} else {
					// throw :    1-9    , [1-9]x  or  [1-9]X
					throwErrMsg(pInfo,  "[1-9] append 'x/X' is invalid");
				}
			} else {
				// u/U  or  l/L
				update_uU_lLCnt(curCh, pInfo);
			}
			//
			////////////////////////////////////////////////////////////////////
		} else {
			// sz >= 2 
			if ( isSuffixExisted() ) {
				// End Flag has already been set
				if ( is_uU_lL(curCh) ) {
					update_uU_lLCnt(curCh, pInfo);
				} else {
					// 0-9        "."
					// e/E   x/X  
					throwErrMsg(pInfo,  "After u/U l/L suffix , Append [0-9] . e/E x/X  is not allowed");
				}
			} else {
				// 0  u/U(s)    &&   0 l/L(s) ,   previous string are all  numbers
				if ( inSideCharInfo->isNumber() ) {
					m_alreadyTravelsaledString += curCh;
					m_endInfo = pInfo->position;
				} else if ( is_dot(curCh) || is_eE(curCh)  ) {
					m_alreadyTravelsaledString += curCh;
					m_endInfo = pInfo->position;
					m_switchFlag = E_TOKEN_CONVERT_TO_OTHER;
					return E_P_FLOAT;
				} else if ( is_xX(curCh) ) {
					throwErrMsg(pInfo,  "[1-9] append 'x/X' is invalid");
				} else {
					// u/U   or  l/L
					update_uU_lLCnt(curCh, pInfo);
				}
			}
		}
	}

	return m_type;	
}


// virtual 
void DecimalParser::reset() // override;
{
	TokenParserBase::reset();

	m_uCnt = 0;
	m_UCnt = 0;
	m_lCnt = 0;
	m_LCnt = 0;
}



// virtual 
TokenInfo* DecimalParser::generateToken() // override;
{
    auto baseRet = TokenParserBase::generateToken();
    if ( baseRet != nullptr ) {
        E_DataType dt = E_TP_S_INT;

        int uUCnt = getuUCnt();
        int lLCnt = getlLCnt();
        int ulTotalCnt = uUCnt + lLCnt;

        int base = 0;
        if ( m_tokenSubType ==  E_TOKEN_DECIMAL_NUMBER ) {
            base = 10;
        } else if ( m_tokenSubType ==  E_TOKEN_OCTAL_NUMBER ) {
            base = 8;
        } else if ( m_tokenSubType ==  E_TOKEN_HEX_NUMBER ) {
            base = 16;
        } else {
            base = 10;
        }

        string copyStr(m_alreadyTravelsaledString);
        if ( ulTotalCnt > 0 ) {
            copyStr = copyStr.substr(0, static_cast<int>(copyStr.size() - ulTotalCnt) );
        }

        StringNumber curNumber(copyStr, base);

        if ( uUCnt > 0 ) {
            if ( lLCnt == 0 ) {
                dt = E_TP_U_INT;

                if ( base == 10 ) {
                    if ( curNumber > StringNumber::s_unsignedIntMax10 ) {
                        if ( curNumber > StringNumber::s_unsignedLongMax10 ) {
                            dt = E_TP_U_LONG_LONG;
                        } else {
                            dt = E_TP_U_LONG;
                        }
                    }
                } else if ( base == 8 ) {
                    if ( curNumber > StringNumber::s_unsignedIntMax8 ) {
                        if ( curNumber > StringNumber::s_unsignedLongMax8  ) {
                            dt = E_TP_U_LONG_LONG;
                        } else {
                            dt = E_TP_U_LONG;
                        }
                    }
                } else if ( base == 16 ) {
                    if ( curNumber > StringNumber::s_unsignedIntMax16 ) {
                        if ( curNumber > StringNumber::s_unsignedLongMax16  ) {
                            dt = E_TP_U_LONG_LONG;
                        } else {
                            dt = E_TP_U_LONG;
                        }
                    }
                }

            } else if ( lLCnt == 1 ) {
                dt = E_TP_U_LONG;
            } else if ( lLCnt == 2 ) {
                dt = E_TP_U_LONG_LONG;
            }
        } else {
            // 0 u/U Flag
            if ( lLCnt == 0 ) {
                dt = E_TP_S_INT;

                if ( base == 10 ) {
                    if ( curNumber > StringNumber::s_signedIntMax10 ) {
                        if ( curNumber > StringNumber::s_signedLongMax10 ) {
                            dt = E_TP_S_LONG_LONG;
                        } else {
                            dt = E_TP_S_LONG;
                        }
                    }
                } else if ( base == 8 ) {
                    if ( curNumber > StringNumber::s_signedIntMax8  ) {
                        if ( curNumber > StringNumber::s_signedLongMax8   ) {
                            dt = E_TP_S_LONG_LONG;
                        } else {
                            dt = E_TP_S_LONG;
                        }
                    }
                } else if ( base == 16 ) {
                    if ( curNumber > StringNumber::s_signedIntMax16  ) {
                        if ( curNumber > StringNumber::s_signedLongMax16  ) {
                            dt = E_TP_S_LONG_LONG;
                        } else {
                            dt = E_TP_S_LONG;
                        }
                    }
                }

            } else if ( lLCnt == 1 ) {
                dt = E_TP_S_LONG;
            } else if ( lLCnt == 2 ) {
                dt = E_TP_S_LONG_LONG;
            }
        }

        baseRet->setDataType(dt);
    }
    return baseRet;
}



void DecimalParser::update_uU_lLCnt(char ch, ParsedCharInfo* pInfo)
{
	if ( ch == 'u' ) {
		m_alreadyTravelsaledString += ch;
		m_endInfo = pInfo->position;

		if ( m_UCnt > 0 ) {
			//  throw 'U' already existed
			throwErrMsg(pInfo, " 2 u/U(s) is not allowed ");
		} else {
			// 0 'U'
			++m_uCnt;

			if ( m_uCnt > 1 ) {
				// throw multi 'u' s
				throwErrMsg(pInfo, " 2 u/U(s) is not allowed ");
			}
			// u.cnt == 1
		}
	} else if ( ch == 'U' ) {
		m_alreadyTravelsaledString += ch;
		m_endInfo = pInfo->position;

		if ( m_uCnt > 0 ) {
			// throw 'u' already existed
			throwErrMsg(pInfo, " 2 u/U(s) is not allowed ");
		} else {
			// 0 'u'
			++m_UCnt;

			if ( m_UCnt > 1 ) {
				// throw multi 'U' s
				throwErrMsg(pInfo, " 2 u/U(s) is not allowed ");
			}
			// U.cnt == 1
		}
	} else if ( ch == 'l' ) {

		if ( m_LCnt > 0 ) {
			//  throw      'L' already existed
			m_alreadyTravelsaledString += ch;
			m_endInfo = pInfo->position;

			throwErrMsg(pInfo, " 'L' is already existed , 'l' is not  allowed ");
		} else {
			// L.cnt == 0

			auto isLast_uU = false;
			if ( m_uCnt > 0 || m_UCnt > 0 ) {
				auto lastCh = m_alreadyTravelsaledString.back();
				isLast_uU = (lastCh == 'u' || lastCh == 'U');
			} 

			++m_lCnt;
			if ( m_lCnt > 2 ) {
				m_alreadyTravelsaledString += ch;
				m_endInfo = pInfo->position;

				// throw     l.cnt > 2
				throwErrMsg(pInfo, "more than 2 l(s) is not allowed ");
			} else if ( m_lCnt == 2 ) {
				// l.cnt <=2 : OK
				if ( isLast_uU ) {
					m_alreadyTravelsaledString += ch;
					m_endInfo = pInfo->position;
					//  throw ...   "lul"  is not allowed
					throwErrMsg(pInfo, " 'lul' is not allowed ");
				} else {
					m_alreadyTravelsaledString += ch;
					m_endInfo = pInfo->position;
				}
			} else {
				// == 1
				m_alreadyTravelsaledString += ch;
				m_endInfo = pInfo->position;
			}
		}
	} else if ( ch == 'L' ) {

		auto isLast_uU = false;
		if ( m_uCnt > 0 || m_UCnt > 0 ) {
			auto lastCh = m_alreadyTravelsaledString.back();
			isLast_uU = (lastCh == 'u' || lastCh == 'U');
		} 

		++m_LCnt;
		if ( m_LCnt > 2 ) {
			m_alreadyTravelsaledString += ch;
			m_endInfo = pInfo->position;
			// throw     L.cnt > 2
			throwErrMsg(pInfo,  "more than 2 L(s) is not allowed "); 
		} else if ( m_LCnt == 2 ) {
			// L.cnt == 2
			if ( isLast_uU ) {
				m_alreadyTravelsaledString += ch;
				m_endInfo = pInfo->position;
				//  throw ...   "LuL"  is not allowed
				throwErrMsg(pInfo, " 'LuL' is not allowed ");
			} else {
				m_alreadyTravelsaledString += ch;
				m_endInfo = pInfo->position;
			}
		} else {
			// L.cnt == 1
			m_alreadyTravelsaledString += ch;
			m_endInfo = pInfo->position;

			if ( m_lCnt > 0 ) {
				throwErrMsg(pInfo, " 'l' is already existed , 'L' is not  allowed ");
			} 
			// OK  l.cnt = 0
		}
	}
}


bool DecimalParser::is_dot(char ch)
{
	return ch == '.';
}


bool DecimalParser::is_eE(char ch)
{
	return ch == 'e' || ch == 'E';
}



bool DecimalParser::is_xX(char ch)
{
	return ch == 'x' || ch == 'X';
}


bool DecimalParser::is_uU_lL(char ch)
{
	return ch == 'u' || ch == 'U' || ch == 'l' || ch == 'L';
}


bool DecimalParser::isSuffixExisted()
{
	return m_uCnt>0 || m_UCnt>0 || m_lCnt>0 || m_LCnt>0;
}


// virtual 
bool DecimalParser::isTokenValid() // override
{
	int prefixNumCnt = 0;
	int suffixNumCnt = 0;
	int otherCnt = 0;
	bool suffixExisted = false;
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );

	for( int i = 0; i < sz; ++i)
	{
		auto ch =  m_alreadyTravelsaledString.at(i);
		if ( ch >='0' && ch <='9' ) {
			if ( suffixExisted ) {
				++suffixNumCnt;
			} else {
				++prefixNumCnt;
			}
		} else if ( is_uU_lL(ch) ) {
			suffixExisted = true;
		} else {
			++otherCnt;
		}
	}
	

	return ( prefixNumCnt>0 
			 && suffixExisted==0 
			 && otherCnt == 0    );
}


// bool DecimalParser::is_fF(char ch)
// {
// 	return ch == 'f' || ch == 'F';
// }



int DecimalParser::getuUCnt()
{
    return static_cast<int>(m_uCnt + m_UCnt);
}

int DecimalParser::getlLCnt()
{
    return static_cast<int>(m_lCnt + m_LCnt);
}


// virtual 
bool DecimalParser::checkIsInsideRange() // override
{
    // TODO 
    return true;
}


