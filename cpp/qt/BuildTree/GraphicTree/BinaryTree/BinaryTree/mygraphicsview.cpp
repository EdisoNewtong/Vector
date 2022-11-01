#include "mygraphicsview.h"

#include <QScrollBar> 

MyGraphicsView::MyGraphicsView(QWidget* parent /* = nullptr */)
	: QGraphicsView(parent)
	, m_hvValue(0,0)
{
}

// virtual 
MyGraphicsView::~MyGraphicsView()
{
}


QPoint MyGraphicsView::getScrollValue()
{
	int x = m_hvValue.x();
    int y = m_hvValue.y();

	auto hBar = this->horizontalScrollBar();
	if ( hBar != nullptr ) {
		x = hBar->value();
	}

	auto vBar = this->verticalScrollBar();
	if ( vBar != nullptr ) {
		y = vBar->value();
	}

	m_hvValue.setX( x );
	m_hvValue.setY( y );

	return m_hvValue;
}

void MyGraphicsView::setScrollValue(const QPoint& pt)
{
	auto hBar = this->horizontalScrollBar();
	if ( hBar != nullptr ) {
		hBar->setValue( pt.x() );
	}

	auto vBar = this->verticalScrollBar();
	if ( vBar != nullptr ) {
		vBar->setValue( pt.y() );
	}

	m_hvValue.setX( pt.x() ); 
	m_hvValue.setY( pt.y() ); 
}
