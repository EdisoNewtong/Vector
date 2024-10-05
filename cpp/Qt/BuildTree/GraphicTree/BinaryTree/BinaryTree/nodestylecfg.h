#pragma once

#include <QBrush>
#include <QPen>
#include <QFont>


class nodeStyleCfg
{
public:
	// circle
	QBrush m_circleBrushColor;
	QPen   m_circlePen;

	// text
	QFont    m_textFont;
	QBrush   m_textBrush;

	// line
	QPen   m_connectionLinePen;

public:
	void reset2Default();


public:
	static const nodeStyleCfg& DefaultCfg(); 

    static const nodeStyleCfg& CurrentCfg();
};
