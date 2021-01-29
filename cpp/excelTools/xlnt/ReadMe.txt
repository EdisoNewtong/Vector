**************************************************
C/C++  Excel Read && Write Library
**************************************************
----------------------------------------------------------------------------------------------------
1. tfussell/xlnt        https://github.com/tfussell/xlnt.git
2. troldal/OpenXLSX     https://github.com/troldal/OpenXLSX.git
3. QtExcel/QXlsx        https://github.com/QtExcel/QXlsx.git
----------------------------------------------------------------------------------------------------





**************************************************
https://github.com/tfussell/xlnt.git   | Test OK |
**************************************************

e66e417b        GitHub      Sun Jan 3 19:44:46 2021         -0500 Merge pull request #525 from kbelyaev/memory_leaks_fix

1. git clone   https://github.com/tfussell/xlnt.git
2. mkdir build
3. cmake ..
4. make

----------------------------------------------------------------------------------------------------

5. mkdir "edisonTest" 
6. copy all the file into this empty   folder  "edisonTest"

//
// compile and link successfully under  c++11 / c++14 
// should link with the libxlnt.1.5.0.dylib  and option :  -Wl,-rpath,../build/source
7. make
8. $ unzip  excelTest.zip  

// 0 :    iSheet0.rows(false)   ,  keep Empty Cell existed
9. $ ./main  ./excelTest/test1.xlsx   0

// 1 :    iSheet0.rows(false)   ,  kick out all Empty Cell
10. $ ./main  ./excelTest/test1.xlsx   1


