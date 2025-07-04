# -*- coding: utf-8 -*-

import os

########################################################################################
#
# in order to  summary the total lines of all the source code files
#
########################################################################################

filecnt = 0
lineCnt = 0
for path,d,filelist in os.walk( "./" ):
	# print( path,d,filelist)
	for f in filelist:
		if (not (f=="r.py"))   and ( f.endswith(".h") or f.endswith(".cpp") ):
			filecnt = filecnt + 1
			fname = os.path.join(path,f)
			f = open( fname, "r" , encoding='utf-8' )   # encoding is a must option to read   utf-8 code 
			# print(fname)
			ls = f.readlines()
			cnt = len(ls)
			f.close()
			# print( "{}. file : {} has {} lines".format(filecnt, fname,cnt) )
			lineCnt = lineCnt + cnt


print("Totally , {} files occupy {} lines. ".format( filecnt, lineCnt) )
