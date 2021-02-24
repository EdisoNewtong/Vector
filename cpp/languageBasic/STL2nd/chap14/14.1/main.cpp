#include <regex>
#include <iostream>
#include <string>
using namespace std;

void out(int idx,bool b)
{
	cout <<  idx << ". " << (b ? "found" : "not found") << endl;
}


int main(int argc, char* argv[], char* env[])
{
	bool found = false;

	//
	//  regex_match(const char* content , const regex& expr);
	//  e.g.
	//			regex_match("xxx" , regex("some reg expr"));   // Compile OK
	//			regex_match("xxx" , "some reg expr");		   // Compile Error , 2nd arg must be a type of regex object
	//
	//			string content = "xxxxx";
	//			regex_match(content , regex("some reg expr"));				   // Compile OK
	//			regex_match(content.c_str() , regex("some reg expr"));		   // Compile OK
	//


	// find XML/HTML-tagged value (using default syntax)
	regex reg1("<.*>.*</.*>");
	found = regex_match("<tag>value</tag>",		// data
                        reg1);                  // regular expression
	// cout << "1 ... " << endl;
	out(1,found);


	// find XML/HTML-tagged value (tags before and after the value must match):
	regex reg2("<(.*)>.*</\\1>");
	found = regex_match("<tag>value</tag>",    	// data
                       reg2);                   // regular expression
	// cout << "2 ... " << endl;
	out(2,found);


	// find XML/HTML-tagged value (using grep syntax):
	regex reg3("<\\(.*\\)>.*</\\1>", regex_constants::grep);
	found = regex_match("<tag>value</tag>",		// data
                        reg3);                  // regular expression
	out(3,found);


	// use C-string as regular expression (needs explicit cast to regex):
	found = regex_match("<tag>value</tag>",		 // data
			            regex("<(.*)>.*</\\1>"));// regular expression
	out(4,found);
	cout << endl;



	//////////////////////////////////////////////////////////////////////
	//
	// regex_match() versus  regex_search():
	//
	//////////////////////////////////////////////////////////////////////
	found = regex_match("XML tag: <tag>value</tag>",
                        regex("<(.*)>.*</\\1>") );	// fails to match , because the prefix "XML tag: " is not matched
	out(5,found);


	found = regex_match("XML tag: <tag>value</tag>",
                        regex(".*<(.*)>.*</\\1>.*") );	// matches
	out(6,found);


	found = regex_search("XML tag: <tag>value</tag>",
                        regex("<(.*)>.*</\\1>") );	// matches
	out(7,found);


	found = regex_search("XML tag: <tag>value</tag>",
                        regex(".*<(.*)>.*</\\1>.*") );	// matches
	out(8,found);


	return 0;
}


