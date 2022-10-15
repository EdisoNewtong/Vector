#include "nodestylecfg.h"
#include "globalSettings.h"


void nodeStyleCfg::reset2Default()
{
	m_circleBrushColor = GlobalSetting::circle_brush;
	m_circlePen = GlobalSetting::circle_outline;
	m_textFont =  GlobalSetting::text_font;
	m_textBrush = GlobalSetting::text_color;
	m_connectionLinePen = GlobalSetting::connection_line;
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