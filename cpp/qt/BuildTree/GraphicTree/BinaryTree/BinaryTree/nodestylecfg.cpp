#include "nodestylecfg.h"


void nodeStyleCfg::reset2Default()
{

}


// static 
const nodeStyleCfg& nodeStyleCfg::DefaultCfg()
{
	static nodeStyleCfg dftCfg;
	dftCfg.m_circleBrushColor = QBrush( Qt::red );
	dftCfg.m_circlePen = QPen( QBrush(Qt::green), 2.5  );
	dftCfg.m_textFont =  QFont( QString("SimSun"), 9 );
	dftCfg.m_textBrush = QBrush( Qt::black ) ;
	dftCfg.m_connectionLinePen = QPen( QBrush(Qt::blue), 3);

	return dftCfg;
}
