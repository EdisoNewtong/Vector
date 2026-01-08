#include <iostream>
#include <cstring>
#include <string>
#include <unordered_map>
#include <list>
using namespace std;



// Copy the code in lua 
// from lstring.c:                           
//                  TString *luaS_newlstr (lua_State *L, const char *str, size_t l) { ... }
unsigned int getStringHash(const char* str, size_t l)
{
    unsigned int h = static_cast<unsigned int>(l);  /* seed */
    size_t step = (l>>5)+1;  /* if string is too long, don't hash all its chars */
    size_t l1;

    for (l1=l; l1>=step; l1-=step) {  /* compute hash */
      h = h ^ ( (h<<5) + (h>>2) + static_cast<unsigned char>( str[l1-1]) );
    }

    return h;
}



struct iteratorRange {
	char* pool;	
	int   elementCnt;
};


void testCase(int charCnt)
{

	if ( charCnt <=0 ) {
		return;
	}

	const size_t nNumberCnt = 10;
	char nums[nNumberCnt] = { 0 }; for( size_t i = 0; i < nNumberCnt; ++i ) { nums[i] = '0' + i; }

	const size_t nAlphaCnt = 26;
	char lowerCase[nAlphaCnt] = { 0 }; for( size_t i = 0; i < nAlphaCnt; ++i ) { lowerCase[i] = 'a' + i; }
	char upperCase[nAlphaCnt] = { 0 }; for( size_t i = 0; i < nAlphaCnt; ++i ) { upperCase[i] = 'A' + i; }

	const size_t nSymbolCnt = 34;
	char symbols[nSymbolCnt] = {
		' ',   // <Space>
		'\t',  // Tab
		'`',   // grave accent
		'~',   // tilde 
		'!',   // 1
		'@',   // 2
		'#',   // 3
		'$',   // 4
		'%',   // 5
		'^',   // 6
		'&',   // 7
		'*',   // 8
		'(',   // 9
		')',   // 0

		'-',   // -  minus
		'_',   // _  underline

		'+',   // + 
		'=',   // = 

		'[',   // [ 
		'{',   // {

		']',   // ] 
		'}',   //} 

		'\\',  //  
		'|',   //  |

		';',   //  ;
		':',   //  :

		'"',    //  "
		'\'',   //  '

		'<',    //  <
		',',   //  ,

		'>',    // > 
		'.',    // .

		'?',    // ?
		'/',    // / 

	};


	const size_t nBegCnt = nAlphaCnt * 2 + 1;
	char beginningChars[nBegCnt] = { 0 };


	int idx = 0;
	beginningChars[idx++] = '_';
	for( size_t i = 0; i < nAlphaCnt; ++i, ++idx ) { beginningChars[idx] = lowerCase[i]; }
	for( size_t i = 0; i < nAlphaCnt; ++i, ++idx ) { beginningChars[idx] = upperCase[i]; }

	// for( size_t i = 0; i < nBegCnt; ++i ) {
	// 	  cout << (i+1) << ". " << beginningChars[i] << endl;
	// }
	// cout << endl << endl;

	//                   (26     * 2) + '_'  + 0~9
	const size_t nValidCnt = nAlphaCnt * 2 + 1    + nNumberCnt; 
	char validChars[nValidCnt] = { 0 };
	idx = 0;
	for ( size_t i = 0; i < nBegCnt;    ++i, ++idx ) { validChars[idx] = beginningChars[i]; }
	for ( size_t i = 0; i < nNumberCnt; ++i, ++idx ) { validChars[idx] = nums[i]; }

	const size_t nAllCharSetCnt = (nValidCnt + nSymbolCnt);
	char allCharSet[nAllCharSetCnt] = { 0 };
	idx = 0;
	for ( size_t i = 0; i < nValidCnt;    ++i, ++idx ) { allCharSet[idx] = validChars[i]; }
	for ( size_t i = 0; i < nSymbolCnt;   ++i, ++idx ) { allCharSet[idx] = symbols[i];  }
	// (void)allCharSet;


	int* indices = new int[charCnt];
	for( int i = 0; i < charCnt; ++i ) { indices[i] = 0; }

	iteratorRange* loopLayers = new iteratorRange[charCnt];
	for( int i = 0; i < charCnt; ++i ) {
		loopLayers[i] = { allCharSet, static_cast<int>( nAllCharSetCnt )  };
	}

	// set layer instance
	/*
	loopLayers[0] = { beginningChars, static_cast<int>( nBegCnt )  };
	for( int i = 1; i < charCnt; ++i ) {
		loopLayers[i] = { validChars, static_cast<int>( nValidCnt )  };
	}
	*/

	unordered_map<unsigned int, list<string> > G_MAP;
	for ( int nBits = 1; nBits <= charCnt; ++nBits ) {

		// set all indexes as 0
		for( int i = 0; i < nBits; ++i ) {
			indices[i] = 0;
		}

		while( indices[0] < loopLayers[0].elementCnt )
		{
			string builtString;
			int layerIdx = 0;
			while ( layerIdx < nBits ) {
				builtString += loopLayers[ layerIdx ].pool[  indices[layerIdx] ];
				++layerIdx;
			}

			//
			// builtString.size() == nBits , calculate hash code
			//
			auto hashCode = getStringHash( builtString.c_str(),  static_cast<size_t>(nBits) );
			auto it = G_MAP.find( hashCode );
			if ( it == G_MAP.end() )  {
				list<string> lst; lst.push_back( builtString );
				G_MAP.insert( make_pair( hashCode, lst) );
			} else {
				it->second.push_back( builtString );
			}


			// Update the last index
			do {
				--layerIdx; // layerIdx == nBits , so index will be (layerIdx-1)
				++indices[layerIdx];
				if (  indices[layerIdx] < loopLayers[ layerIdx ].elementCnt ) {
					break;
				} else {
                    if ( layerIdx > 0 ) {
					    indices[layerIdx] = 0;
                    }
				}
			} while( layerIdx > 0 );
		}
	}

	delete [] indices;
	delete [] loopLayers;

	// cout << "G_MAP.size = " << G_MAP.size();
	for( const auto& pr : G_MAP ) {
		size_t sz = pr.second.size();
		cout << pr.first << " : size=" << sz << " [ ";
		auto it = pr.second.begin();
		for ( size_t i = 0; i < sz; ++i, ++it ) {
			cout << "\"" << *it << "\"";
			if ( i < ( sz-1 ) ) {
				cout << " , ";
			}
		}
		cout << " ] " << endl;
	}
}





int main(int argc, char* argv[])
{
	if ( argc != 2 ) {
		cout << "[ERROR] Please input only one string to calculate hash. " << endl;
		return -1;
	}

    /*****************************************************
	testCase( atoi(argv[1]) );
    *****************************************************/


	const char* s = argv[1];
	size_t len_of_s = strlen(s);

	unsigned int hash_of_s = getStringHash( s, len_of_s );
	cout << "Length  of the string \"" << s << "\" is " << len_of_s 
	     << ", Hash = " << hash_of_s << endl;

	return 0;
}
