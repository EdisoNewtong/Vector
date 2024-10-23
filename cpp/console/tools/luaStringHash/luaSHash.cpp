#include <iostream>
#include <cstring>
#include <string>
using namespace std;


// Copy the code in lua 
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


int main(int argc, char* argv[])
{
	if ( argc != 2 ) {
		cout << "[ERROR] Please input only one string to calculate hash. " << endl;
		return -1;
	}

	const char* s = argv[1];
	size_t len_of_s = strlen(s);

	unsigned int hash_of_s = getStringHash( s, len_of_s );
	cout << "Length  of the string \"" << s << "\" is " << len_of_s 
	     << ", Hash = " << hash_of_s << endl;

	return 0;
}
