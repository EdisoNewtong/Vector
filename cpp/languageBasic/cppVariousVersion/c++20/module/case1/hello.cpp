module;  // compare with project "case0"     do not use  sentence :       export module aaaYourhello;     at the first line

import <iostream>;
export module aaaYourhello;
// #include <iostream>
// import core.std;




export void hello()
{
	std::cout << "Hello World!" << std::endl;
}


// without  export  , internal use
void hello2()
{
	std::cout << "Hello World internal !" << std::endl;
}

