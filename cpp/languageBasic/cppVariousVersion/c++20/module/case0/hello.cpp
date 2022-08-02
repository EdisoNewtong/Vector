// as the tutorial show :
//
//    You'd better write such statement at the 1st line of this such in order to declare it as a named exported   module
export module aaaYourhello;

import<iostream>;
// #include <iostream>
// import core.std;




export void hello()
{
	std::cout << "Hello World!" << std::endl;
}


// without  keyword "export"  , internal use only
// export 
void hello2()
{
	std::cout << "Hello World internal !" << std::endl;
}

