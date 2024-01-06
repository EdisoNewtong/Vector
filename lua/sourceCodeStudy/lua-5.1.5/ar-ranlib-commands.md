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


## detail infomation about   `ranlib` from command `man ranlib`
ranlib - generate an index to an archive

**ranlib** generates an index to the contents of an archive and
stores it in the archive.  The index lists each symbol defined by
a member of an archive that is a relocatable object file.


### Why need use command 'ranlib' after create a library ( use command 'ar' to create )
Q : What will happen if  creating a library by ar  without use  'ranlib' ?
A : 

When creating a library with `ar` (the Unix archiver), the `ranlib` command is traditionally used to generate an index for the archive. This index makes it faster to link against the library, as the linker can quickly locate the object files it needs.

If you create a library with `ar` but do not use `ranlib` to generate the index, several things might happen:

1. **Slower Linking**: Without the index, the linker may take longer to resolve symbols and locate the required object files within the library. This can result in slower compile and link times.

2. **No Warning/Error**: `ranlib` is not always required for every system or toolchain. Some modern linkers or systems may automatically handle the absence of an index without producing any warnings or errors. However, the absence of an index could still affect performance.

3. **Potential Compatibility Issues**: Some older tools or systems might expect the index to be present. Without `ranlib`, these tools might not work as expected or could produce warnings or errors.

4. **Lack of Optimization**: The absence of an index means that the linker cannot take advantage of the optimized lookup mechanism that the index provides. This might not be noticeable for small projects or libraries, but for larger projects, the performance difference could be more pronounced.

In summary, while it's technically possible to create a library with `ar` without using `ranlib`, doing so could result in slower linking times and potential compatibility issues. It's generally recommended to use `ranlib` after creating a library with `ar` to ensure optimal performance and compatibility.


### 《 从 Linux 程序设计 ( 第3版 ) 》  中的解释
Page 9~10

7) 库文件创建好后，几个目标文件也已添加。
在某些系统，尤其是从 Berkeley UNIX 衍生的系统中，要想成功地使用函数库，还需要为函数库生成一个内容表。我们通过 ranlib 命令来完成这一工作

