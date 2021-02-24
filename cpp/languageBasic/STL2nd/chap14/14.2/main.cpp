#include <iostream>
#include <regex>
#include <string>
using namespace std;


int main(int argc, char* argv[], char* env[])
{
	string data = "XML tag: <tag-name>the value</tag-name>.";
	cout << "data:           " << data << "\n\n";


	smatch m; // for returned detail of the match
	bool found = regex_search(data,m,  regex("<(.*)>(.*)</(\\1)>")   );


	// print match details:
	cout << "m.empty():      " << boolalpha << m.empty() << endl;
	cout << "m.size():       " << m.size() << endl;

	if ( found ) {
		cout << "m.str():        " << m.str() << endl;
		cout << "m.length():     " << m.length() << endl;
		cout << "m.position():   " << m.position() << endl;
		cout << "m.prefix():     " << m.prefix() << endl;
		cout << "m.suffix():     " << m.suffix() << endl;
		cout << endl;


		// iterating over all matches (using the match index):
		for ( int i = 0; i < static_cast<int>(m.size()); ++i ) {
			cout << "m[" << i << "].str():     " << m[i].str() << endl;
			cout << "m.str(" << i << "):       " << m.str(i) << endl;
			cout << "m.position(" << i << "):  " << m.position(i) 
				 << endl;
		}
		cout << endl;


		// iterating over all matches (using iterators):
		cout << "matches:" << endl;
		for (auto pos = m.begin(); pos != m.end(); ++pos ) {
			cout << "   " << *pos << " ";
			cout << "  (length:  " << pos->length() << ")" << endl;
		}
	} else {
		cout << "regex_search(...)  Failed" << endl;
	}

	return 0;
}

