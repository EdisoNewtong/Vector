#include "buff.h"
#include "charUtil.h"
#include "commonEnum.h"

using namespace std;


ChInfo::ChInfo()
    : ch(0)
    , cursorIdx(0)
    , line(1)
    , column(1)
    , isLastCh(false)
{
}


string ChInfo::getPos(int flag /* = 1 */) const
{
    using namespace charUtil;

    string positionStr;
    positionStr = (to_string(line) + ":" + to_string(column) + " (idx=" + to_string(cursorIdx) + ") ");
    if ( flag == 1 ) {
        if ( ch <= 32 ) {
            if ( ch == 32 ) {
                positionStr += "' ' ";
            } else if ( ch == '\t' ) {
                positionStr += "'\\t' ";
            } else if ( ch == '\r' ) {
                positionStr += "'\\r' ";
            } else if ( ch == '\n' ) {
                positionStr += "'\\n' ";
            } else {
                auto code = static_cast<int>(ch & 0xFF);
                positionStr += string("'?' code = ") + to_string(code) + SPACE_1;
            }
        } else if ( ch == 127 ) {
                positionStr += "'Del' ";
        } else {
            // normal printable character
            positionStr += (SINGLE_QUOTO + string(1, ch) + SINGLE_QUOTO + SPACE_1);
        }
    }

    return positionStr;
}


Buff::Buff(const char* fileContent, int length)
    : m_fileContent(fileContent)
    , m_length(length)
    , m_chCursor()
{
    m_chCursor.ch = m_fileContent[0];
    m_chCursor.isLastCh = ( m_chCursor.cursorIdx == (m_length-1) ); // because cursorIdx's init value is 0
}

    

// virtual 
Buff::~Buff()
{
    INNER_SAFE_DELETE( m_fileContent )
}

void Buff::moveNext()
{
    using namespace charUtil;

    if ( isAtEnd() ) {
        return;
    }
    
    // Not at last
    char previousCh = m_fileContent[m_chCursor.cursorIdx];
    ++m_chCursor.cursorIdx;
    if ( isAtEnd() ) {
        return;
    }
    // else cursor is inside the range

    // set current
    m_chCursor.ch = m_fileContent[m_chCursor.cursorIdx];
    m_chCursor.isLastCh = (m_chCursor.cursorIdx == (m_length-1));
    char curCh = m_fileContent[m_chCursor.cursorIdx];
    if ( previousCh == NEW_LINE_N || previousCh == NEW_LINE_R ) {
        if ( previousCh == NEW_LINE_R ) {
            if ( curCh == NEW_LINE_N ) {
                // \r\n
                ++m_chCursor.column;
            } else {
                ++m_chCursor.line;
                m_chCursor.column = 1;
            }
        } else {
            // previousCh == '\n'
            ++m_chCursor.line;
            m_chCursor.column = 1;
        }
    } else {
        ++m_chCursor.column;
    }
}


// const char* Buff::getPreviosChar(bool moveCursor  /* = true */ )
// {
//     int prevIdx = m_chCursor.cursorIdx - 1;
// 
//     if ( moveCursor ) {
//         m_chCursor.cursorIdx = prevIdx;
//     }
// 
//     if ( prevIdx >= 0  ) {
//         return &m_fileContent[prevIdx];
//         // return const_cast<char*>( &m_fileContent[nextIdx] );
//     } 
// 
//     // else idx is out of range
//     return nullptr;
// }


const char* Buff::getCurrentChar()
{
    int idx = m_chCursor.cursorIdx;
    if ( idx < m_length ) {
        return &m_fileContent[idx];
    } 

    // else idx is out of range
    return nullptr;
}



const char* Buff::getNextChar()
{
    auto nextIdx = m_chCursor.cursorIdx + 1;
    if ( nextIdx < m_length ) {
        return &m_fileContent[nextIdx];
    }

    // else idx is out of range
    return nullptr;
}




bool Buff::isAtEnd()
{
    return    ( m_chCursor.cursorIdx < 0
             || m_chCursor.cursorIdx >= m_length);
}


ChInfo& Buff::getCursor()
{
    return m_chCursor;
}



