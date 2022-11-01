#include "mytextanimationitem.h"
#include "treenode.h"

#include <QDebug>


MyTextAnimationItem::MyTextAnimationItem(QTextEdit* pEdit, const QVector<treenode*>& nodeList, const QString& prefix)
	: m_pRichTextEdit( pEdit )
	, m_travelsalNodeList( nodeList )
	, m_step(-1, 0)
	, m_spanList()
	, m_normalColor( Qt::black )
	, m_processingColor( Qt::green )
	, m_nodeFinishedColor( Qt::red )
	, m_normalFontSize( 20 )
	, m_highLightFontSize( 25 )
{
	m_spanList.clear();
	m_spanList << prefix;
}


// virtual 
MyTextAnimationItem::~MyTextAnimationItem()
{
}


QPoint MyTextAnimationItem::step()
{
	return m_step;
}

void MyTextAnimationItem::setStep(const QPoint& stp)
{
	// static int G_CNT = 1;
	// qDebug() << G_CNT++ << ". " << stp;
	
	auto oldX = m_step.x();
	auto newX = stp.x();
	auto oldY = m_step.y();
	auto newY = stp.y();

	auto bChange_x = ( oldX != newX );
	auto bChange_y = ( oldY != newY );

	
	// y = 1 :   is changing
	// y = 0 :   finish transition
    if ( m_pRichTextEdit != nullptr ) {
		if ( bChange_x  ) {
			if ( newX == 0 ) {
                treenode* firstNd = m_travelsalNodeList.at(newX);
				if ( firstNd != nullptr ) {
					QString spanElement = QString(R"(<span style="color:%1; font-size:%2px; "><b>%3</b></span>)")
											.arg( m_nodeFinishedColor.name() )
											.arg( m_highLightFontSize )
											.arg( firstNd->text() );

					m_spanList << spanElement;
                    m_pRichTextEdit->setHtml( buildRichText() );
				}
			} else {
				// newX >=1
                treenode* previousNode = m_travelsalNodeList.at(newX-1);

				QString previousSpanElement = QString(R"(<span style="color:%1; font-size:%2px; ">%3</span>)")
												.arg( m_normalColor.name() )
												.arg( m_normalFontSize )
												.arg( previousNode->text() );

				QString afterReplaced = replaceLastText( previousSpanElement );

				QString underProcess = QString(R"(<span style="color:%1; font-size:%2px; "><b> --> </b></span>)")
											.arg( m_processingColor.name() )
											.arg( m_highLightFontSize );

                m_spanList << underProcess;
                m_pRichTextEdit->setHtml( afterReplaced + underProcess );
			}
		} else if ( bChange_y ) {
			// internal transition is running
            treenode* currentNode  = m_travelsalNodeList.at(newX);
			QString previousProcessingSpanElement = QString(R"(<span style="color:%1; font-size:%2px; "> --> </span>)")
														.arg( m_normalColor.name() )
														.arg( m_normalFontSize );

			QString afterReplaced = replaceLastText( previousProcessingSpanElement );

			QString currentSpanElement = QString(R"(<span style="color:%1; font-size:%2px; "><b>%3</b></span>)")
											.arg( m_nodeFinishedColor.name() )
											.arg( m_normalFontSize )
											.arg( currentNode->text() );

			m_spanList << currentSpanElement;
            m_pRichTextEdit->setHtml( afterReplaced + currentSpanElement);
		}
	}
	
	m_step = stp;
}





QString  MyTextAnimationItem::buildRichText()
{
	QString retStr;
	for( const auto& e : m_spanList ) {
		retStr += e;
	}	
	return retStr;
}


QString  MyTextAnimationItem::replaceLastText(const QString& placement)
{
	if ( m_spanList.size() >= 2 ) {
		m_spanList.removeLast();
		m_spanList << placement;
	}

	return buildRichText();
}


