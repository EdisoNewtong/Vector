#ifndef MY_HEX_EDITOR_H
#define MY_HEX_EDITOR_H

#include <QTextEdit>
#include <QSet>

class myhexeditor : public QTextEdit
{
    Q_OBJECT

public:
    explicit myhexeditor(QWidget* parent = nullptr);
    virtual ~myhexeditor();

signals:
    void invalidInputKey(const QString& hints,bool errorFlag);
    
protected:
    virtual void	keyPressEvent(QKeyEvent * e) Q_DECL_OVERRIDE;

    // virtual void	keyReleaseEvent(QKeyEvent * e) Q_DECL_OVERRIDE;
private:
    QSet<int>       m_validKeyboardkeys;
    QString         m_hints;
    QString         m_validstr;
};

#endif
