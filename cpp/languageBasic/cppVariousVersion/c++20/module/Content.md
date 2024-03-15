# C++20 Modules
The form of a module source code is like the following
[modern cpp](https://www.modernescpp.com/index.php/c-20-module-interface-unit-and-module-implementation-unit/)


```c++

module;                      // global module fragment

#include <headers for libraries not modularized so far>

export module math;          // module declartion 

import <importing of other modules> 

<non-exported declarations>  // names with only visibiliy inside the module

export namespace math {

    <exported declarations>  // exported names 

}

```


```c++

// before c++20 module 
#include <iostream>

// since c++20 module ,  Here '#' must be deleted , and  ';' is a must at the end of the statement
import <iostream>;

```


# case0  
- one module implementation source code : myFirstModule.cpp  ==>  HelloModule.gcm
- one main source code to make use of module 'HelloModule' rather than  'myFirstModule'

# case1 
- one module implementation for a class source code : personModule.cpp  ==>  person.gcm
- one main source code to make use of module 'person'

# case2 
- one module implementation source code for a class defination ( and its implementation is after the defination )  : personModule.cpp  ==>  person.gcm
- one main source code to make use of module 'person'

# case3 
- one module's interface
- the module's implementation
- one main source code to make use of module 'person'

# case4 
- one module's interface
- **seperated implementation** part 1
- **seperated implementation** part 2
- one main source code to make use of module 'person'

# case5 
- one module's interface
- it's **submodule** part 1 decclearation
- it's **submodule** part 1 implementation
- it's **submodule** part 2 decclearation
- it's **submodule** part 2 implementation
- one main source code to make use of module 'person'

# case6 
- one module's interface
- one module's **partition 1**
- one module's **partition 2**
- one main source code to make use of module 'person'

