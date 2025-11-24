# How to build an international Application 

1. write source code with QTranslator and a given load translation file name such as "my_tr". (with no extension)
1. Use xxx::tr("original language text") to mark the text to be translated 
1. Use tool 'lupdate.exe' to generate a text based configuration file with XML format translation file "my_tr.ts".
1. Use input the translated text of "original language text" refers from Step#2
1. Use tool 'urelease.exe' to convert "my_tr.ts" to a binary translated file : "my_tr.qm"
1. Copy the "my_tr.qm" to the working directory. 
1. Run the Application
