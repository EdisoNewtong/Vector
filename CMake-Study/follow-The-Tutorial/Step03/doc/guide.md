# Step03 : Adding Usage Requirements for a Library

In Step02 :
  We first state that anybody linking to MathFunctions needs to include the current source directory, while MathFunctions itself doesn't. 
  So this can become an **==INTERFACE==** usage requirement.

Remember **==INTERFACE==** means things that 
consumers require         ( here consumers refers to   the program project  "tutorial" )
but the producer doesn't. ( here producer   efers to   the library project  "MathFunctions" )

```cmake
# inside MathFunctions/CMakeLists.txt

target_include_directories(MathFunctions
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )
```

