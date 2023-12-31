# set the required cmake version with 3.1.0 
# ( if the cmake version is lower than 3.1.0 , cmake will stop working with an error )
cmake_minimum_required(VERSION 3.1.0)

# set the project name as  "Tutorial"
# Notes : You should *** Never *** add the double quotes surround the workd  Tutorial
# and the final built output program will be named as Tutorial with the capital letter 'L'
# rather than the lower case source file's ( tutorial.cxx )   base name  'tutorial'
#           ***** HERE *****   set the version of the project
project(Tutorial VERSION 1.0)

# specify the C++ standard 
# Make sure to add the CMAKE_CXX_STANDARD declarations above the call to add_executable.
set(CMAKE_CXX_STANDARD 11)                     # compile with c++11 standard
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Copy "TutorialConfig.h.in" into the build dir 
# replace the file content of given VERSION number
#          and then rename it to "TutorialConfig.h"
configure_file(TutorialConfig.h.in TutorialConfig.h)



# add the source code "tutorial.cxx" to the project named "Tutorial" 's build file list 
add_executable(Tutorial tutorial.cxx)


target_include_directories(Tutorial PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

