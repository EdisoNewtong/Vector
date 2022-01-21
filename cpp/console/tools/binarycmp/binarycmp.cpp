#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;


int main(int argc, char* argv[], char* env[])
{
	if ( argc != 3 ) {
		cout << "[ERROR] : You must give 2 file paths to compare ! " << endl;
		return -1;
	}

	size_t sz1 = 0;
	size_t sz2 = 0;


	ifstream f1(argv[1], ios::in | ios::binary);
	if ( !f1 ) {
		f1.close();
		cout << "[ERROR] : Can't open/find file-1 : " << argv[1] <<  endl;
		return -1;
	}
	f1.seekg(0, ios::end);
	sz1 = static_cast<size_t>( f1.tellg() );
	f1.seekg(0, ios::beg);

	ifstream f2(argv[2], ios::in | ios::binary);
	if ( !f2 ) {
		f2.close();
		cout << "[ERROR] : Can't open/find file-2 : " << argv[2] <<  endl;
		return -1;
	}
	f2.seekg(0, ios::end);
	sz2 = static_cast<size_t>( f2.tellg() );
	f2.seekg(0, ios::beg);


	string fname1(argv[1]);
	string fname2(argv[2]);
	size_t f1len = fname1.size();
	size_t f2len = fname2.size();
	size_t longerFname = (f1len > f2len) ? f1len : f2len;

	if ( sz1 != sz2 ) {
		f1.close();
		f2.close();
		cout << "[INFO] : 2 files size are !!!<Not>!!! equal " << endl
		     << "         file-1 : \"" << setfill(' ') << setw(longerFname) << argv[1] << "\" , size = " << std::dec << sz1 << endl
		     << "         file-2 : \"" << setfill(' ') << setw(longerFname) << argv[2] << "\" , size = " << std::dec << sz2 << endl;
		return -1;
	}

	char ch1 = 0;
	char ch2 = 0;
	bool isSame = true;
	size_t chIdx = 0;
	while ( !f1.eof() && !f2.eof() ) {
		ch1 = ch2 = 0;
		f1.read(&ch1, sizeof(ch1) );
		f2.read(&ch2, sizeof(ch2) );
		if ( ch1 != ch2 ) {
			isSame = false;
			break;
		}

		// otherwise
		++chIdx;
	}
	f1.close();
	f2.close();
	
	
	if ( isSame ) {
		// Nice Same
		cout << "[INFO] : 2 files are absolutely <identical>    :) " << endl;
	} else {
		// not Same
		cout << "[INFO] : 2 files are !!!<Not>!!!   Same " << endl
			 << "         @idx = [" << std::dec << chIdx << "] , in file-1 : \"" << setfill(' ') << setw(longerFname) << argv[1] << "\" , character = 0x" << std::uppercase << std::hex << setw(2) << setfill('0') << static_cast<int>(ch1) << endl
			 << "         @idx = [" << std::dec << chIdx << "] , in file-2 : \"" << setfill(' ') << setw(longerFname) << argv[2] << "\" , character = 0x" << std::uppercase << std::hex << setw(2) << setfill('0') << static_cast<int>(ch2) << endl;
	}

	return 0;
}
