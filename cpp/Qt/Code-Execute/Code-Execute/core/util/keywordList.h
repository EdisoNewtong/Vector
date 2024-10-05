#ifndef KEY_WORD_LIST_H
#define KEY_WORD_LIST_H

#include <unordered_set>
#include <string>

class KeyWordList
{
protected:
    static std::unordered_set<std::string> s_keywordList;
    static std::unordered_set<std::string> s_typeKeyword;
public:
    static bool isKeyWord(const std::string& varName);
    static bool isTypeKeyWord(const std::string& name);
};


#endif
