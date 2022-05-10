#include <QStyleOption>
#include <QPainter>

#include "MyWidget.h"

MyWidget::MyWidget(QWidget* parent, int tag)
	: QWidget(parent)
    , m_tag(tag)
{
    this->setStyleSheet("QWidget { background : rgb(240,240,240); }" );
}


// virtual 
MyWidget::~MyWidget()
{
}


// virtual  
void  MyWidget::enterEvent(QEvent* event)  // Q_DECL_OVERRIDE;
{
	QWidget::enterEvent(event);
	emit onMouseEnter(this);
}

// virtual  
void  MyWidget::leaveEvent(QEvent* event)  // Q_DECL_OVERRIDE;
{
    QWidget::leaveEvent(event);
	emit onMouseLeave(this);
}


// virtual
void	MyWidget::paintEvent(QPaintEvent * event) // Q_DECL_OVERRIDE;
{
    Q_UNUSED(event)

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


int MyWidget::getTag()
{
    return m_tag;
}
