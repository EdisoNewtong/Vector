# set the required cmake version with 3.1.0 
# ( if the cmake version is lower than 3.1.0 , cmake will stop working with an error )
cmake_minimum_required(VERSION 3.1.0)

# set the project name as  "Tutorial"
# Notes : You should *** Never *** add the double quotes surround the workd  Tutorial
# and the final built output program will be named as Tutorial with the capital letter 'L'
# rather than the lower case source file's ( tutorial.cxx )   base name  'tutorial'
project(Tutorial)

# add the source code "tutorial.cxx" to the project named "Tutorial" 's build file list 
add_executable(Tutorial tutorial.cxx)

