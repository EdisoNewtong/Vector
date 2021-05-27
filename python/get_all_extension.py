# -*- coding: utf-8 -*-

import sys
import os
import os.path
import time

def checkIsV3():
    # win32  darwin(MacOS)  linux
    platform = sys.platform

    #    3               6           8
    # sys_info[0] , sys_info[1] , sys_info[2]
    sys_info = sys.version_info # 3.6.8
    if sys_info[0] < 3:
        return False

    return True


def collectExtension(path):
    const_non_ext = "<No Extention File>"
    extObj = { }

    #
    # specific the  visit path as the parent folder
    #
    if path=="":
        visitPath = os.path.abspath( os.path.join(os.getcwd() ) )
    else:
        visitPath = os.path.abspath( path )

    for path,dirname,flist in os.walk( visitPath ):
        # print(path, dirname, flist)
        for fname in flist:
            # full_path = os.path.join(path , fname)
            extAry = os.path.splitext(fname)
            ext = extAry[1]

            key_ext = ext if ext!="" else const_non_ext
            if key_ext.startswith("."):
                key_ext = key_ext[1:]

            if not (key_ext in extObj):
                # extObj[key_ext] = { "file_list" : [ full_path ], "count" : 1 }
                extObj[key_ext] = 1
            else:
                # extObj[key_ext]["count"] = extObj[key_ext]["count"] + 1
                # extObj[key_ext]["file_list"].append(full_path)
                extObj[key_ext] = extObj[key_ext] + 1
                # extObj[key_ext]["file_list"].append(full_path)



    printDetialFlag = False
    nHowMany = 0
    if printDetialFlag:
        for extstr,infoObj in extObj.items():
            nHowMany = nHowMany + 1
            print("==================================================")
            print("{}. *.{}  |  total count = {}".format(nHowMany,extstr, infoObj["count"] ) )
            print("==================================================")
            for idx,name in enumerate(infoObj["file_list"]):
                print("     {}.  {}".format(idx+1, name) )
            print()
    else:
        # print("==================================================")
        for extstr,infoObj in extObj.items():
            nHowMany = nHowMany + 1
            # print("{}. *.{}  |  total count = {}".format(nHowMany,extstr, infoObj["count"] ) )
        # print("==================================================")


    print("How Many kinds of extention  = {}".format(nHowMany))
    return extObj


def main():
    if not checkIsV3():
        print("You must run this script under python3")
        return
    
    start = time.time()
    strstart = time.asctime()
    print("start Time = {}".format(strstart) )
    arglen = len(sys.argv)
    retObj = None
    if arglen==1:
        retObj = collectExtension("")
    else:
        retObj = collectExtension( sys.argv[1] )

    end = time.time()
    strend = time.asctime()
    print("end = {}".format(strend) )
    elapsedTime = end - start
    print("elapsedtime = {} ".format(elapsedTime) )

    print("retObj.len = {}".format(len(retObj)) )
    if "cpp" in retObj:
        print("OK.    \".cpp\" .count = {}".format( retObj["cpp"]) )
    else:
        print("Sorry. \".cpp\" .count = 0")


if __name__ == "__main__":
    main()

