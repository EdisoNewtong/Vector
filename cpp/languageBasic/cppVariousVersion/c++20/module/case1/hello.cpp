// compare with project "case0"     
//    do not write the 1st  sentence :       export module aaaYourhello;     at the first line of this file
module;  

import <iostream>;
export module aaaYourhello;
// #include <iostream>
// import core.std;




export void hello()
{
	std::cout << "Hello World!" << std::endl;
}


// without  keyword "export"  , internal use only
void hello2()
{
	std::cout << "Hello World internal !" << std::endl;
}

