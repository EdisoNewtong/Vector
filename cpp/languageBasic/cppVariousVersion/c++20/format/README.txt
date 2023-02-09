==================================================
About Compiler
==================================================
$ g++  --version
g++ (GCC) 12.2.1 20230111






==================================================
About Header File ( format )
==================================================
main.cpp:4:10: 致命错误：format：没有那个文件或目录
    4 | #include <format>
      |          ^~~~~~~~


The Absolute filename of header file is : 
**************************************************
  /usr/include/fmt/format.h
**************************************************

// use the following include   directive to include  'format'   header
#include <fmt/format.h>




==================================================
About Compile Time Issue
==================================================
//
// the function format(...) is not inside the namespace    std { ... }
//
// The namespace 'v9' is default namespace    and can be omitted
// Use fmt::format(...)  or  fmt::v9::format(...)  instead
//

main.cpp:20:35: 错误：‘format’ is not a member of ‘std’; did you mean ‘fmt::v9::format’?
   20 |     auto formated_strError = std::format("{} = {}", strAge, age);
      |                                   ^~~~~~
In file included from /usr/include/fmt/format-inl.h:29,
                 from main.cpp:5:
/usr/include/fmt/format.h:4185:13: 附注：‘fmt::v9::format’ declared here
 4185 | inline auto format(const Locale& loc, format_string<T...> fmt, T&&... args)
      |             ^~~~~~




==================================================
About Link Time Issue
==================================================
/usr/bin/ld: build/main.o: in function `char fmt::v9::detail::decimal_point<char>(fmt::v9::detail::locale_ref)':
main.cpp:(.text._ZN3fmt2v96detail13decimal_pointIcEET_NS1_10locale_refE[_ZN3fmt2v96detail13decimal_pointIcEET_NS1_10locale_refE]+0x14): undefined reference to `char fmt::v9::detail::decimal_point_impl<char>(fmt::v9::detail::locale_ref)'
/usr/bin/ld: build/main.o: in function `fmt::v9::detail::thousands_sep_result<char> fmt::v9::detail::thousands_sep<char>(fmt::v9::detail::locale_ref)':
main.cpp:(.text._ZN3fmt2v96detail13thousands_sepIcEENS1_20thousands_sep_resultIT_EENS1_10locale_refE[_ZN3fmt2v96detail13thousands_sepIcEENS1_20thousands_sep_resultIT_EENS1_10locale_refE]+0x2f): undefined reference to `fmt::v9::detail::thousands_sep_result<char> fmt::v9::detail::thousands_sep_impl<char>(fmt::v9::detail::locale_ref)'
collect2: 错误：ld 返回 1
make: *** [Makefile:13: main] Error 1

You must link a library named :               'libfmt.so'  to solve the linker error
It refers to the Absolute file of   '/usr/lib/libfmt.so'   


Add the following link flag at the last step of building phase
   -lfmt

g++    -Wall   -std=c++20   -lfmt   -o    main    build/main.o








==================================================
No Fixed literal format string
==================================================
Use  :
    std::string vformat( std::string_view fmt, std::format_args args );

Notes : 
    <std::format_args args> is only one arguments
    if you have a lot of argumenst to format , use <fmt::make_format_args> to wrap all object to be used by formatter



void test4(const string& fmt, const string& a,const string& b)
{
    cout << fmt::vformat(fmt.c_str(), fmt::make_format_args(a,b)  ) << endl;
}

int main(int argc, char* argv[]) 
{
    test4( std::string(argv[1]), argv[2], argv[3] );
}


$ ./main "output : s2 = '{1}',  s1 = '{0}'"     World    Hello
output : s2 = 'World', s1 = 'Hello'
