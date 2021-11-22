#include "tokenParserMgr.h"

#include "decimalParser.h"
#include "octalParser.h"
#include "hexParser.h"

#include "floatParser.h"

#include "varibleParser.h"

#include "operatorParser.h"

#include "singleLineCommentParser.h"
#include "multiLineCommentParser.h"

#include "blankParser.h"

#include "expevaluation.h"



using namespace std;

#ifdef USE_INT_INSTEAD_OF_ENUM
    // static 
    unordered_map<int, TokenParserBase*> TokenParserMgr::s_parserMap;
#else
    // static 
    unordered_map<E_PaserType, TokenParserBase*> TokenParserMgr::s_parserMap;
#endif

// static 
void TokenParserMgr::init()
{
	s_parserMap.clear();
	s_parserMap.insert( make_pair(enumCvt(E_P_DEFAULT), new TokenParserBase(E_P_DEFAULT) ) );
	// 
	s_parserMap.insert( make_pair(enumCvt(E_P_DECIMAL), new DecimalParser(E_P_DECIMAL) ) );
	s_parserMap.insert( make_pair(enumCvt(E_P_OCTAL),   new OctalParser(E_P_OCTAL) ) );
	s_parserMap.insert( make_pair(enumCvt(E_P_HEX),     new HexParser(E_P_HEX) ) );
	//
	s_parserMap.insert( make_pair(enumCvt(E_P_FLOAT),  new FloatParser(E_P_FLOAT) ) );
	//
	s_parserMap.insert( make_pair(enumCvt(E_P_VARIBLE), new VaribleParser(E_P_VARIBLE) ) );
	//
 	s_parserMap.insert( make_pair(enumCvt(E_P_OPERATOR),  new OperatorParser(E_P_OPERATOR) ) );
	//
 	s_parserMap.insert( make_pair(enumCvt(E_P_SINGLE_LINE_COMMENT), new SingleLineCommentParser(E_P_SINGLE_LINE_COMMENT) ) );
 	s_parserMap.insert( make_pair(enumCvt(E_P_MULTI_LINE_COMMENT),  new MultiLineCommentParser(E_P_MULTI_LINE_COMMENT)  ) );
    //
 	s_parserMap.insert( make_pair(enumCvt(E_P_BLANK),  new BlankParser(E_P_BLANK) ) );

 	s_parserMap.insert( make_pair(enumCvt(E_P_ENDLESEE_SEMICOLON),  new ExpEvaluation(E_P_ENDLESEE_SEMICOLON) ) );

	//
	// init
	//
	for( auto it = s_parserMap.begin(); it != s_parserMap.end(); ++it )
	{
		auto parser = it->second;
		if ( parser != nullptr ) {
			parser->init();
		}
	}

}

//static 
void TokenParserMgr::finalize()
{
	// clear parser map
	for( auto it = s_parserMap.begin(); it != s_parserMap.end(); ++it )
	{
		auto parser = it->second;
		if ( parser != nullptr ) {
			delete parser;
			it->second = nullptr;
		}
	}
	s_parserMap.clear();
}



// static 
TokenParserBase* TokenParserMgr::getParserByType(E_PaserType type)
{
#ifdef USE_INT_INSTEAD_OF_ENUM
	auto it = s_parserMap.find( enumCvt(type) );
#else
	auto it = s_parserMap.find(type);
#endif 
	if ( it != s_parserMap.end() ) {
		return it->second;
	} else {
		return nullptr;
	}
}


