# Launch GDB with arguments

`$gdb ==-p <process-id>==  $` gdb ==--args &lt;Program Name&gt; \[program arguments]==

# Inquire the program status ( running / stoped / or any status else )

(gdb) i(nfo) pro(gram)

# Start your program with arguments

(gdb) sta(rt) \[program arguments]    // it set a temporary breakpoint at the beginning of the **==main==** entrypoint 
(gdb) r(un)   \[program arguments]

(gdb) set args \[program arguments]\
(gdb) show args
(gdb) run

# Breakpoint

## Set Breakpoint

(gdb) b &lt;lineNumber&gt;\
(gdb) b &lt;functionname&gt;\
(gdb) b &lt;filename&gt;:&lt;lineNumber&gt;\
(gdb) b &lt;filename&gt;:&lt;functionname&gt;\
(gdb) b &lt;lineNumber&gt; if condition

### watchpoint / catchpoint / temporary breakpoint

(gdb) watch  &lt;expr&gt;   // stop running when the &lt;expr&gt; when its value is changed
(gdb) rwatch &lt;expr&gt;   // stop running when the &lt;expr&gt; only when it is read by the program
(gdb) awatch &lt;expr&gt;   // stop running when the &lt;expr&gt; has been either read or written by the program

(gdb) catch catch     //
(gdb) catch throw     //

(gdb) tbreak ...      // set a temporary breakpoint which has only 1 chance to hit and then being delete by GDB

## Set the condition for an existed breakpoint

(gdb) condition <#BreakNum>   &lt;condition&gt;

## Breakpoints Commands List

(gdb) commands <#BreakNum>
...
input your commands Here
...
end&lt;Enter&gt;

e.g.
(gdb) commands 1
printf  "index = %d\n", i
end&lt;Enter&gt;

## automatic display the expr when the program stop at the line to be set by command    'display'

(gdb) display &lt;expr&gt;
(gdb) info display
(gdb) delete  display &lt;display-id&gt;
(gdb) enable  display &lt;display-id&gt;
(gdb) disable display &lt;display-id&gt;

## Skip some function which you are not interested (After you set the step will skip any of them in the skip list  )

(gdb) skip &lt;function-Name&gt;
(gdb) info skip
(gdb) skip delete  &lt;skip-id&gt;
(gdb) skip enable  &lt;skip-id&gt;
(gdb) skip disable &lt;skip-id&gt;

## List Breakpoint(s) ( Also Watchpoint(s) && Catchpoint(s) ) you set

(gdb) i(nfo) b

## Enable/Disable Breakpoint

(gdb) ena(ble)     &lt;Breakpoint-ID&gt;\
(gdb) dis(able)    &lt;Breakpoint-ID&gt;

## Delete breakpoint(s)

(gdb) cl(ear) // delete any breakpoints at the next instruction to be executed in the selected stack frame\
(gdb) cle(ar) &lt;Breakpoint-ID&gt;\
(gdb) del(ete) \[breakpoints] &lt;Breakpoint-ID&gt;\
(gdb) del     // Delete all breakpoints you have set before

# Enter/Exit Function | Single-Step | Continue

(gdb) c(ontinue)   // continue executing until the next breakpoints hint\
(gdb) fg           // alias to   `continue`

(gdb) n(ext)       // executing till the next line of the current context\
(gdb) s(tep)       // entering to the function at the current line\
(gdb) fin(ish)     // exit the function ==normally==\
(gdb) ret(urn) \[return-value]  // exit the function ==forcely== ( the rest code after the current line ==has no chance== to execute ) with the proper return value\
(gdb) u(til)   \[line-number]   // jump out of the loop at the first time iterator changes

(gdb) nexti      // executing one instruction only

    i = 3; i -= 4; ++i;

> (gdb) nexti      // executing `i = 3;` only
> (gdb) nexti      // executing `i -= 4;` only
> (gdb) nexti      // executing `++i;` only

# Call Stack

(gdb) bt           // show the call stack
(gdb) where        // alias to 'bt'
(gdb) f(rame)      // show the current line to be executed
(gdb) f(rame) &lt;N&gt;  // pick a certain frame by the given argument &lt;N&gt;
(gdb) i ar(gs)     // show the function's argument list
(gdb) i lo(cals)   // show the function's inner local varible
(gdb) up   \[n]     // move to the previous \[n] frame
(gdb) down \[n]     // move to the previous \[n] frame

# print

(gdb) p   &lt;varible-name&gt;          // default mode is decimal
(gdb) p/d &lt;varible-name&gt;          // decimal mode
(gdb) p/x &lt;varible-name&gt;          // hex mode
(gdb) p/o &lt;varible-name&gt;          // octal mode
(gdb) p/u &lt;varible-name&gt;          // treat varible as an unsigned type if necessary
(gdb) p/f &lt;varible-name&gt;          // treat varible as a float format
(gdb) p/t &lt;varible-name&gt;          // print as a binary
(gdb) p/s &lt;varible-name&gt;          // print as a string if necessary
(gdb) p   &lt;varible-name&gt;=&lt;value&gt;  // assign a value to a varible

## print Array

(gdb) p   \*pArrayAddress\@len
(gdb) set print array-indexes

(gdb) with print array on -- print some\_array
is Equivalent to the following 3 commands
(gdb) set print array on
(gdb) print some\_array
(gdb) set print array off

/\*
==n== : the repeat count
==f== : the display format   ( d x o t u t s f ... )
==u== : the unit size        ( b\:bytes , h(HalfWords):2 bytes  w(Words):4 bytes   g(Giant word):8 bytes)

e.g.

```
typedef struct AAA
{
    int black;
    int red;
    int green;
    int blue;
} AAA;

AAA Obj; Obj.black = 10; Obj.red = 20; Obj.green = 30; Obj.blue = 40;
AAA* pObj = &Obj;
(gdb) ==x/4dw pObj==
0x7fffffffdea0:	10	20	30	40



typedef struct BBB
{
    char ch;
    int  num;
} BBB;
BBB bb; bb.ch = ' '; bb.num = 123;
BBB* pb = &bb;
(gdb) ==x/8db pb==
0x7fffffffde98:	32	0	0	0	123	0	0	0

```

\*/
(gdb) x/nfu  <address>

# Debug Macro  ( Use option   ==-g3 -gdwarf-2==     to compile your source code )

(gdb) info macro &lt;macroname&gt;
(gdb) macro expand      &lt;macro-expression&gt;
(gdb) macro expand-once &lt;macro-expression&gt;

# Misc

(gdb) ptype  &lt;exp&gt;    // show the expression's structure and data type
(gdb) whatis &lt;exp&gt;    // show the expression's evaluated data type

(gdb) save breakpoints <a>   // save all breakpoints into file
(gdb) source <a>                // execute the gdb commands inside the given filename
