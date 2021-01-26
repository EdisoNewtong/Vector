#include "myhexeditor.h"

#include <QKeyEvent>
// #include <QDebug>

// explicit 
myhexeditor::myhexeditor(QWidget* parent /* = nullptr */ )
    : QTextEdit(parent)
    , m_validKeyboardkeys()
{
    // 0-9 , <Space>  Left/Right/Up/Down   <BackSpace> <Del>
    m_validKeyboardkeys.clear();
    // 0 ~ 9 && A ~ F
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_0) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_1) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_2) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_3) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_4) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_5) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_6) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_7) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_8) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_9) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_A) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_B) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_C) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_D) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_E) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_F) );
    // <Space>
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_Space) );
    // Left/Right   |   Up/Down
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_Left) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_Right) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_Up) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_Down) );
    // <BackSpace> <Delete>
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_Backspace) );
    m_validKeyboardkeys.insert( static_cast<int>(Qt::Key_Delete) );

    m_hints = "Invalid input : valid range [0-9A-F] and <Space> <Up> <Down> <Left> <Right> <Backspace> <Delete>";
    m_validstr = "";
}


// virtual 
myhexeditor::~myhexeditor()
{
}



// virtual 
void	myhexeditor::keyPressEvent(QKeyEvent * e) // Q_DECL_OVERRIDE;
{
    if ( e !=nullptr ) {
        auto keycode = e->key();
        auto modifies = e->modifiers();
        // Qt::Key_Shift , Qt::Key_Control , Qt::Key_Meta, Qt::Key_Alt
        // qDebug() << "key = " << keycode;
        auto it = m_validKeyboardkeys.find(keycode);
        if ( it != m_validKeyboardkeys.end() 
            || keycode == Qt::Key_Control
            || keycode == Qt::Key_Alt
            || keycode == Qt::Key_Shift
            || keycode == Qt::Key_Meta ) 
        {
            QTextEdit::keyPressEvent(e);
            emit invalidInputKey(m_validstr, false);
        } else {
            // To Support Ctrl+X or Ctrl+V   , C is include in A-F
            auto isSpecialKey = (keycode == Qt::Key_X || keycode == Qt::Key_V);
            if ( isSpecialKey 
                 && ((modifies &  Qt::MetaModifier) == Qt::MetaModifier) ) {
                // To Support Ctrl+X Ctrl+V  ,  (Ctrl+) C in included in A-F
                QTextEdit::keyPressEvent(e);
                emit invalidInputKey(m_validstr, false);
            } else {
                e->ignore();
                emit invalidInputKey(m_hints, true);
            }
        }
    }
}


// virtual 
// void	myhexeditor::keyReleaseEvent(QKeyEvent * e) // Q_DECL_OVERRIDE;
// {
// 
// }
