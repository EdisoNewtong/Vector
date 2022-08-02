import aaaYourhello;

#include <iostream>


int main(int argc, char* argv[], char* env[])
{
    if ( argc != 3 ) {
        std::cout << "[ERROR] : require 2 interger number arguments . " << std::endl;
        return -1;
    }

    auto nArg1 = atoi( argv[1] );
    auto nArg2 = atoi( argv[2] );
	auto ret = hello_DoModule(nArg1, nArg2);
    std::cout << nArg1 << " % " << nArg2 << " = " << ret << std::endl;

	return 0;
}

