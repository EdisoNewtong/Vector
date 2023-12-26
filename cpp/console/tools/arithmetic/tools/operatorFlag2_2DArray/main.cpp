#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

static const int TYPE_CNT = 24;

struct BoolPairInfo
{
	bool occupied;

	bool flag1;
	bool flag2;
};


static BoolPairInfo G_infoArray[TYPE_CNT][TYPE_CNT];


enum E_OperatorType
{
    // E_OPERATOR_UNKNOWN = 0,

    E_ADD = 0,      // +  , e.g.   a + b
    E_MINUS,    // -  , e.g.   a - b
    E_MULTIPLY, // *
    E_DIVIDE,   // /
    E_MOD,      // %

    E_BIT_AND,  // &
    E_BIT_OR,   // |
    E_BIT_XOR,  // ^
    E_BIT_NOT,  // ~
    E_BIT_LEFT_SHIFT,     // <<
    E_BIT_RIGHT_SHIFT,    // >>

    E_OPEN_PARENTHESES,   // (
    E_CLOSE_PARENTHESES,  // )
    E_ASSIGNMENT,         // =

    E_ADD_ASSIGNMENT,                 // +=
    E_MINUS_ASSIGNMENT,               // -=
    E_MULTIPLY_ASSIGNMENT,            // *=
    E_DIVIDE_ASSIGNMENT,              // /=
    E_MOD_ASSIGNMENT,                 // %=
    E_BIT_AND_ASSIGNMENT,             // &=
    E_BIT_OR_ASSIGNMENT,              // |=
    E_BIT_XOR_ASSIGNMENT,             // ^=
    E_BIT_LEFT_SHIFT_ASSIGNMENT,      // <<=
    E_BIT_RIGHT_SHIFT_ASSIGNMENT,     // >>=

       E_POSITIVE, // +  , e.g.   +a
       E_NEGATIVE, // -  , e.g.   -a

    E_OPERATOR_UNKNOWN 

};


static unordered_map<string,int> s_Map {
	{ make_pair("E_ADD", E_ADD) },
	{ make_pair("E_MINUS",E_MINUS) },
	{ make_pair("E_MULTIPLY", E_MULTIPLY) },
	{ make_pair("E_DIVIDE",   E_DIVIDE) },
	{ make_pair("E_MOD",      E_MOD) },
	{ make_pair("E_BIT_AND",  E_BIT_AND) },
	{ make_pair("E_BIT_OR",   E_BIT_OR) },
	{ make_pair("E_BIT_XOR", E_BIT_XOR) },
	{ make_pair("E_BIT_NOT", E_BIT_NOT) },
	{ make_pair("E_BIT_LEFT_SHIFT",   E_BIT_LEFT_SHIFT) },
	{ make_pair("E_BIT_RIGHT_SHIFT", E_BIT_RIGHT_SHIFT) },
	{ make_pair("E_OPEN_PARENTHESES",E_OPEN_PARENTHESES) },
	{ make_pair("E_CLOSE_PARENTHESES",E_CLOSE_PARENTHESES) },
	{ make_pair("E_ASSIGNMENT",      E_ASSIGNMENT) },
	{ make_pair("E_ADD_ASSIGNMENT", E_ADD_ASSIGNMENT) },
	{ make_pair("E_MINUS_ASSIGNMENT",E_MINUS_ASSIGNMENT) },
	{ make_pair("E_MULTIPLY_ASSIGNMENT",E_MULTIPLY_ASSIGNMENT) },
	{ make_pair("E_DIVIDE_ASSIGNMENT", E_DIVIDE_ASSIGNMENT) },
	{ make_pair("E_MOD_ASSIGNMENT",   E_MOD_ASSIGNMENT) },
	{ make_pair("E_BIT_AND_ASSIGNMENT",E_BIT_AND_ASSIGNMENT) },
	{ make_pair("E_BIT_OR_ASSIGNMENT",E_BIT_OR_ASSIGNMENT) },
	{ make_pair("E_BIT_XOR_ASSIGNMENT",E_BIT_XOR_ASSIGNMENT) },
	{ make_pair("E_BIT_LEFT_SHIFT_ASSIGNMENT",E_BIT_LEFT_SHIFT_ASSIGNMENT) },
	{ make_pair("E_BIT_RIGHT_SHIFT_ASSIGNMENT", E_BIT_RIGHT_SHIFT_ASSIGNMENT) }

};

static unordered_map<int, pair<string,string> > s_MapOppsite {
	{ make_pair(E_ADD, make_pair("E_ADD", "+") ) },
	{ make_pair(E_MINUS, make_pair("E_MINUS", "-" ) ) },
	{ make_pair(E_MULTIPLY, make_pair("E_MULTIPLY", "*") ) },
	{ make_pair(E_DIVIDE, make_pair("E_DIVIDE", "/")   ) },
	{ make_pair(E_MOD, make_pair("E_MOD", "%")      ) },
	{ make_pair(E_BIT_AND, make_pair("E_BIT_AND", "&")  ) },
	{ make_pair(E_BIT_OR, make_pair("E_BIT_OR", "|")   ) },
	{ make_pair(E_BIT_XOR, make_pair("E_BIT_XOR", "^") ) },
	{ make_pair(E_BIT_NOT, make_pair("E_BIT_NOT", "~") ) },
	{ make_pair(E_BIT_LEFT_SHIFT, make_pair("E_BIT_LEFT_SHIFT", "<<")   ) },
	{ make_pair(E_BIT_RIGHT_SHIFT, make_pair("E_BIT_RIGHT_SHIFT", ">>") ) },
	{ make_pair(E_OPEN_PARENTHESES, make_pair("E_OPEN_PARENTHESES", "(") ) },
	{ make_pair(E_CLOSE_PARENTHESES, make_pair("E_CLOSE_PARENTHESES", ")") ) },
	{ make_pair(E_ASSIGNMENT, make_pair("E_ASSIGNMENT", "=")      ) },
	{ make_pair(E_ADD_ASSIGNMENT, make_pair("E_ADD_ASSIGNMENT", "+=") ) },
	{ make_pair(E_MINUS_ASSIGNMENT, make_pair("E_MINUS_ASSIGNMENT", "-=")) },
	{ make_pair(E_MULTIPLY_ASSIGNMENT, make_pair("E_MULTIPLY_ASSIGNMENT", "*=")) },
	{ make_pair(E_DIVIDE_ASSIGNMENT, make_pair("E_DIVIDE_ASSIGNMENT", "/=") ) },
	{ make_pair(E_MOD_ASSIGNMENT, make_pair("E_MOD_ASSIGNMENT", "%=")   ) },
	{ make_pair(E_BIT_AND_ASSIGNMENT, make_pair("E_BIT_AND_ASSIGNMENT", "&=")) },
	{ make_pair(E_BIT_OR_ASSIGNMENT, make_pair("E_BIT_OR_ASSIGNMENT", "|=")) },
	{ make_pair(E_BIT_XOR_ASSIGNMENT, make_pair("E_BIT_XOR_ASSIGNMENT", "^=")) },
	{ make_pair(E_BIT_LEFT_SHIFT_ASSIGNMENT, make_pair("E_BIT_LEFT_SHIFT_ASSIGNMENT", "<<=")) },
	{ make_pair(E_BIT_RIGHT_SHIFT_ASSIGNMENT, make_pair("E_BIT_RIGHT_SHIFT_ASSIGNMENT", ">>=") ) }
};




bool collectInfoFromFile( const string& filename, vector<string>* retList)
{
	if ( retList == nullptr ) {
		return false;
	}
	retList->clear();

	ifstream f(filename.c_str(), ios::in);
	if ( !f ) {
		cout << "[ERROR] Can't open file \"" << filename << "\"" << endl;
		return false;
	}

	int lineNo = 1;
	// int cnt = 0;
	while( !f.eof() ) {
		string line;
		getline(f, line);

		auto pos = line.find("{ E");
		if ( pos != string::npos ) {
			auto pos1 = line.find("E");
			auto pos2 = line.find(",");
			auto tag1 = line.substr(pos1, pos2-pos1);

			auto pos3 = line.find("E", pos2+1);
			auto pos4 = line.find(",", pos3+1);
			auto tag2 = line.substr(pos3, pos4-pos3);

			auto pos5 = line.find_first_not_of(' ', pos4+1);
			auto pos6 = line.find(',', pos4+1);
			pos6 = line.find_first_not_of(' ', pos6+1);

			/*

			if ( cnt <= 24 ) {
				cout << (cnt + 1) << " tag1 = " << tag1
				<< ", tag2 = " << tag2 
				<< ", flag1 = " << (line.at(pos5) == 'f' ? "false" : "true") 
				<< ", flag2 = " << (line.at(pos6) == 'f' ? "false" : "true") << endl;


				++cnt;
			}

		   */

			auto idx_i = -1;
			auto idx_j = -1;
			auto it_i = s_Map.find(tag1);
			if ( it_i == s_Map.end() ) {
				cout << "[ERROR] Not found : " << tag1 << endl;
			} else {
				idx_i = it_i->second;
			}

			auto it_j = s_Map.find(tag2);
			if ( it_j == s_Map.end() ) {
				cout << "[ERROR] Not found : " << tag2 << endl;
			} else {
				idx_j = it_j->second;
			}

			if ( idx_i != -1   &&   idx_j != -1 ) {
				if ( !G_infoArray[idx_i][idx_j].occupied  ) {
					G_infoArray[idx_i][idx_j].occupied = true;

					G_infoArray[idx_i][idx_j].flag1 = (line.at(pos5) == 't');
					G_infoArray[idx_i][idx_j].flag2 = (line.at(pos6) == 't');
				} else {
					cout << "[ERROR] Meet same pair at LINE : " <<  lineNo << endl;
				}
			}

			retList->push_back( line );
		}

		++lineNo;
	}

	return true;
}

int main(int argc, char* argv[])
{
	if ( argc <= 1 ) {
		cout << "[ERROR] Missing arguments. " << endl;
		return -1;
	}

	// init 2D Array
	for( int i = 0; i < TYPE_CNT; ++i ) {
		for( int j = 0; j < TYPE_CNT; ++j ) {
			G_infoArray[i][j].occupied = false;

			G_infoArray[i][j].flag1 = false;
			G_infoArray[i][j].flag2 = false;
		}
	}




	vector<string> lst;

	auto ret = collectInfoFromFile( argv[1], &lst);
	if ( ret ) {
		cout << "list.size = " << lst.size() << endl;

		auto all_occupied = true;
		for( int i = 0; i < TYPE_CNT; ++i ) {
			for( int j = 0; j < TYPE_CNT; ++j ) {
				if ( !G_infoArray[i][j].occupied ) {
					all_occupied = false;
					break;
				}
			}

			if ( !all_occupied ) {
				break;
			}
		}


		cout << (all_occupied ? "[OK]" : "[FAILED]") << endl;

        if ( all_occupied ) {
            cout << "Array2D = { " << endl;
            for( int i = 0; i < TYPE_CNT; ++i ) {
                auto it2_i = s_MapOppsite.end();
                if (   i >=0  &&  i <=E_BIT_RIGHT_SHIFT_ASSIGNMENT ) {
                    it2_i = s_MapOppsite.find(i);
                }

                if ( it2_i != s_MapOppsite.end() ) {
                    // Comment Part   Row Tag
                    cout << "\t{ " << "  //  "  << it2_i->second.first
                         << endl;

                    for( int j = 0; j < TYPE_CNT; ++j ) {
                        if ( j >=0  &&  j <=E_BIT_RIGHT_SHIFT_ASSIGNMENT ) {
                            auto it2_j = s_MapOppsite.find(j);
                            if ( it2_j != s_MapOppsite.end() ) {

                                //  Column Tag
                                /*
                                cout << "\t\t                 " 
                                     << it2_i->second.first
                                     << "  "
                                     << it2_j->second.first
                                     << "  // " << it2_i->second.second << "   " << it2_j->second.second
                                     << endl;
                                */

                                cout << "\t\t{  BoolPairInfo( " 
                                       << (G_infoArray[i][j].flag1 ? "true" : "false" )
                                       << ", " 
                                       << (G_infoArray[i][j].flag2 ? "true" : "false" )
                                     << " ) }" // BoolPairInfo end   )
                                     << (j < (TYPE_CNT-1)  ? "," : "")
                                     // Comment Part
                                       << "\t // "
                                       << it2_i->second.second << "  " << it2_j->second.second
                                       << " "
                                     << endl;
                            }
                        }
                    }

                    cout << "\t}" << (i <(TYPE_CNT-1) ? "," : "") << endl;
                }
            }

            cout << "}" << endl;
        }

	} else {
		cout << "Collecting Failed " << endl;
	}

	return 0;
}
