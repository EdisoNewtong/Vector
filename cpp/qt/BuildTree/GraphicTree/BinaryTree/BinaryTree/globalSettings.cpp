#include "globalSettings.h"

namespace GlobalSetting
{

	extern const int  SPECIAL_COLUMN_INDEX = 2;

	extern const QString ROOT_TAG("root");
	extern const QString LEFT_TAG("left");
	extern const QString RIGHT_TAG("right");
	extern const QString TEXT_TAG("text");

    extern const QString GLOBAL_SETTINGS_TAG("globalsettings");
	 extern const QString SCENE_BG_TAG("sceneBg");
	 extern const QString RADIUS("r");
	 extern const QString LR_GAP1("leftRightNodeGap1");
	 extern const QString RL_GAP2("rightLeftNodeGap2");
	 extern const QString LAYER_GAP("parentChildGap");
	 extern const QString LEFT_MARGIN_TAG("leftMargin");
	 extern const QString RIGHT_MARGIN_TAG("rightMargin");
	 extern const QString TOP_MARGIN_TAG("topMargin");
	 extern const QString BOTTOM_MARGIN_TAG("bottomMargin");

	extern const QString CIRCLE_COLOR_TAG("circleColor");
	extern const QString CIRCLE_PEN_COLOR_TAG("circlePenColor");
	extern const QString CIRCLE_PEN_WIDTH_TAG("circlePenWidth");
	//
	extern const QString TEXT_FONT_NAME_TAG("textFontName");
	extern const QString TEXT_FONT_SIZE_TAG("textFontSize");
	extern const QString TEXT_COLOR_TAG("textColor");
	//
	extern const QString CONNECTION_LINE_COLOR_TAG("connectionLineColor");
	extern const QString CONNECTION_LINE_WIDTH_TAG("connectionLineWidth");


	extern const QBrush SCENE_BG = QBrush(Qt::white);
	//
	extern const QBrush CIRCLE_BRUSH = QBrush(Qt::red);
    extern const QPen CIRCLE_OUTLINE = QPen( QBrush(Qt::green), 2.5 );
	extern const QFont TEXT_FONT = QFont("SimSun", 9);
    extern const QBrush TEXT_COLOR = QBrush(Qt::black);
    extern const QPen CONNECTION_LINE = QPen( QBrush(Qt::blue), 3);


	extern const qreal CIRCLE_RADIUS = 40.0;
    extern const qreal DISTANCE_BETWEEN_LEFTRIGHT = 20.0;
    extern const qreal DISTANCE_BETWEEN_RIGHT__LEFT = 35.0;
	extern const qreal HEIGHT_BETWEEN_PARENT_AND_CHILDREN = 23.0;

	extern const qreal LEFT_MARGIN = 20.0;
	extern const qreal RIGHT_MARGIN = 20.0;
	extern const qreal TOP_MARGIN = 25.0;
	extern const qreal BOTTOM_MARGIN = 25.0;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/*extern*/ QBrush scene_bg = SCENE_BG;
	
	/*extern*/ QBrush circle_brush = CIRCLE_BRUSH;
	/*extern*/ QPen circle_outline = CIRCLE_OUTLINE;
	/*extern*/ QFont text_font = TEXT_FONT;
	/*extern*/ QBrush text_color = TEXT_COLOR;
	/*extern*/ QPen connection_line = CONNECTION_LINE;


	/*extern*/ qreal circle_radius = CIRCLE_RADIUS;
	/*extern*/ qreal distance_between_leftright = DISTANCE_BETWEEN_LEFTRIGHT;
	/*extern*/ qreal distance_between_right__left = DISTANCE_BETWEEN_RIGHT__LEFT;
	/*extern*/ qreal height_between_parent_and_children = HEIGHT_BETWEEN_PARENT_AND_CHILDREN;

	// margin
	/*extern*/ qreal left_margin = LEFT_MARGIN;
	/*extern*/ qreal right_margin = RIGHT_MARGIN;
	/*extern*/ qreal top_margin = TOP_MARGIN;
	/*extern*/ qreal bottom_margin = BOTTOM_MARGIN;

	extern void resetToDefault()
	{
		scene_bg = SCENE_BG;

		circle_brush = CIRCLE_BRUSH;
		circle_outline = CIRCLE_OUTLINE;
		text_font     = TEXT_FONT;
		text_color  = TEXT_COLOR;
		connection_line = CONNECTION_LINE;

		circle_radius = CIRCLE_RADIUS;
		distance_between_leftright = DISTANCE_BETWEEN_LEFTRIGHT;
		distance_between_right__left = DISTANCE_BETWEEN_RIGHT__LEFT;
		height_between_parent_and_children = HEIGHT_BETWEEN_PARENT_AND_CHILDREN;

		// margin
		left_margin = LEFT_MARGIN;
		right_margin = RIGHT_MARGIN;
		top_margin = TOP_MARGIN;
		bottom_margin = BOTTOM_MARGIN;
	}

}
