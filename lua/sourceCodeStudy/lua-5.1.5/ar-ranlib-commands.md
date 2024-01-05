# A Brief introduction of  `ar`  and   `randlib` 

```Makefile
AR= ar rcu
RANLIB= ranlib


######################################################################################################
#
# Here symbol  **$@**  means   the target to be built which refers to    $(LUA_A)
#
######################################################################################################
$(LUA_A): $(CORE_O) $(LIB_O)
	$(AR) $@ $(CORE_O) $(LIB_O)	# DLL needs all object files
	$(RANLIB) $@
```


## Detail infomation  about   `ar   rcu`

### option `r` 
       Insert the files member... into archive (with replacement). This operation differs from q in that any previously existing members are deleted if their names match those being added.
	   If one of the files named in member... does not exist, ar displays an error message, and leaves undisturbed any existing members of the archive matching that name.
	   By default, new members are added at the end of the file; but you may use one of the modifiers a, b, or i to request placement relative to some existing member.
	   The modifier v used with this operation elicits a line of output for each file inserted, along with one of the letters a or r to indicate whether  the  file	 was  appended	(no  old  member  deleted)  or
	   replaced.

### option `c`
       Create the archive.	The specified archive is always created if it did not exist, when you request an update.  But a warning is issued unless you specify in advance that you expect to create it, by using
	   this modifier.

### option `u`
       Normally,  ar r... inserts all files listed into the archive.  If you would like to insert only those of the files you list that are newer than existing members of the same names, use this modifier.  
       The u modifier is allowed only for the operation r (replace).  In particular, the combination qu is not allowed, since checking the timestamps would lose any speed advantage from the operation q.


```bash
$ ar rcu 
   liblua.a 
   lapi.o lcode.o ldebug.o ldo.o ldump.o 
   lfunc.o lgc.o llex.o lmem.o lobject.o 
   lopcodes.o lparser.o lstate.o lstring.o 
   ltable.o ltm.o lundump.o lvm.o lzio.o 
   lauxlib.o lbaselib.o ldblib.o liolib.o 
   lmathlib.o loslib.o ltablib.o lstrlib.o 
   loadlib.o linit.o	                        # DLL needs all object files



$ ranlib liblua.a
```


## detail infomation about   `randlib`
ranlib - generate an index to an archive

**ranlib** generates an index to the contents of an archive and
stores it in the archive.  The index lists each symbol defined by
a member of an archive that is a relocatable object file.
