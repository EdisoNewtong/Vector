#ifndef PARSERED_BUFF_H
#define PARSERED_BUFF_H

#include <string>
// BuffCursor , Character
struct ChInfo
{
    ChInfo();

    char ch;
    //
    // Positon Info
    //
    int cursorIdx;
    int line;
    int column;

    bool isLastCh;

    std::string getPos(int flag = 1) const;
};



class Buff
{
public:
    Buff(const char* fileContent, int length);
    virtual ~Buff();

    void moveNext();
    bool isAtEnd();

    // const char* getPreviosChar(bool moveCursor = false);
    const char* getCurrentChar();
    const char* getNextChar();

    ChInfo& getCursor();
protected:
    const char* m_fileContent;
    const int m_length;
    
    ChInfo m_chCursor;
};




#endif
