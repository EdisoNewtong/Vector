#include <iostream>
using namespace std;

#include "SuperBinaryDisplayUtil.h"

int main(int argc,char* argv[], char* env[])
{
	if ( argc != 2 ) {
		cout << "[ERROR] : Missing args . You must add a file name to read" << endl;
		return -1;
	}
	
	string fname = argv[1];
	SuperBinaryDisplayUtil::FileInfo finfo;
	auto bret = SuperBinaryDisplayUtil::readFile_V1(fname, finfo);
	if ( bret ) {
		string outStr;
		SuperBinaryDisplayUtil::printFileInfo(finfo, outStr, true);
	} else {
		cout << "[ERROR] : read file failed " << endl;
	}


	return 0;
}
