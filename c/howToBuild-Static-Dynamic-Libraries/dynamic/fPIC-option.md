
# What's the difference between  `-fPIC`  and `-fpic` 

Use -fPIC or -fpic to generate code. 
  Whether to use -fPIC or -fpic to generate code is target-dependent. 
The -fPIC choice always works, but may produce larger code than -fpic (mnenomic to remember this is that PIC is in a larger case, so it may produce larger amounts of code). 
Using -fpic option usually generates smaller and faster code, but will have platform-dependent limitations, such as the number of globally visible symbols or the size of the code. 
The linker will tell you whether it fits when you create the shared library. 
When in doubt, I choose -fPIC, because it always works.


## gcc -fpic / -fPIC

  With the -fpic or -fPIC flag, gcc creates an object file with position independent code. This is usually necessary when creating a shared object.
Apparently, on a x86 environment, -fpic and -fPIC are equivalent. On other architectures, there is a difference.

### Preprocessor macros
  When compiling with -fpic, the compiler defines the macro \_\_pic\_\_ and \_\_PIC\_\_ to 1. When copmiling with -fPIC, these macros are defined to 2.

-fpic
> Generate position-independent code (PIC) suitable for use in a shared library, if supported for the target
> machine. Such code accesses all constant addresses through a global offset table (GOT). The dynamic
> loader resolves the GOT entries when the program starts (the dynamic loader is not part of GCC; it is part of the
> perating system). If the GOT size for the linked executable exceeds a machine-specific maximum size, you
> get an error message from the linker indicating that -fpic does not work; in that case, recompile with
> -fPIC instead. (These maximums are 16k on the m88k, 8k on the SPARC, and 32k on the m68k and RS/6000. The 386
> has no such limit.)

  Position-independent code requires special support, and therefore works only on certain machines. For the
386, GCC supports PIC for System V but not for the Sun 386i. Code generated for the IBM RS/6000 is always
position-independent.


-fPIC
> If supported for the target machine, emit position-independent code, suitable for dynamic linking and
> avoiding any limit on the size of the global offset table. This option makes a difference on the m68k, m88k,
> and the SPARC.

  Position-independent code requires special support, and therefore works only on certain machines.

