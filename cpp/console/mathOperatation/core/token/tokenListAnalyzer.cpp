#include <cassert>
#include <iostream>
#include "tokenListAnalyzer.h"
#include "parserException.h"
using namespace std;

TokenListAnalyzer::TokenListAnalyzer()
	: m_tokenList()
	, m_banPickCfgMap()
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


// 
// 
// Next Character Possibility <After> a Specific Token
// 
// 1. Blank Type    <== || Before ||
//----------------------------------------
//  ||   After      ||
//----------------------------------------
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      | YES |
//    4. Float         | YES |
//    5. Operator      | YES |
//    6. Varible       | YES |
//    7. Semicolon     | YES |   a=b+c   ;

// 
// 2. Comment Type    <== || Before ||
//----------------------------------------
// ||    After      ||
//----------------------------------------
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      | YES |
//    4. Float         | YES |
//    5. Operator      | YES |
//    6. Varible       | YES |
//    7. Semicolon     | YES |   a=b+c/*Comment Line*/;

// 
// 3. Interger    <== || Before ||
//----------------------------------------
// ||    After      ||
//----------------------------------------
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      |  NO |
//    4. Float         |  NO |
//    5. Operator      |PART |   Not Avalible List :      ~  ,  ( ,
//    6. Varible       |  NO |
//    7. Semicolon     | YES |   a = 10 + 3;

//
// 4. Float Type    <== || Before ||
//----------------------------------------
// ||    After      ||
//----------------------------------------
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      |  NO |
//    4. Float         |  NO |
//    5. Operator      |PART |   Not Avalible List  : %  &  | ^  ~  << >>   (   =
//    6. Varible       |  NO |
//    7. Semicolon     | YES |   a = 10 + 3.1f;
// 

// 
// 5. Operator Type    <== || Before ||
//----------------------------------------
// ||    After      ||
//----------------------------------------
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      | YES |
//    4. Float         | YES |
//    5. Operator      | TBD |   
//       + :  
//            +          |  NO |  ++ is not supported
//            -          | YES |  +-  e.g.    int a = 10+-3;   // int a = 10 + (-3) = 7;
//            *          |  NO |
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |
//            |          |  NO |
//            ^          |  NO |
//            ~          | YES |  +~  e.g.    int a = 1+~2;
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | "+=" is not supported
// 
//       - :  
//            +          | YES |  -+  e.g.    int a = 10-+3;   // int a = 10 - (+3) = 7;
//            -          |  NO |  -- is not supported
//            *          |  NO |
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |
//            |          |  NO |
//            ^          |  NO |
//            ~          | YES |  -~  e.g.    int a = 1-~2;
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | "-=" is not supported
// 
//       * :  
//            +          | YES |  *+  e.g.    int a = 10*+3;   // int a = 30 = 10 * (+3);
//            -          | YES |  *-  e.g.    int a = 10*-3;   // int a = 10 * (-3) = -30;
//            *          |  NO |  
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |
//            |          |  NO |
//            ^          |  NO |
//            ~          | YES |  *~  e.g.    int a = 10*~2;
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | "*=" is not supported
// 
//       / :  
//            +          | YES |  *+  e.g.    int a = 10*+3;   // int a = 30 = 10 * (+3);
//            -          | YES |  *-  e.g.    int a = 10*-3;   // int a = 10 * (-3) = -30;
//            *          | YES |  /* multi-line comment */
//            /          | YES |  // singline line comment 
//            %          |  NO |
//            &          |  NO |
//            |          |  NO |
//            ^          |  NO |
//            ~          | YES |  /~  e.g.    int a = 10/~2;
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | "/=" is not supported
// 
//       % :  
//            +          | YES |  %+  e.g.    int a = 10%+3;   // int a = 10%(+3) = 1;
//            -          | YES |  %-  e.g.    int a = 10%-3;   // int a = 10%(-3) = 1;
//            *          |  NO |  
//            /          |  NO |  
//            %          |  NO |
//            &          |  NO |
//            |          |  NO |
//            ^          |  NO |
//            ~          | YES |  %~  e.g.    int a = 10%~2 = 10 % (~2);
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | "%=" is not supported
// 
// 
//       & :  
//            +          | YES |  &+  e.g.    int a = 10&+3;   // int a = 10 & (+3); 
//            -          | YES |  *-  e.g.    int a = 10&-3;   // int a = 10 & (-3);
//            *          |  NO |  
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |  "&&"  is not supported
//            |          |  NO |  
//            ^          |  NO |  
//            ~          | YES |  &~  e.g.   int a = 10&~2;  // int a = 10 & (~2);
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | "&=" is not supported
// 
// 
//       | :  
//            +          | YES |  |+  e.g.    int a = 10|+3;   // int a = 10 | (+3); 
//            -          | YES |  |-  e.g.    int a = 10|-3;   // int a = 10 | (-3);
//            *          |  NO |  
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |  
//            |          |  NO |  "||" is not supported
//            ^          |  NO |  
//            ~          | YES |  |~  e.g.   int a = 10|~2;  // int a = 10 / (~2);
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | "|=" is not supported
// 
//       ^ :  
//            +          | YES |  ^+  e.g.    int a = 10^+3;   // int a = 10 ^ (+3); 
//            -          | YES |  ^-  e.g.    int a = 10^-3;   // int a = 10 ^ (-3);
//            *          |  NO |  
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |  
//            |          |  NO |  
//            ^          |  NO |  
//            ~          | YES |  ^~  e.g.   int a = 10^~2;  // int a = 10 ^ (~2);
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | "^=" is not supported
// 
//       ~ :  
//            +          | YES |  ~+  e.g.    int a = ~+3;   // int a = 10 ~ (+3); 
//            -          | YES |  ~-  e.g.    int a = ~-3;   // int a = 10 ~ (-3);
//            *          |  NO |  
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |  
//            |          |  NO |  
//            ^          |  NO |  
//            ~          | YES |  ~~  e.g.   int a = ~~2;  // int a = ~( ~2 );
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | "~=" is not supported
// 
// 
//      << :  
//            +          | YES |  <<+  e.g.    int a = 10<<+3;   // int a = 10 << (+3); 
//            -          | YES |  <<-  e.g.    int a = 10<<-2;   // int a = 10 << (-2);  //  警告：左移次数为负 [默认启用]
//            *          |  NO |  
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |  
//            |          |  NO |  
//            ^          |  NO |  
//            ~          | YES |  <<~  e.g.   int a = 10<<~2;  // int a = 10 << (~2);  //  警告：左移次数为负 [默认启用]                    //  int a = 10<<~-2;   OK
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | "<<=" is not supported
// 
//      >> :  
//            +          | YES |  >>+  e.g.    int a = 10>>+3;   // int a = 10 >> (+3); 
//            -          |  NO |  >>-  e.g.    int a = 10>>-2;   // int a = 10 >> (-2);  //  警告 : 右左移次数为负 [默认启用]
//            *          |  NO |  
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |  
//            |          |  NO |  
//            ^          |  NO |  
//            ~          | YES |  >>~  e.g.   int a = 10>>~2;  // int a = 10 >> (~2);  //  警告 : 右移次数为负 [默认启用]                    //  int a = 10>>~-2;   =>   int a = 10 >> (~-2);  OK
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  
//            =          |  NO | ">>=" is not supported
// 
//      (  :  
//            +          | YES |  (+  e.g.     a = (+3+2);
//            -          | YES |  (-  e.g.     a = (-3+2);
//            *          |  NO |  
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |  
//            |          |  NO |  
//            ^          |  NO |  
//            ~          | YES |  (~ e.g.      a = (~2);
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  () is not allowed 
//            =          |  NO |  (=     e.g.   (a)=3;
// 
//      )  :  
//            +          | YES |  )+  e.g.     a = (-a)+3;
//            -          | YES |  (-  e.g.     a = (-a)-3;
//            *          | YES |  
//            /          | YES |
//            %          | YES |
//            &          | YES |  
//            |          | YES |  
//            ^          | YES |  
//            ~          |  NO |  )~  ???
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  () is not allowed 
//            =          | YES |  )=     e.g.   (a)=3;
// 
//      =  :  
//            +          | YES |  =+  e.g.     a =+3;
//            -          | YES |  =-  e.g.     a =-3; 
//            *          |  NO |  
//            /          |  NO |
//            %          |  NO |
//            &          |  NO |  
//            |          |  NO |  
//            ^          |  NO |  
//            ~          | YES |  =~   e.g.  a=~2;
//            <<         |  NO | 
//            >>         |  NO |  
//            (          | YES |  
//            )          |  NO |  =)  is not allowed 
//            =          |  NO |  ==  is not allowed   
// 
//    6. Varible       | YES |
//    7. Semicolon     | YES |   Not Avalible List  : + - * / % & | ^ ~ << >> ( =     |    Only Avalible is :  )     e.g.    a=(b+c);


// 
// 6. Varible Type    <== || Before ||
//----------------------------------------
// ||    After      ||
//----------------------------------------
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      |  NO |
//    4. Float         |  NO |
//    5. Operator      |PART |   Not Avalible List :      ~  ,  ( 
//    6. Varible       |  NO |
//    7. Semicolon     | YES |   
// 

//
// 7. Expression Evaluate Type (;)    <== || Before || 
//----------------------------------------
// ||    After      ||
//----------------------------------------
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      | YES |
//    4. Float         | YES |
//    5. Operator      |PART |   Not Avalible List :  *  /  %  &  | ^  << >>  )  =
//    6. Varible       | YES |
//    7. Semicolon     | YES |   
// 

bool TokenListAnalyzer::checkParserIsValid(E_PaserType tp)
{
	(void)tp;
	return true;
}

void TokenListAnalyzer::pushToken(TokenInfo* pToken)
{
	m_tokenList.push_back( pToken );

	if ( pToken != nullptr ) {
		auto curTp = pToken->getType();
		if ( curTp == E_TOKEN_OPERATOR ) {
			curTp = pToken->getSubType();
		}
		auto curTokenType = getTokenName(curTp);

		if ( !(curTp == E_TOKEN_BLANK || curTp == E_TOKEN_COMMENT_TYPE) ) {
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
						auto detail = previousTokenType + string(" Can't located in front of Type   ") + curTokenType;

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
							auto detail = previousTokenType + string(" Can't located <No-Skip> in front of Type   ") + curTokenType;

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
					// 1st pushed is valid
				} else {
					ParserException e(E_TOKEN_LOGIC_INVALID);
					string detail = curTokenType + " Can't be pushed at the 1st position";
					e.setDetail( detail );
					throw e;
				}

			}
		}

		// tp == E_TOKEN_BLANK || tp == E_TOKEN_COMMENT_TYPE  is always valid no matter  previous token is which type

	} else {
		cout << "[ERROR] : Pushed Token == nullptr" << endl;
		assert(false);
	}
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

