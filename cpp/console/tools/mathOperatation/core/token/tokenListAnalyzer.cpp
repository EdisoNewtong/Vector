#include <cassert>
#include <iostream>
#include <iterator>
#include "parserOptions.h"
#include "tokenListAnalyzer.h"
#include "parserException.h"
#include "enum2name.h"
using namespace std;

TokenListAnalyzer::TokenListAnalyzer()
	: m_tokenList()
	, m_banPickCfgMap()
	, m_evaluateSuffixExpression()
	, m_operatorStack()
	, m_lastSemicolonPosition(0)
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
	// static const string SC_LEFT(" ( ");
	// static const string SC_RIGHT(" ) ");
	static const string SC_BLANK("  ");

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
	auto curTokenType = Enum2Name::getTokenName(curTp);

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
                auto previousOpenParentheseToken = hasOpenParentheseBefore();
				if ( previousOpenParentheseToken == nullptr ) {
                    ParserException e(E_TOKEN_CLOSE_PARENTHESES_MISMATCHED);
					string detail;
					detail += endPos.getPosStr();
                    e.setDetail( detail );
                    throw e;
				} else {
					// pop previous element until '('
					popUtilOpenParenthese();
				}
			} else {
				auto pOperInfo = token2Op(pToken);
				if ( pOperInfo == nullptr ) {
					cout << "[ERROR] : Can't found OperatorBaseInfo about " << Enum2Name::getTokenName(pToken->getSubType()) << endl;
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
			auto previousTokenType = Enum2Name::getTokenName(previousType);

			auto key = genFlag(previousType, curTp);
			auto foundIt = m_banPickCfgMap.find(key);
			if ( foundIt != m_banPickCfgMap.end() ) {
				auto banPickFlag = foundIt->second;

				if ( banPickFlag == 0 ) {
					// current operator type is not allowed  ( at ban/pick list)
					string detail;
					detail += endPos.getPosStr();
					detail += ", ";
					detail +=   (previousTokenType 
								  + SC_BLANK + previousValidToken->getDetail()
								  + string(" Can't located in front of Type   ") + curTokenType + string(" ")
								  + pToken->getDetail());

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
						detail += endPos.getPosStr();
						detail += ", ";
						detail +=  (previousTokenType 
									 + SC_BLANK + previousValidToken->getDetail() 
									 + string(" Can't located <No-Skip> in front of Type   ") + curTokenType
									 + pToken->getDetail() );

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
				detail += endPos.getPosStr();
				detail += ", ";
				detail += (curTokenType 
							+ pToken->getDetail() 
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
	TokenInfo* pPreviousInfo = getPreviousToken(true, &skipFlag, false);

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



void TokenListAnalyzer::expValidCheck()
{
    auto pOpenParentheseToken  = hasOpenParentheseBefore();
	if ( pOpenParentheseToken != nullptr ) {
        string detail;
        ParserException e(E_TOKEN_OPEN_PARENTHESES_MISMATCHED);
	    const auto& endPos = pOpenParentheseToken->getEndPos();
        detail += endPos.getPosStr();
        e.setDetail( detail );
        throw e;
	}

	// pop rest   operator
	for( auto rit = m_operatorStack.crbegin(); rit != m_operatorStack.crend(); ++rit )
	{
		auto pToken = rit->first;
		m_evaluateSuffixExpression.push_back(pToken);
	}
	m_operatorStack.clear();
}


int  TokenListAnalyzer::evaluateExp()
{
	auto flag = ParserOptions::getFlag();
	expValidCheck();

	// auto it_beg =  m_tokenList.begin();
	// std::advance(it_beg, m_lastSemicolonPosition);

	if ( flag == 1 ) {
		auto idx = 1;
		for( auto it =  m_evaluateSuffixExpression.begin(); it!= m_evaluateSuffixExpression.end(); ++it, ++idx )
		{
			auto pToken = *it;
			if ( pToken != nullptr ) {
				const auto& begPos = pToken->getBeginPos();
				const auto& endPos = pToken->getEndPos();
				cout << "[DEBUG] " << idx << ". '" << pToken->getDetail() << "'"
					               << ", " <<  begPos.getPosStr(0)
								   << " ~ "<<  endPos.getPosStr(0);

				auto subType = pToken->getSubType();
				if ( subType == E_TOKEN_OP_POSITIVE ) {
					cout << " => '+' (Positive)";
				} else if ( subType == E_TOKEN_OP_NEGATIVE ) {
					cout << " => '-' (Negative)";
				} else if ( subType == E_TOKEN_OP_ADD ) {
					cout << " => '+' (Add)";
				} else if ( subType == E_TOKEN_OP_MINUS ) {
					cout << " => '-' (Minus)";
				} else {
                    cout << Enum2Name::getTokenName(subType) << " ";
                }
				cout << endl;
			} else {
				cout << "[ERROR] "<< idx << ". exp.element == nullptr" << endl;
			}
		}
	}
	
	// TODO :  Core Core Core , Key Code
    int idx = 0;
    for( auto it =  m_evaluateSuffixExpression.begin(); it!= m_evaluateSuffixExpression.end(); ++idx )
    {
        auto bAdvanceFlag = false;
        auto pToken = *it;
        if ( pToken != nullptr ) {
            auto type = pToken->getType();
            if ( type == E_TOKEN_OPERATOR ) {
	            auto pCurrentOperInfo = token2Op(pToken);
                if ( pCurrentOperInfo != nullptr ) {
                    auto isBinaryOp = pCurrentOperInfo->isBinaryOp();
                    if ( isBinaryOp ) {
                        //
                        // Binary operator
                        //
                        assert( idx >=2 );
                        auto itPrevious1 = prev(it);
                        auto itPrevious2 = prev(it,2);
                        TokenInfo* leftOperand = *itPrevious2;
                        TokenInfo* rightOperand = *itPrevious1;

                        // remove operator , right operand , left operand
                        ++it;
                        it = m_evaluateSuffixExpression.erase(itPrevious2, it);

                        if ( leftOperand != nullptr && rightOperand != nullptr ) {
                            auto binOpRet = generateTmpMiddleBinaryToken(leftOperand, pToken, rightOperand);
                            it = m_evaluateSuffixExpression.insert(it, binOpRet);
                            ++it;
                        }
                    } else {
                        //
                        // Unary operator
                        //
                        assert( idx >=1 );
                        auto itPrevious1 = prev(it);
                        TokenInfo* singleOperand = *itPrevious1;

                        // remove operator , right operand
                        ++it;
                        it = m_evaluateSuffixExpression.erase(itPrevious1, it);

                        if ( singleOperand != nullptr ) {
                            auto unaryOpRet = generateTmpMiddleUnaryToken( pToken, singleOperand);
                            it = m_evaluateSuffixExpression.insert(it, unaryOpRet);
                            ++it;
                        }
                    }

                    bAdvanceFlag = true;
                }
            }
        }

        if ( !bAdvanceFlag ) {
            ++it;
        }
    }

	/*

	     0   1   2   3     4    5   6    7    8   9    10    11
	     a   =   b   ;     b    =   c    ;    d   =    e      ;
		             4                   8                   12

		 1. pos = 4;
		 2. pos = 8;
	     
	*/
	// After Done , move iterator 
	// m_lastSemicolonPosition = m_tokenList.size() - m_lastSemicolonPosition;
	// m_lastSemicolonPosition = static_cast<int>( m_tokenList.size() );


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

//
// search the previous '(' in reverse order
//
TokenInfo* TokenListAnalyzer::hasOpenParentheseBefore()
{
	TokenInfo* foundBefore = nullptr;
	for( auto rit = m_operatorStack.crbegin(); rit != m_operatorStack.crend(); ++rit )
	{
        TokenInfo* tokenInfo = rit->first;
		auto pOperatorInfo = rit->second;
		if ( pOperatorInfo != nullptr && ( pOperatorInfo->getChar() == '(' ) ) {
			foundBefore = tokenInfo;
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
		//   currentOp  !=  '('
		if ( currentOpPriority < lastOperPriority ) {
			m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
		} else if ( currentOpPriority == lastOperPriority ) {
			if ( currentOpIsLeft2Right ) {
				m_evaluateSuffixExpression.push_back( pLastToken );

				m_operatorStack.pop_back();
				m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
			} else {
				// from Right 2 Left
				m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
			}
		} else {
			// currentOpPriority > lastOperPriority 
			if ( pLastToken->getSubType() == E_TOKEN_OP_OPEN_PARENTHESES ) {
				m_operatorStack.push_back( make_pair(pCurrentToken, pCurrentOperInfo) );
			} else {
				//
				// Try to find the 1st operator whose priority is 
				//
				for ( auto rit = m_operatorStack.rbegin(); rit != m_operatorStack.rend(); )
				{
					auto pritToken = rit->first;
					auto pOperBaseInfo = rit->second;
					if ( pritToken->getSubType() == E_TOKEN_OP_OPEN_PARENTHESES ) {
						// Special , core core core   : '(' 
						//     =>  then break
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
								m_evaluateSuffixExpression.push_back( pritToken ); // push erased element from  m_operatorStack
							} else {
								// from Right 2 Left
								break;
							}
							//
							///////////////////////////////////////////////////////////
						} else {
							// lastPri > currentOpPriority
							break;
						}
					}
				}

				m_operatorStack.push_back( make_pair(pCurrentToken,pCurrentOperInfo)  );
			}
		}
	}

}

OperatorBaseInfo* TokenListAnalyzer::token2Op(TokenInfo* pToken)
{
	auto tp = pToken->getType();
	if ( tp != E_TOKEN_OPERATOR ) {
		cout << "tp != E_TOKEN_OPERATOR , tp = " << Enum2Name::getTokenName(tp) << endl;
		assert(false);
	}

	auto subTp = pToken->getSubType();

	CharBaseInfo* charInfo = nullptr;
	switch( subTp )
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

TokenInfo* TokenListAnalyzer::generateTmpMiddleBinaryToken(TokenInfo* left, TokenInfo* op, TokenInfo* right)
{
	TokenInfo* tmpMiddleToken = nullptr;
	auto subTp = op->getSubType();

	switch( subTp )
	{
	case E_TOKEN_OP_ADD:
		tmpMiddleToken = doAdd(left,right);
		break;
	case E_TOKEN_OP_MINUS:
		tmpMiddleToken = doMinus(left,right);
		break;
	case E_TOKEN_OP_MULTIPLY:
		tmpMiddleToken = doMultiply(left,right);
		break;
	case E_TOKEN_OP_DIVIDE:
		tmpMiddleToken = doDivide(left,right);
		break;
	case E_TOKEN_OP_MOD:
		tmpMiddleToken = doMod(left,right);
		break;
	case E_TOKEN_OP_BIT_AND:
		tmpMiddleToken = doBitAnd(left,right);
		break;
	case E_TOKEN_OP_BIT_OR:
		tmpMiddleToken = doBitOr(left,right);
		break;
	case E_TOKEN_OP_BIT_XOR:
		tmpMiddleToken = doBitXor(left,right);
		break;
	case E_TOKEN_OP_BIT_LEFT_SHIFT:
		tmpMiddleToken = doBitLeftShift(left,right);
		break;
	case E_TOKEN_OP_BIT_RIGHT_SHIFT:
		tmpMiddleToken = doBitRightShift(left,right);
		break;
	case E_TOKEN_OP_ASSIGNMENT:
		tmpMiddleToken = doAssignment(left,right);
		break;
	default:
		break;
	}

    return tmpMiddleToken;

}

TokenInfo* TokenListAnalyzer::generateTmpMiddleUnaryToken(TokenInfo* op, TokenInfo* right)
{
	TokenInfo* tmpMiddleToken = nullptr;
	auto subTp = op->getSubType();

	switch( subTp )
	{
	case E_TOKEN_OP_POSITIVE:
		tmpMiddleToken = doPositive(right);
		break;
	case E_TOKEN_OP_NEGATIVE:
		tmpMiddleToken = doNegative(right);
		break;
	case E_TOKEN_OP_BIT_NOT:
		tmpMiddleToken = doBitNot(right);
		break;
	default:
		break;
	}

    return tmpMiddleToken;

}


void TokenListAnalyzer::decideFinalExpDataTypeFor2Operands(TokenInfo* left, TokenInfo* op, TokenInfo* right)
{
    // auto opType = op->getType();
    // switch( opType ) 
    // {
    // case 
    // }
}


/*
     
*/
// a+b
TokenInfo* TokenListAnalyzer::doAdd(TokenInfo* left,  TokenInfo* right)
{
    /*
        E_TokenType getType();
        E_TokenType getSubType();

    auto lftDt = left->getDataType();
    auto rgtDt = right->getDataType();
    */ 

    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// a-b
TokenInfo* TokenListAnalyzer::doMinus(TokenInfo* left,  TokenInfo* right)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// a*b
TokenInfo* TokenListAnalyzer::doMultiply(TokenInfo* left,  TokenInfo* right)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// a/b
TokenInfo* TokenListAnalyzer::doDivide(TokenInfo* left,  TokenInfo* right)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// a%b
TokenInfo* TokenListAnalyzer::doMod(TokenInfo* left,  TokenInfo* right)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// a&b
TokenInfo* TokenListAnalyzer::doBitAnd(TokenInfo* left,  TokenInfo* right)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// a|b
TokenInfo* TokenListAnalyzer::doBitOr(TokenInfo* left,  TokenInfo* right)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// a^b
TokenInfo* TokenListAnalyzer::doBitXor(TokenInfo* left, TokenInfo* right)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// a<<b
TokenInfo*  TokenListAnalyzer::doBitLeftShift(TokenInfo* left,  TokenInfo* right)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// a>>b
TokenInfo* TokenListAnalyzer::doBitRightShift(TokenInfo* left,  TokenInfo* right)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// a=b
TokenInfo* TokenListAnalyzer::doAssignment(TokenInfo* left,  TokenInfo* right)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}




void TokenListAnalyzer::decideFinalExpDataTypeFor1Operands(TokenInfo* right)
{
}


// (+a)
TokenInfo* TokenListAnalyzer::doPositive(TokenInfo* token)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// (-a)
TokenInfo* TokenListAnalyzer::doNegative(TokenInfo* token)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}

// ~a
TokenInfo* TokenListAnalyzer::doBitNot(TokenInfo* token)
{
    TokenInfo* tmpMiddleToken = nullptr;
    return tmpMiddleToken;
}




