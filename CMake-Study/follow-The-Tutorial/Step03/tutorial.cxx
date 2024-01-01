// A simple program that computes the square root of a number
#include <cmath>
#include <iostream>
#include <string>


#include "TutorialConfig.h"


// TODO 5: Include MathFunctions.h
/***************************************************
 * Core Core Core *
 ******************

 This headerfile "MathFunctions.h" must be located 
after headerfile "TutorialConfig.h"

Because the marco defination of symbol "USE_MYMATH" is inside TutorialConfig.h
rather than a command line  compile flag

Otherwise the compile will report an error : unknown namespace    mathfunctions::detail::
due to the header file  "MathFunctions.h" hasn't been included yet.

 ***************************************************/
#ifdef USE_MYMATH
  #include "MathFunctions.h"
#endif



int main(int argc, char* argv[])
{
  if (argc < 2) {
    // report version
    std::cout << argv[0] << " Version " << Tutorial_VERSION_MAJOR << "."
              << Tutorial_VERSION_MINOR << std::endl;
    std::cout << "Usage: " << argv[0] << " number" << std::endl;
    return 1;
  }

  // convert input to double
  const double inputValue = std::stod(argv[1]);
  // TODO 6: Replace sqrt with mathfunctions::sqrt

  // calculate square root
#ifdef USE_MYMATH
  const double outputValue = mathfunctions::detail::mysqrt(inputValue);
#else
  const double outputValue = sqrt(inputValue);
#endif


  std::cout << "The square root of " << inputValue << " is " << outputValue
            << std::endl;
  return 0;
}
