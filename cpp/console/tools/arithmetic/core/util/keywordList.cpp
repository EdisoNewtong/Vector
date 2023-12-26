#include "keywordList.h"

using namespace std;

// static 
unordered_set<string> KeyWordList::s_keywordList{
    "and",    // &&
    "and_eq", // &=
    "or",     // || 
    "or_eq",  // |=
    "xor_eq", // ^=
    "not",    // !
    "not_eq", // !=
    "bitand", // &
    "bitor",  // |
    "xor",    // ^
    "compl",  // same as '~'

    "auto",
    "bool", // Try    bool    support ?
    "break",
    "case",
    "catch",
    "char",  // picked 
    "class",
    "const",
    "const_cast",
    "continue",
    "decltype",
    "default",
    "delete",
    "do",
    "double",  // picked   
    "dynamic_cast",
    "else",
    "enum",
    "explicit",
    "extern",
    "false",  // Try    bool    support ?
    "float",  // picked   
    "for",
    "friend",
    "goto",
    "if",
    "inline",
    "int",  // picked   
    "long",  // picked   
    "mutable",
    "namespace",
    "new",
    "noexcept",
    "nullptr",
    "operator",
    "private",
    "protected",
    "public",
    "register",
    "reinterpret_cast",
    "return",
    "short",  // picked   
    "signed",  // picked   
    "sizeof",
    "static",
    "static_cast",
    "struct",
    "switch",
    "template",
    "this",
    "throw",
    "true",  // Try    bool    support ?
    "try",
    "typedef",
    "typeid",
    "typename",
    "union",
    "unsigned",  // picked   
    "using",
    "virtual",
    "void",
    "volatile",
    "volatile",
    "wchar_t",
    "while",

    //
    // c++ 11 new keyword
    //
    "alignas",
    "alignof",
    // "char8_t",    // Since C++ 20
    "char16_t",
    "char32_t",
    "constexpr",
    "static_assert",
    "thread_local",

    "override",
    "final",

};


unordered_set<string> KeyWordList::s_typeKeyword{
    "unsigned",
    "signed",

    "char",
    "short",
    "int",
    "long",

    "float",
    "double"
};


// static 
bool KeyWordList::isKeyWord(const string& varName)
{
    return s_keywordList.find(varName) != s_keywordList.end();
}


// static
bool KeyWordList::isTypeKeyWord(const std::string& name)
{
    return s_typeKeyword.find(name) != s_typeKeyword.end();
}
