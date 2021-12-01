#include <cassert>
#include <iostream>
#include "tokenListAnalyzer.h"
#include "parserException.h"
using namespace std;

TokenListAnalyzer::TokenListAnalyzer()
	: m_tokenList()
	, m_banPickCfgMap()
	, m_evaluateSuffixExpression()
	, m_operatorStack()
{
	m_tokenList.clear();

	initBanPickCfg();
}
	
// virtual 
TokenListAnalyzer::~TokenListAnalyzer()
{
	for( auto it = m_tokenList.begin(); it != m_tokenList.end(); ++it )
	{
		if ( *it != nullptr ) {
			delete (*it);
		}
	}
	m_tokenList.clear();

}


bool TokenListAnalyzer::isListEmpty()
{
	return m_tokenList.empty();
}



// bool TokenListAnalyzer::checkParserIsValid(E_PaserType tp)
// {
// 	(void)tp;
// 	return true;
// }

int TokenListAnalyzer::pushToken(TokenInfo* pToken)
{
	static const string SC_LEFT(" ( ");
	static const string SC_RIGHT(" ) ");

	judgeTokenIsPositiveOrNegativeAndReset(pToken);

	if ( pToken == nullptr ) {
		cout << "[ERROR] : Pushed Token == nullptr" << endl;
		assert(false);
	}

	//
	// pToken != nullptr
	//
	m_tokenList.push_back( pToken );

	auto curTp = pToken->getType();
	if ( curTp == E_TOKEN_OPERATOR ) {
		curTp = pToken->getSubType();
	}

	const auto& endPos = pToken->getEndPos();
	auto curTokenType = getTokenName(curTp);

	if ( !(curTp == E_TOKEN_BLANK || curTp == E_TOKEN_COMMENT_TYPE) ) {
		if (    curTp == E_TOKEN_INTEGER_NUMBER 
			 || curTp == E_TOKEN_FLOAT_NUMBER
			 || curTp == E_TOKEN_VARIBLE )
		{
			m_evaluateSuffixExpression.push_back( pToken );
		}
		else if ( isOperatorType(curTp) ) 
		{
			if ( curTp == E_TOKEN_OP_CLOSE_PARENTHESES ) {
				// special ')'
				if ( !hasMathedOpenParentheseBefore() ) {
					// TODO : throw
				} else {
					// pop previous element until '('
					popUtilOpenParenthese();
				}
			} else {
				auto pOperInfo = token2Op(pToken);
				if ( pOperInfo == nullptr ) {
					cout << "[ERROR] : Can't found OperatorBaseInfo about " << getTokenName(pToken->getSubType()) << endl;
					assert(false);
				}

				tryPopPreviousOperatorLowerPriority(pToken);
			}
		}



		int hasSkipBlank = 0;
		auto previousValidToken = getPreviousToken(true, &hasSkipBlank, true);
		if ( previousValidToken != nullptr ) {
			auto previousType = previousValidToken->getType();
			if ( previousType == E_TOKEN_OPERATOR ) {
				previousType = previousValidToken->getSubType();
			}
			auto previousTokenType = getTokenName(previousType);

			auto key = genFlag(previousType, curTp);
			auto foundIt = m_banPickCfgMap.find(key);
			if ( foundIt != m_banPickCfgMap.end() ) {
				auto banPickFlag = foundIt->second;

				if ( banPickFlag == 0 ) {
					// Not Allowed at all
					string detail;
					detail += "@";
					detail += to_string(endPos.nLine);
					detail += ":";
					detail += to_string(endPos.nCol);
					detail += ", charIdx = ";
					detail += to_string( static_cast<int>(endPos.nCharIdx) );
					detail += ", ";
					detail +=   (previousTokenType 
								  + SC_LEFT + previousValidToken->getDetail() + SC_RIGHT 
								  + string(" Can't located in front of Type   ") + curTokenType
								  + SC_LEFT + pToken->getDetail() + SC_RIGHT);

					ParserException e(E_TOKEN_LOGIC_INVALID);
					e.setDetail( detail );
					throw e;
				} else if ( banPickFlag == 1 ) {
					// No flag whose value is (0,1)
				} else if ( banPickFlag == 2 ) {
					//  banPickFlag = 10(2) = 2(10)
					if ( hasSkipBlank == 1 ) {
						// Valid
					} else {
						// continuously token next to previous is not valid
						// auto detail = previousTokenType + string(" Can't located <No-Skip> in front of Type   ") + curTokenType;
						string detail;
						detail += "@";
						detail += to_string(endPos.nLine);
						detail += ":";
						detail += to_string(endPos.nCol);
						detail += ", charIdx = ";
						detail += to_string( static_cast<int>(endPos.nCharIdx) );
						detail += ", ";
						detail +=  (previousTokenType 
									 + SC_LEFT + previousValidToken->getDetail() + SC_RIGHT 
									 + string(" Can't located <No-Skip> in front of Type   ") + curTokenType
									 + SC_LEFT + pToken->getDetail() + SC_RIGHT);

						ParserException e(E_TOKEN_LOGIC_INVALID);
						e.setDetail( detail );
						throw e;
					}
				} else {
					// banPickFlag == 3u,  both is OK
				}
			} else { 
				cout << "[ERROR] : Can't find key : " << previousTokenType << " After " <<  curTokenType <<  endl;
				assert(false);
			}
		} else {
			// previousValidToken == nullptr
			// pToken is the 1st pushed token
			//     int  / float   / varible / semicolon /  : All Valid
			//     123; /  3.5f;  /   nCnt  /    ;      /
			//  /* ... */ : 1st is OK
			//  // ...    : 1st is OK
			/*

				Operator is partially valid
				Valid list include : +    -    ~     ( 

			*/
			if (    curTp == E_TOKEN_BLANK 
				 || curTp == E_TOKEN_COMMENT_TYPE
				 ||	curTp == E_TOKEN_INTEGER_NUMBER 
				 || curTp == E_TOKEN_FLOAT_NUMBER 
				 || curTp == E_TOKEN_VARIBLE
				 || curTp == E_TOKEN_SEMICOLON
				 ////////////////////////////////////////////////
				 //  Special Operator 
				 || curTp == E_TOKEN_OP_ADD
				 || curTp == E_TOKEN_OP_MINUS
				 || curTp == E_TOKEN_OP_BIT_NOT
				 || curTp == E_TOKEN_OP_OPEN_PARENTHESES 
				 // 
				 ////////////////////////////////////////////////
			) 
			{
				// 1st pushed is Valid
			} else {
				ParserException e(E_TOKEN_LOGIC_INVALID);

				string detail;
				detail += "@";
				detail += to_string(endPos.nLine);
				detail += ":";
				detail += to_string(endPos.nCol);
				detail += ", charIdx = ";
				detail += to_string( static_cast<int>(endPos.nCharIdx) );
				detail += ", ";
				detail += (curTokenType 
							+ SC_LEFT + pToken->getDetail() + SC_RIGHT 
							+ string(" Can't be pushed at the 1st position"));

				e.setDetail( detail );
				throw e;
			}
		}
	}

	// tp == E_TOKEN_BLANK || tp == E_TOKEN_COMMENT_TYPE  is always valid no matter  previous token is which type
	int ret = (curTp == E_TOKEN_SEMICOLON ? 1 : 0);
	return ret;
}



TokenInfo* TokenListAnalyzer::getPreviousToken(bool needSkipBlankComment, int* pHasSkipBlankComment, bool needBack2LastButOne)
{
	// init  flag
	if ( pHasSkipBlankComment != nullptr ) {
		*pHasSkipBlankComment = 0;
	}

	if ( m_tokenList.empty() ) {
		return nullptr;
	}

	TokenInfo* pRetTokenInfo = nullptr;

	auto r_it = m_tokenList.rbegin();
	if ( needBack2LastButOne ) {
		++r_it;
	}

	for( ; r_it != m_tokenList.rend();  )
	{
		auto pTokenInfo = *r_it;
		if ( pTokenInfo != nullptr ) {
			auto tp = pTokenInfo->getType();
			if ( needSkipBlankComment && (tp == E_TOKEN_BLANK || tp == E_TOKEN_COMMENT_TYPE ) ) {
				++r_it;
				// set flag
				if ( pHasSkipBlankComment != nullptr ) {
					*pHasSkipBlankComment = 1;
				}
			} else {
				pRetTokenInfo = pTokenInfo;
				break;
			}
		} else {
			++r_it;
		}
	}

	return pRetTokenInfo;
}


void TokenListAnalyzer::initBanPickCfg()
{
	m_banPickCfgMap.clear();

	//
	// E_TOKEN_BLANK , E_TOKEN_COMMENT_TYPE are always valid after any token type no matter skip Blank/Comment or Not-Skip Blank/Comment
	//
	pair< unordered_map<unsigned int,unsigned int>::iterator,bool>  retpr;

	//
	// E_TOKEN_INTEGER_NUMBER
	//
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_INTEGER_NUMBER) , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_DECIMAL_NUMBER) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_FLOAT_NUMBER)   , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_ADD)         , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_ADD) Failed " << endl; }
     retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_POSITIVE)    , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_POSITIVE) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_MINUS)       , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_MINUS) Failed " << endl; }
     retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_NEGATIVE)    , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_MULTIPLY)    , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_DIVIDE)      , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_DIVIDE) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_MOD)         , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_MOD) Failed " << endl; }

    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_AND)     , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_AND) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_OR)      , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_OR) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_XOR)     , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_NOT)     , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_LEFT_SHIFT) , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; } // e.g.   a = (3 + 4);
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_VARIBLE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_VARIBLE) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_SEMICOLON), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_INTEGER_NUMBER, E_TOKEN_SEMICOLON) Failed " << endl; }

	//
	// E_TOKEN_FLOAT_NUMBER
	//
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_INTEGER_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_FLOAT_NUMBER),   genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_ADD)         , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_ADD) Failed " << endl; }
     retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_POSITIVE)    , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_POSITIVE) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_MINUS)       , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_MINUS) Failed " << endl; }
     retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_NEGATIVE)    , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_MULTIPLY)    , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_DIVIDE)      , genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_DIVIDE) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_MOD)         , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_MOD) Failed " << endl; }

    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_AND)     , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_AND) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_OR)      , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_OR) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_XOR)     , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_NOT)     , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_LEFT_SHIFT) , genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; } 
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_VARIBLE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_VARIBLE) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_SEMICOLON), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_FLOAT_NUMBER, E_TOKEN_SEMICOLON) Failed " << endl; }


	//
	// E_TOKEN_OPERATOR
	//
	 //
	 // '+' : ADD
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_FLOAT_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_ADD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_POSITIVE), genBanPickMask(1,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_MINUS), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ADD, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ADD, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // '+' : Positive
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_FLOAT_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_ADD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_POSITIVE), genBanPickMask(1,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_MINUS), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_POSITIVE, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // '-' : Minus
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_FLOAT_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_MINUS), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MINUS, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MINUS, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // '-' : Negative
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_FLOAT_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_ADD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_MINUS), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_NEGATIVE, E_TOKEN_SEMICOLON) Failed " << endl; }


	 //
	 // '*'
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_FLOAT_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MULTIPLY, E_TOKEN_SEMICOLON) Failed " << endl; }


	 //
	 // '/'
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_FLOAT_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_DIVIDE, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // '%'
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_FLOAT_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_MOD, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_MOD, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // '&'
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_FLOAT_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_AND, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // '|'
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_FLOAT_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_OR, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // '^'
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_FLOAT_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_XOR, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // '~'
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_FLOAT_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_NOT, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // '<<'
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_FLOAT_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_LEFT_SHIFT, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // '>>'
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_FLOAT_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_BIT_RIGHT_SHIFT, E_TOKEN_SEMICOLON) Failed " << endl; }


	 //
	 // '='
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_FLOAT_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_ADD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_MINUS), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_ASSIGNMENT, E_TOKEN_SEMICOLON) Failed " << endl; }


	 //
	 // '('
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_FLOAT_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_ADD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_MINUS), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_SEMICOLON), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_SEMICOLON) Failed " << endl; }

	 //
	 // ')'
	 //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_INTEGER_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_FLOAT_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_POSITIVE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_NEGATIVE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_MULTIPLY), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_DIVIDE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_MOD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_AND), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_OR), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_XOR), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_NOT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(1,1) ) ); /* e.g. (a)=3; */ if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; } // e.g.   (a)=3;
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_VARIBLE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_OP_CLOSE_PARENTHESES, E_TOKEN_SEMICOLON), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_OP_OPEN_PARENTHESES, E_TOKEN_SEMICOLON) Failed " << endl; }


   //
   // Varible
   //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_INTEGER_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_FLOAT_NUMBER), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_ADD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_POSITIVE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_MINUS), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_NEGATIVE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_MULTIPLY), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_DIVIDE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_MOD), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_AND), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_OR), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_XOR), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_NOT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_VARIBLE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_VARIBLE, E_TOKEN_SEMICOLON), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_VARIBLE, E_TOKEN_SEMICOLON) Failed " << endl; }

   //
   // Semicolon
   //
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_INTEGER_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_INTEGER_NUMBER) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_FLOAT_NUMBER), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_FLOAT_NUMBER) Failed " << endl; }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Operator Begin
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_ADD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_ADD) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_POSITIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_POSITIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_MINUS), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_MINUS) Failed " << endl; }
    retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_NEGATIVE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_NEGATIVE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_MULTIPLY), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_MULTIPLY) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_DIVIDE), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_DIVIDE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_MOD), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_MOD) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_AND), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_AND) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_OR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_OR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_XOR), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_XOR) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_NOT), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_NOT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_LEFT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_LEFT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_RIGHT_SHIFT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_BIT_RIGHT_SHIFT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_ASSIGNMENT), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_ASSIGNMENT) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_OPEN_PARENTHESES), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_OPEN_PARENTHESES) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_CLOSE_PARENTHESES), genBanPickMask(0,0) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_OP_CLOSE_PARENTHESES) Failed " << endl; }
      // Operator End
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_VARIBLE), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_VARIBLE) Failed " << endl; }
   retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SEMICOLON, E_TOKEN_SEMICOLON), genBanPickMask(1,1) ) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SEMICOLON, E_TOKEN_SEMICOLON) Failed " << endl; }

}

unsigned int TokenListAnalyzer::genFlag(E_TokenType frontToken, E_TokenType genToken)
{
	/*
	     | front Token | Gen Token |
	 ----------------------------------------------------------------------
	     |  xxxx xxxx  | xxxx xxxx |
	*/
	unsigned int ret = 0U;

	ret |= static_cast<unsigned int>(   0xFFU & genToken);
	ret |= (static_cast<unsigned int>( ((0xFFU & frontToken) << 8U) ) );

	return ret;
}

unsigned int TokenListAnalyzer::genBanPickMask(unsigned int noContinuedFlag, unsigned int continuedFlag)
{
	unsigned int retMask = 0;
	retMask |= (noContinuedFlag << 1);
	retMask |= continuedFlag;
	return retMask;
}

void TokenListAnalyzer::judgeTokenIsPositiveOrNegativeAndReset(TokenInfo* pToken)
{
	if ( pToken == nullptr ) {
		return;
	}

	int skipFlag = 0;
	TokenInfo* pPreviousInfo = getPreviousToken(true,&skipFlag, false);

	//
	// pPreviousInfo != nullptr
	//
	auto tp = pToken->getType();
	auto subTp = pToken->getSubType();
	if (       tp == E_TOKEN_OPERATOR 
		 && (subTp == E_TOKEN_OP_ADD || subTp == E_TOKEN_OP_MINUS)
	)
	{
		if ( pPreviousInfo == nullptr ) {
			pToken->resetSubType( (subTp == E_TOKEN_OP_ADD) ? E_TOKEN_OP_POSITIVE : E_TOKEN_OP_NEGATIVE );
		} else {
			auto previousTp = pPreviousInfo->getType();
			if ( previousTp == E_TOKEN_INTEGER_NUMBER || previousTp == E_TOKEN_FLOAT_NUMBER || previousTp == E_TOKEN_VARIBLE ) {
				// keep it as  add/minus   rather than   positive/negative
			} else {
				if ( previousTp == E_TOKEN_OPERATOR ) {
					auto previousSubTp = pPreviousInfo->getSubType();
					/*
					switch ( previousSubTp )
					{
					case E_TOKEN_OP_ADD:
					case E_TOKEN_OP_POSITIVE:
					case E_TOKEN_OP_MINUS:
					case E_TOKEN_OP_NEGATIVE:
					case E_TOKEN_OP_MULTIPLY:
					case E_TOKEN_OP_DIVIDE:
					case E_TOKEN_OP_MOD:
					case E_TOKEN_OP_BIT_AND:
					case E_TOKEN_OP_BIT_OR:
					case E_TOKEN_OP_BIT_XOR:
					case E_TOKEN_OP_BIT_NOT:
					case E_TOKEN_OP_BIT_LEFT_SHIFT:
					case E_TOKEN_OP_BIT_RIGHT_SHIFT:
					case E_TOKEN_OP_OPEN_PARENTHESES:
					case E_TOKEN_OP_OPEN_PARENTHESES:
						pToken->resetSubType( (subTp == E_TOKEN_OP_ADD) ? E_TOKEN_OP_POSITIVE : E_TOKEN_OP_NEGATIVE );
						break;
					}
					*/

					if ( previousSubTp != E_TOKEN_OP_CLOSE_PARENTHESES ) {
						pToken->resetSubType( (subTp == E_TOKEN_OP_ADD) ? E_TOKEN_OP_POSITIVE : E_TOKEN_OP_NEGATIVE );
					}
				} else {
					// previousTp == E_TOKEN_SEMICOLON
					pToken->resetSubType( (subTp == E_TOKEN_OP_ADD) ? E_TOKEN_OP_POSITIVE : E_TOKEN_OP_NEGATIVE );
				}
			}
		}

	}
}



string  TokenListAnalyzer::getTokenName(E_TokenType tp)
{
	string ret;
	switch( tp )
	{
	case E_TOKEN_UNKNOWN:
		ret = "E_TOKEN_UNKNOWN";
		break;
	case E_TOKEN_BLANK:
		ret = "E_TOKEN_BLANK";
		break;
	case E_TOKEN_COMMENT_TYPE:
		ret = "E_TOKEN_COMMENT_TYPE";
		break;
	case E_TOKEN_INTEGER_NUMBER:
		ret = "E_TOKEN_INTEGER_NUMBER";
		break;
	case E_TOKEN_FLOAT_NUMBER:
		ret = "E_TOKEN_FLOAT_NUMBER";
		break;
	case E_TOKEN_VARIBLE:
		ret = "E_TOKEN_VARIBLE";
		break;
	case E_TOKEN_OP_OPEN_PARENTHESES: // = 8,  // (
		ret = "'('";
		break;
	case E_TOKEN_OP_CLOSE_PARENTHESES: // = 9, // )
		ret = "')'";
		break;
	case E_TOKEN_OP_ADD: // = 10,       // +
		ret = "'+'";
		break;
	case E_TOKEN_OP_POSITIVE: // = 11,     // +3
		ret = "'+' positive ";
		break;
	case E_TOKEN_OP_MINUS: // = 12,             // -
		ret = "'-'";
		break;
	case E_TOKEN_OP_NEGATIVE: // = 13,     // -3
		ret = "'-' negative";
		break;
	case E_TOKEN_OP_MULTIPLY: // = 14,          // *
		ret = "'*'";
		break;
	case E_TOKEN_OP_DIVIDE: // = 15,            // /
		ret = "'/'";
		break;
	case E_TOKEN_OP_MOD: // = 16,               // %
		ret = "'%'";
		break;
	case E_TOKEN_OP_BIT_AND: // = 17,           // &
		ret = "'&'";
		break;
	case E_TOKEN_OP_BIT_OR: // = 18,            // |
		ret = "'|'";
		break;
	case E_TOKEN_OP_BIT_XOR: // = 19,           // ^
		ret = "'^'";
		break;
	case E_TOKEN_OP_BIT_NOT: // = 20,           // ~
		ret = "'~'";
		break;
	case E_TOKEN_OP_BIT_LEFT_SHIFT: // = 21,    // <<
		ret = "'<<'";
		break;
	case E_TOKEN_OP_BIT_RIGHT_SHIFT: // = 22,   // >>
		ret = "'>>'";
		break;
	case E_TOKEN_OP_ASSIGNMENT: // = 23,        // =
		ret = "'='";
		break;
	default:
		ret = "In switch-case default part :  E_TOKEN_UNKNOWN";
		break;
	}

	return ret;
}



int  TokenListAnalyzer::evaluateExp()
{
	// TODO : ???
	return 0;
}

void TokenListAnalyzer::clearEvaluateList()
{
	m_evaluateSuffixExpression.clear();
	m_operatorStack.clear();
}


bool TokenListAnalyzer::isOperatorType(E_TokenType tp)
{
	int curIdx = static_cast<int>(tp);
	int begIdx = static_cast<int>(E_TOKEN_OP_BEGIN);
	int endIdx = static_cast<int>(E_TOKEN_OP_END);

	return (curIdx > begIdx   &&   curIdx <= endIdx);
}

bool TokenListAnalyzer::hasMathedOpenParentheseBefore()
{
	auto foundBefore = false;
	for( auto rit = m_operatorStack.crbegin(); rit != m_operatorStack.crend(); ++rit )
	{
		auto pOperatorInfo = rit->second;
		if ( pOperatorInfo != nullptr && ( pOperatorInfo->getChar() == ')' ) ) {
			foundBefore = true;
			break;
		}
	}
	return foundBefore;
}


void TokenListAnalyzer::tryPopPreviousOperatorLowerPriority(TokenInfo* pCurrentToken)
{
	auto pCurrentOperInfo = token2Op(pCurrentToken);
	if ( m_operatorStack.empty() ) {
		m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
		return;
	}

	// operator stack is <Not> empty()
	auto pr = m_operatorStack.back();
	auto pLastToken = pr.first;
	auto pLastOperInfo = pr.second;

	auto lastOperPriority = pLastOperInfo->getPriority();
	auto currentOpPriority = pCurrentOperInfo->getPriority();
	auto currentOpIsLeft2Right = pCurrentOperInfo->isLeft2Right();

	if ( pCurrentToken->getSubType() == E_TOKEN_OP_OPEN_PARENTHESES ) {
		// Special operator '('
		m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
	} else {
		// != '('
		if ( lastOperPriority > currentOpPriority ) {
			m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
		} else if ( lastOperPriority == currentOpPriority ) {
			if ( currentOpIsLeft2Right ) {
				m_evaluateSuffixExpression.push_back( pLastToken );

				m_operatorStack.pop_back();
				m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
			} else {
				m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
			}
		} else {
			// lastOperPriority < currentOpPriority 
			if ( pLastToken->getSubType() == E_TOKEN_OP_OPEN_PARENTHESES ) {
				m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
			} else {
				for ( auto rit = m_operatorStack.rbegin(); rit != m_operatorStack.rend(); )
				{
					auto pritToken = rit->first;
					auto pOperBaseInfo = rit->second;
					if ( pritToken->getSubType() == E_TOKEN_OP_OPEN_PARENTHESES ) {
						// Special , core core core   : '('
						m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
						break;
					} else {
						auto lastPri = pOperBaseInfo->getPriority();
						if ( lastPri < currentOpPriority ) {
							// Do Not Apply  
							// //               ++rit; 
							// again
							// move stack top into Suffix-Exp-List
							m_operatorStack.erase( (++rit).base() );
							m_evaluateSuffixExpression.push_back( pritToken );
						} else if ( lastPri == currentOpPriority ) {
							///////////////////////////////////////////////////////////
							//
							if ( currentOpIsLeft2Right ) {
								m_operatorStack.erase( (++rit).base() );
								m_evaluateSuffixExpression.push_back( pritToken );
							} else {
								m_evaluateSuffixExpression.push_back( pritToken );
							}
							//
							///////////////////////////////////////////////////////////
						} else {
							// lastPri > currentOpPriority
							break;
						}
					}
				}
			}
		}
	}

}

OperatorBaseInfo* TokenListAnalyzer::token2Op(TokenInfo* pToken)
{
	auto tp = pToken->getType();
	if ( tp != E_TOKEN_OPERATOR ) {
		cout << "tp != E_TOKEN_OPERATOR , tp = " << getTokenName(tp) << endl;
		assert(false);
	}

	CharBaseInfo* charInfo = nullptr;
	switch( tp )
	{
	case E_TOKEN_OP_OPEN_PARENTHESES: // = 8,  // (
		charInfo = CharUtil::getCharBaseInfo('(');
		break;
	case E_TOKEN_OP_CLOSE_PARENTHESES: // = 9, // )
		charInfo = CharUtil::getCharBaseInfo(')');
		break;
	case E_TOKEN_OP_ADD: // = 10,       // +
		charInfo = CharUtil::getCharBaseInfo('+');
		break;
	case E_TOKEN_OP_POSITIVE: // = 11,     // +3
		charInfo = CharUtil::getPositiveCharInfo();
		break;
	case E_TOKEN_OP_MINUS: // = 12,             // -
		charInfo = CharUtil::getCharBaseInfo('-');
		break;
	case E_TOKEN_OP_NEGATIVE: // = 13,     // -3
		charInfo = CharUtil::getNegativeCharInfo();
		break;
	case E_TOKEN_OP_MULTIPLY: // = 14,          // *
		charInfo = CharUtil::getCharBaseInfo('*');
		break;
	case E_TOKEN_OP_DIVIDE: // = 15,            // /
		charInfo = CharUtil::getCharBaseInfo('/');
		break;
	case E_TOKEN_OP_MOD: // = 16,               // %
		charInfo = CharUtil::getCharBaseInfo('%');
		break;
	case E_TOKEN_OP_BIT_AND: // = 17,           // &
		charInfo = CharUtil::getCharBaseInfo('&');
		break;
	case E_TOKEN_OP_BIT_OR: // = 18,            // |
		charInfo = CharUtil::getCharBaseInfo('|');
		break;
	case E_TOKEN_OP_BIT_XOR: // = 19,           // ^
		charInfo = CharUtil::getCharBaseInfo('^');
		break;
	case E_TOKEN_OP_BIT_NOT: // = 20,           // ~
		charInfo = CharUtil::getCharBaseInfo('~');
		break;
	case E_TOKEN_OP_BIT_LEFT_SHIFT: // = 21,    // <<
		charInfo = CharUtil::getCharBaseInfo('<');
		break;
	case E_TOKEN_OP_BIT_RIGHT_SHIFT: // = 22,   // >>
		charInfo = CharUtil::getCharBaseInfo('>');
		break;
	case E_TOKEN_OP_ASSIGNMENT: // = 23,        // =
		charInfo = CharUtil::getCharBaseInfo('=');
		break;
	default:
		break;
	}

	OperatorBaseInfo* pRetInfo = nullptr;
	if ( charInfo != nullptr ) {
		pRetInfo = charInfo->getOpInfo();
	}
	return pRetInfo;
}



void TokenListAnalyzer::popUtilOpenParenthese()
{
	//
	// pop ')' which is pushed just now
	//

	auto lastElement = m_operatorStack.back();
	if ( lastElement.first != nullptr &&  (lastElement.first)->getSubType() == E_TOKEN_OP_OPEN_PARENTHESES ) {
		// TODO : throw   empty content inside   ()
	} else {
		for( auto rit = m_operatorStack.crbegin(); rit != m_operatorStack.crend();  )
		{
			auto pOpToken  = rit->first;
			if ( pOpToken != nullptr ) {
				if ( pOpToken->getSubType() == E_TOKEN_OP_OPEN_PARENTHESES ) {
					// pop '('
					m_operatorStack.erase( (++rit).base() );
					break;
				} else {
					m_operatorStack.erase( (++rit).base() );
					m_evaluateSuffixExpression.push_back( pOpToken );
				}
			} else {
				++rit;
			}
		}
	}

}

