#include <iostream>
#include "tokenListAnalyzer.h"
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
// 1. Blank Type
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      | YES |
//    4. Float         | YES |
//    5. Operator      | YES |
//    6. Varible       | YES |
//    7. Semicolon     | YES |   a=b+c   ;

// 
// 2. Comment Type
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      | YES |
//    4. Float         | YES |
//    5. Operator      | YES |
//    6. Varible       | YES |
//    7. Semicolon     | YES |   a=b+c/*Comment Line*/;

// 
// 3. Interger
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      | NO  |
//    4. Float         | NO  |
//    5. Operator      |PART |   Not Avalible List :      ~  ,  ( , =
//    6. Varible       | NO  |
//    7. Semicolon     | YES |   a = 10 + 3;

//
// 4. Float Type
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      | NO  |
//    4. Float         | NO  |
//    5. Operator      |PART |   Not Avalible List  : %  &  | ^  ~  << >>   (   =
//    6. Varible       | NO  |
//    7. Semicolon     | YES |   a = 10 + 3.1f;
// 

// 
// 5. Operator Type
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      | NO  |
//    4. Float         | NO  |
//    5. Operator      | TBD |   
//       + :  
//            +          | NO  |  ++ is not supported
//            -          | YES |  +-  e.g.    int a = 10+-3;   // int a = 10 + (-3) = 7;
//            *          | NO  |
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |
//            |          | NO  |
//            ^          | NO  |
//            ~          | YES |  +~  e.g.    int a = 1+~2;
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//            =          | NO  | "+=" is not supported
// 
//       - :  
//            +          | YES |  -+  e.g.    int a = 10-+3;   // int a = 10 - (+3) = 7;
//            -          | NO  |  -- is not supported
//            *          | NO  |
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |
//            |          | NO  |
//            ^          | NO  |
//            ~          | YES |  -~  e.g.    int a = 1-~2;
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//           =          | NO  | "-=" is not supported
// 
//       * :  
//            +          | YES |  *+  e.g.    int a = 10*+3;   // int a = 30 = 10 * (+3);
//            -          | YES |  *-  e.g.    int a = 10*-3;   // int a = 10 * (-3) = -30;
//            *          | NO  |  
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |
//            |          | NO  |
//            ^          | NO  |
//            ~          | YES |  *~  e.g.    int a = 10*~2;
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//            =          | NO  | "*=" is not supported
// 
//       / :  
//            +          | YES |  *+  e.g.    int a = 10*+3;   // int a = 30 = 10 * (+3);
//            -          | YES |  *-  e.g.    int a = 10*-3;   // int a = 10 * (-3) = -30;
//            *          | YES |  /* multi-line comment */
//            /          | YES |  // singline line comment 
//            %          | NO  |
//            &          | NO  |
//            |          | NO  |
//            ^          | NO  |
//            ~          | YES |  /~  e.g.    int a = 10/~2;
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//            =          | NO  | "/=" is not supported
// 
//       % :  
//            +          | YES |  %+  e.g.    int a = 10%+3;   // int a = 10%(+3) = 1;
//            -          | YES |  %-  e.g.    int a = 10%-3;   // int a = 10%(-3) = 1;
//            *          | NO  |  
//            /          | NO  |  
//            %          | NO  |
//            &          | NO  |
//            |          | NO  |
//            ^          | NO  |
//            ~          | YES |  %~  e.g.    int a = 10%~2 = 10 % (~2);
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//            =          | NO  | "%=" is not supported
// 
// 
//       & :  
//            +          | YES |  &+  e.g.    int a = 10&+3;   // int a = 10 & (+3); 
//            -          | YES |  *-  e.g.    int a = 10&-3;   // int a = 10 & (-3);
//            *          | NO  |  
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |  "&&"  is not supported
//            |          | NO  |  
//            ^          | NO  |  
//            ~          | YES |  &~  e.g.   int a = 10&~2;  // int a = 10 & (~2);
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//            =          | NO  | "&=" is not supported
// 
// 
//       | :  
//            +          | YES |  |+  e.g.    int a = 10|+3;   // int a = 10 | (+3); 
//            -          | YES |  |-  e.g.    int a = 10|-3;   // int a = 10 | (-3);
//            *          | NO  |  
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |  
//            |          | NO  |  "||" is not supported
//            ^          | NO  |  
//            ~          | YES |  |~  e.g.   int a = 10|~2;  // int a = 10 / (~2);
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//            =          | NO  | "|=" is not supported
// 
//       ^ :  
//            +          | YES |  ^+  e.g.    int a = 10^+3;   // int a = 10 ^ (+3); 
//            -          | YES |  ^-  e.g.    int a = 10^-3;   // int a = 10 ^ (-3);
//            *          | NO  |  
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |  
//            |          | NO  |  
//            ^          | NO  |  
//            ~          | YES |  ^~  e.g.   int a = 10^~2;  // int a = 10 ^ (~2);
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//            =          | NO  | "^=" is not supported
// 
//       ~ :  
//            +          | YES |  ~+  e.g.    int a = ~+3;   // int a = 10 ~ (+3); 
//            -          | YES |  ~-  e.g.    int a = ~-3;   // int a = 10 ~ (-3);
//            *          | NO  |  
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |  
//            |          | NO  |  
//            ^          | NO  |  
//            ~          | YES |  ~~  e.g.   int a = ~~2;  // int a = ~( ~2 );
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//            =          | NO  | "~=" is not supported
// 
// 
//      << :  
//            +          | YES |  <<+  e.g.    int a = 10<<+3;   // int a = 10 << (+3); 
//            -          | YES |  <<-  e.g.    int a = 10<<-2;   // int a = 10 << (-2);  //  警告：左移次数为负 [默认启用]
//            *          | NO  |  
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |  
//            |          | NO  |  
//            ^          | NO  |  
//            ~          | YES |  <<~  e.g.   int a = 10<<~2;  // int a = 10 << (~2);  //  警告：左移次数为负 [默认启用]                    //  int a = 10<<~-2;   OK
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//            =          | NO  | "<<=" is not supported
// 
//      >> :  
//            +          | YES |  >>+  e.g.    int a = 10>>+3;   // int a = 10 >> (+3); 
//            -          | NO  |  >>-  e.g.    int a = 10>>-2;   // int a = 10 >> (-2);  //  警告 : 右左移次数为负 [默认启用]
//            *          | NO  |  
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |  
//            |          | NO  |  
//            ^          | NO  |  
//            ~          | YES |  >>~  e.g.   int a = 10>>~2;  // int a = 10 >> (~2);  //  警告 : 右移次数为负 [默认启用]                    //  int a = 10>>~-2;   =>   int a = 10 >> (~-2);  OK
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  
//            =          | NO  | ">>=" is not supported
// 
//      (  :  
//            +          | YES |  (+  e.g.     a = (+3+2);
//            -          | YES |  (-  e.g.     a = (-3+2);
//            *          | NO  |  
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |  
//            |          | NO  |  
//            ^          | NO  |  
//            ~          | YES |  (~ e.g.      a = (~2);
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  () is not allowed 
//            =          | NO  |  (=     e.g.   (a)=3;
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
//            ~          | NO  |  )~  ???
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  () is not allowed 
//            =          | YES |  )=     e.g.   (a)=3;
// 
//      =  :  
//            +          | YES |  =+  e.g.     a =+3;
//            -          | YES |  =-  e.g.     a =-3; 
//            *          | NO  |  
//            /          | NO  |
//            %          | NO  |
//            &          | NO  |  
//            |          | NO  |  
//            ^          | NO  |  
//            ~          | YES |  =~   e.g.  a=~2;
//            <<         | NO  | 
//            >>         | NO  |  
//            (          | YES |  
//            )          | NO  |  =)  is not allowed 
//            =          | NO  |  ==  is not allowed   
// 
//    6. Varible       | YES |
//    7. Semicolon     | YES |   Not Avalible List  : + - * / % & | ^ ~ << >> ( =     |    Only Avalible is :  )     e.g.    a=(b+c);


// 
// 6. Varible Type
//    1. Blank         | YES |
//    2. Comment       | YES |
//    3. Interger      | NO  |
//    4. Float         | NO  |
//    5. Operator      |PART |   Not Avalible List :      ~  ,  ( 
//    6. Varible       | NO  |
//    7. Semicolon     | YES |   
// 

//
// 7. Expression Evaluate Type      ==>   Semicolon       ";"
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

bool TokenListAnalyzer::pushToken(TokenInfo* pToken)
{
	m_tokenList.push_back( pToken );
	return true;
}



TokenInfo* TokenListAnalyzer::getPreviousToken(bool needSkipBlankComment, int* pHasSkipBlankComment)
{
	// init  flag
	if ( pHasSkipBlankComment != nullptr ) {
		*pHasSkipBlankComment = 0;
	}

	if ( m_tokenList.empty() ) {
		return nullptr;
	}

	TokenInfo* pRetTokenInfo = nullptr;
	for( auto r_it = m_tokenList.rbegin(); r_it != m_tokenList.rend();  )
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
	// E_TOKEN_BLANK   -->  ???
	//
	auto retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_BLANK, E_TOKEN_BLANK) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_BLANK, E_TOKEN_BLANK) Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_BLANK, E_TOKEN_COMMENT_TYPE) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_BLANK, E_TOKEN_COMMENT_TYPE)  Failed " << endl; }
	// retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_BLANK, E_TOKEN_MULTI_COMMENT) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_BLANK, E_TOKEN_MULTI_COMMENT)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_BLANK, E_TOKEN_DECIMAL_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_BLANK, E_TOKEN_DECIMAL_NUMBER)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_BLANK, E_TOKEN_OCTAL_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_BLANK, E_TOKEN_OCTAL_NUMBER)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_BLANK, E_TOKEN_HEX_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_BLANK, E_TOKEN_HEX_NUMBER)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_BLANK, E_TOKEN_FLOAT_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_BLANK, E_TOKEN_FLOAT_NUMBER)  Failed " << endl; }
	for ( int tp = static_cast<int>(E_TOKEN_OP_BEGIN); tp <= E_TOKEN_OP_ASSIGNMENT; ++tp ) {
		retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_BLANK, static_cast<E_TokenType>(tp) ) , 1) );
		if ( !retpr.second ) { cout << "genFlag(E_TOKEN_BLANK, " << tp << ")  Failed " << endl; }
	}

	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_BLANK, E_TOKEN_VARIBLE) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_BLANK, E_TOKEN_VARIBLE)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_BLANK, E_TOKEN_SEMICOLON) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_BLANK, E_TOKEN_SEMICOLON)  Failed " << endl; }


	//
	// E_TOKEN_COMMENT   -->  ???
	//
	//    E_TOKEN_SINGLE_COMMENT
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_BLANK) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_SINGLE_COMMENT)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_SINGLE_COMMENT) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_SINGLE_COMMENT)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_MULTI_COMMENT) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_MULTI_COMMENT)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_DECIMAL_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_DECIMAL_NUMBER)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_OCTAL_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_OCTAL_NUMBER)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_HEX_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_HEX_NUMBER)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_FLOAT_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_FLOAT_NUMBER)  Failed " << endl; }
	for ( int tp = static_cast<int>(E_TOKEN_OP_BEGIN); tp <= E_TOKEN_OP_ASSIGNMENT; ++tp ) {
		retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SINGLE_COMMENT, static_cast<E_TokenType>(tp) ) , 1) );
		if ( !retpr.second ) { cout << "genFlag(E_TOKEN_SINGLE_COMMENT, " << tp << ")  Failed " << endl; }
	}
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_VARIBLE) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_VARIBLE)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_SINGLE_COMMENT, E_TOKEN_SEMICOLON) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_SEMICOLON)  Failed " << endl; }

	//     E_TOKEN_MULTI_COMMENT
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_BLANK) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_SINGLE_COMMENT)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_SINGLE_COMMENT) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_SINGLE_COMMENT)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_MULTI_COMMENT) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_MULTI_COMMENT)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_DECIMAL_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_DECIMAL_NUMBER)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_OCTAL_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_OCTAL_NUMBER)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_HEX_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_HEX_NUMBER)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_FLOAT_NUMBER) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_FLOAT_NUMBER)  Failed " << endl; }
	for ( int tp = static_cast<int>(E_TOKEN_OP_BEGIN); tp <= E_TOKEN_OP_ASSIGNMENT; ++tp ) {
		retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_MULTI_COMMENT, static_cast<E_TokenType>(tp) ) , 1) );
		if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, " << tp << ")  Failed " << endl; }
	}
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_VARIBLE) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_VARIBLE)  Failed " << endl; }
	retpr = m_banPickCfgMap.insert( make_pair( genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_SEMICOLON) , 1) ); if ( !retpr.second ) { cout << "genFlag(E_TOKEN_MULTI_COMMENT, E_TOKEN_SEMICOLON)  Failed " << endl; }



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
