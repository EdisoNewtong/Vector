#pragma once

#include <QTextEdit>
#include <QVector>
#include <QStringList>
#include <QPoint>
#include <QColor>

#include "treenode.h"


class MyTextAnimationItem : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QPoint step READ step WRITE setStep)

public:
	MyTextAnimationItem(QTextEdit* pEdit, const QVector<treenode*>& nodeList, const QString& prefix);
	virtual ~MyTextAnimationItem();

	QPoint step();
	void setStep(const QPoint& stp);

protected:
	QTextEdit*            m_pRichTextEdit;
	QVector<treenode*>    m_travelsalNodeList;

	QPoint                m_step;
	QStringList           m_spanList;

	const QColor          m_normalColor;
	const QColor          m_processingColor;
	const QColor          m_nodeFinishedColor;
	const int             m_normalFontSize;
	const int             m_highLightFontSize;


	QString               buildRichText();
	QString               replaceLastText(const QString& placement);

};
