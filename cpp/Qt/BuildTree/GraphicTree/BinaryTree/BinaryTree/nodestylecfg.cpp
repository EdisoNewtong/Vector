#include "nodestylecfg.h"
#include "globalSettings.h"


void nodeStyleCfg::reset2Default()
{
	m_circleBrushColor = GlobalSetting::CIRCLE_BRUSH;
	m_circlePen = GlobalSetting::CIRCLE_OUTLINE;
	m_textFont =  GlobalSetting::TEXT_FONT;
	m_textBrush = GlobalSetting::TEXT_COLOR;
    m_connectionLinePen = GlobalSetting::CONNECTION_LINE;
}


// static 
const nodeStyleCfg& nodeStyleCfg::DefaultCfg()
{
	static nodeStyleCfg dftCfg;
	dftCfg.m_circleBrushColor = GlobalSetting::circle_brush;
	dftCfg.m_circlePen = GlobalSetting::circle_outline;
	dftCfg.m_textFont =  GlobalSetting::text_font;
	dftCfg.m_textBrush = GlobalSetting::text_color;
	dftCfg.m_connectionLinePen = GlobalSetting::connection_line;

	return dftCfg;
}



