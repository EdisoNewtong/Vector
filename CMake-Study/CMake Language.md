# CMake Language 


## Organization 
CMake input files are written in the "CMake Language" in source files named  
1. named **==CMakeLists.txt==** or  
1. ending in a **==.cmake==** file name extension. 

## Directories 
When CMake processes a project source tree, the entry point is a source file called **==CMakeLists.txt==**  in the top-level source directory.  
This file may contain the entire build specification or use the **==add_subdirectory()==** command to add subdirectories to the build.  
Each subdirectory added by the command must also contain a **==CMakeLists.txt==** file as the entry point to that directory. 

## Script 
An individual <script>.cmake source file may be processed in script mode by using the cmake(1) command-line tool with the -P option. 


## Encoding 
 CMake Language source file may be written in 7-bit ASCII text for maximum portability across all supported platforms.  
 Newlines may be encoded as either \n or \r\n but will be converted to \n as input files are read. 

Note that the implementation is 8-bit clean so source files may be encoded as UTF-8 on platforms with system APIs supporting this encoding. 
In addition, CMake 3.2 and above support source files encoded in UTF-8 on Windows (using UTF-16 to call system APIs). 
Furthermore, CMake 3.0 and above allow a leading UTF-8 Byte-Order Mark in source files. 

## Commands
In Cmake , Built-in Command names are case-**==in==**sensitive. 
```CMake
SET(a "123")
set(a "123")
```

## Comment
- Single Line Comment ( '#'     All the content after the symbol '#' until the end of line has been commented )
```CMake
# Line 1 has been commented
# CMakeLists.txt
project(Hello)     # it declare a new project named  "Hello"
```


- Multi Lines Comment ( #[[ comment lines ]]   or   #[=[  comment lines ]=]       or   #[==[  comment lines ]==]   ) 
```CMake
#[[
Coment Line 1
Coment Line 2
Coment Line 3
]]

###################################################
# Alternative 1
###################################################

#[=[
Coment Line 1 ]]
Coment Line 2 ]]
Coment Line 3 ]]
]=]



###################################################
# Alternative 2
###################################################

#[==[
Coment Line 1 ]=]
Coment Line 2 ]=]
Coment Line 3 ]=]
]==]
```

## Output some log message to the console
message([<mode>]   "message text")
```CMake
message(STATUS   "Hello CMake")
```



## Multi-Line Text ( [[ multi line text ]] )  
```CMake
message(STATUS [[Here is line-1 , 
Here is line-2 , 
Here is line-3 ,
]])

###################################################
# Alternative 1
###################################################
message(STATUS "Here is line-1 , 
Here is line-2 , 
Here is line-3 ,
")


######################################################################################################
# Alternative 2 ( Note that the symbol '\' must be the last character of the line )
######################################################################################################
message(STATUS "Here is line-1 ,\ 
Here is line-2 , \
Here is line-3 , \
")

```



## function argument seperator ( different from other programming languages , CMake use <Space> or NewLine to seperate arguments)
```CMake
set(version 123) # set a new variable whose value is 123 
```




## Control Structures 
### if-else part 
```CMake

# Single if sample code
if( <one-condition> )
   # if part code
endif()



#  if-else sample code
if( <one-condition> )
   # if part code
else()
   # else part code
endif()



#  if-elseif-else  sample code
if(<condition>)
  <commands>
elseif(<condition>) # optional block, can be repeated
  <commands>
else()              # optional block
  <commands>
endif()

```

### loop part 
```CMake
foreach( ... )
  # can use built-in function      break()  or  continue()
  <commands>
endforeach()


while( ... )
  # can use built-in function      break()  or  continue()
  <commands>
endwhile()
```

## Varibles
Variables are the basic unit of storage in the CMake Language. 
Their values are always of **==String==** type, 
though some commands may interpret the strings as values of other types. 
The set() and unset() commands explicitly set or unset a variable, 
but other commands have semantics that modify variables as well.

Variables have dynamic scope. 
Each variable "set" or "unset" creates a binding in the current scope:

Note :
CMake reserves identifiers that:
- begin with CMAKE_ (upper-, lower-, or mixed-case), or
- begin with _CMAKE_ (upper-, lower-, or mixed-case), or
- begin with _ followed by the name of any CMake Command.





