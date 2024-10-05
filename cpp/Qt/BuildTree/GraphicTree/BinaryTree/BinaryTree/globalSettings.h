#pragma once

#include <QBrush>
#include <QPen>
#include <QFont>

namespace GlobalSetting
{
	extern const int  SPECIAL_COLUMN_INDEX;

	//
	// For XML read/write
	//
	extern const QString ROOT_TAG;
	extern const QString LEFT_TAG;
	extern const QString RIGHT_TAG;
	extern const QString TEXT_TAG;

	extern const QString GLOBAL_SETTINGS_TAG;
	 extern const QString SCENE_BG_TAG;
	 extern const QString RADIUS;
	 extern const QString LR_GAP1;
	 extern const QString RL_GAP2;
	 extern const QString LAYER_GAP;
	 extern const QString LEFT_MARGIN_TAG;
	 extern const QString RIGHT_MARGIN_TAG;
	 extern const QString TOP_MARGIN_TAG;
	 extern const QString BOTTOM_MARGIN_TAG;

	extern const QString CIRCLE_COLOR_TAG;
	extern const QString CIRCLE_PEN_COLOR_TAG;
	extern const QString CIRCLE_PEN_WIDTH_TAG;
	//
	extern const QString TEXT_FONT_NAME_TAG;
	extern const QString TEXT_FONT_SIZE_TAG;
	extern const QString TEXT_COLOR_TAG;
	//
	extern const QString CONNECTION_LINE_COLOR_TAG;
	extern const QString CONNECTION_LINE_WIDTH_TAG;
	///////////////////////////////////////////////////////


	extern const QBrush SCENE_BG;

	extern const QBrush CIRCLE_BRUSH;
	extern const QPen CIRCLE_OUTLINE;
	extern const QFont TEXT_FONT;
	extern const QBrush TEXT_COLOR;
	extern const QPen CONNECTION_LINE;


	extern const qreal CIRCLE_RADIUS;
	extern const qreal DISTANCE_BETWEEN_LEFTRIGHT;
	extern const qreal DISTANCE_BETWEEN_RIGHT__LEFT;
	extern const qreal HEIGHT_BETWEEN_PARENT_AND_CHILDREN;


	// margin
	extern const qreal LEFT_MARGIN;
	extern const qreal RIGHT_MARGIN;
	extern const qreal TOP_MARGIN;
	extern const qreal BOTTOM_MARGIN;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////
	extern QBrush scene_bg;

	extern QBrush circle_brush;
	extern QPen circle_outline;
	extern QFont text_font;
	extern QBrush text_color;
	extern QPen connection_line;


	extern qreal circle_radius;
	extern qreal distance_between_leftright;
	extern qreal distance_between_right__left;
	extern qreal height_between_parent_and_children;

	// margin
	extern qreal left_margin;
	extern qreal right_margin;
	extern qreal top_margin;
	extern qreal bottom_margin;

	extern void resetToDefault();
	
}
