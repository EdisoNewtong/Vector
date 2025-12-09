#!/bin/bash

##########################################################################################################
# ************************************************************************************************************
# 
#             The multiple comment is not well supported  in  xxx.l file 
# 
# ************************************************************************************************************
# 
# The following command will generate a new source code file named lex.yy.c by default.
# 
# use -o  <new_file_name> to specify a given name 
#
##########################################################################################################
#
# $ flex rules.l
# or 
# $ flex -o rules.yy.c  rules.l 
##########################################################################################################

rules.yy.c:rules.l
	flex -o rules.yy.c  rules.l 


##########################################################################################################
#
# to build a new program for testing the feature of   lex 
#
##########################################################################################################
#
#
# !!! Notes !!! : 
#    To deleting the original file  "rules.l"  is prohibit !! ( gcc need the rule file "rules.l"  during the build time
#
#  e.g. Command
#     $ gcc  lex.yy.c  -lfl  -o  mylexer
#
##########################################################################################################
mylexer:rules.yy.c   rules.l
	gcc  -Wall  -o  mylexer rules.yy.c  -lfl


# After the program    "mylexer" has been generated
# run the test
test:mylexer test.c
	./mylexer   test.c
     


.PHONY:clean
clean:
	-rm   -Rf   *.yy.c  *.o*  *.exe   mylexer 


