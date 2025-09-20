#include <QStyleOption>
#include <QPainter>
#include <QKeyEvent>

#include "InputDetechWidget.h"
#include <QDebug>

InputDetechWidget::InputDetechWidget(QWidget *parent) : QWidget(parent)
{
    this->setEnabled(true);
    this->setStyleSheet("QWidget { background : rgb(175,0,0); }" );
    this->setFocusPolicy( Qt::ClickFocus );
}


void InputDetechWidget::testOnly()
{
    qDebug() << "Hello World";
}


// virtual
void  InputDetechWidget::enterEvent(QEvent* event)  // Q_DECL_OVERRIDE;
{
    QWidget::enterEvent(event);
    qDebug() << "On Enter : set Color as [Blue]";

    this->setStyleSheet("QWidget { background : rgb(0,0,180); }" );

    // emit onMouseEnter(this);
}

// virtual
void  InputDetechWidget::leaveEvent(QEvent* event)  // Q_DECL_OVERRIDE;
{
    QWidget::leaveEvent(event);
    this->setFocus( Qt::NoFocusReason );

    // qDebug() << "Leave Out : set Color as [Default] ";
    // this->setStyleSheet("QWidget { background : rgb(175,0,0); }" );

    // emit onMouseLeave(this);
}

void InputDetechWidget::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event)
    qDebug() << "Focus In : set Color as [Green] ";
    this->setStyleSheet("QWidget { background : rgb(0,200,0); }" );
}


void InputDetechWidget::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event)

    qDebug() << "Focus out : Set Color as [Default] ";
    this->setStyleSheet("QWidget { background : rgb(175,0,0); }" );
}

void   InputDetechWidget::keyPressEvent(QKeyEvent *event) // Q_DECL_OVERRIDE;
{
    Q_UNUSED(event)
    qDebug() << "[Green Widget] : Key Press , Content : " << event->text();

}

void   InputDetechWidget::keyReleaseEvent(QKeyEvent *event) // Q_DECL_OVERRIDE;
{
    qDebug() << "[Green Widget] : Key Release , Content : " << event->text();
}




// virtual
void	InputDetechWidget::paintEvent(QPaintEvent * event) // Q_DECL_OVERRIDE;
{
    Q_UNUSED(event)

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


