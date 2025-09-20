#ifndef INPUTDETECHWIDGET_H
#define INPUTDETECHWIDGET_H

#include <QWidget>

class InputDetechWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InputDetechWidget(QWidget *parent = nullptr);

    virtual void testOnly();


protected:
    virtual  void  enterEvent(QEvent* event)  Q_DECL_OVERRIDE;
    virtual  void  leaveEvent(QEvent* event)  Q_DECL_OVERRIDE;
    virtual void   focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    virtual void   focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    virtual void   keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void   keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    virtual  void	paintEvent(QPaintEvent * event) Q_DECL_OVERRIDE;

// signals:
//     void onMouseEnter(InputDetechWidget* self);
//     void onMouseLeave(InputDetechWidget* self);

signals:

public slots:
};

#endif // INPUTDETECHWIDGET_H
