#pragma once

#include <QBrush>
#include <QPen>
#include <QFont>

namespace GlobalSetting
{
	extern const int  SPECIAL_COLUMN_INDEX;
	extern const bool SPECIAL_COLUMN_EDITABLE;

	extern const QBrush CIRCLE_BRUSH;
	extern const QPen CIRCLE_OUTLINE;
	extern const QFont TEXT_FONT;
	extern const QBrush  TEXT_COLOR;


	extern const qreal CIRCLE_RADIUS;
	extern const qreal DISTANCE_BETWEEN_LEFTRIGHT;
	extern const qreal HEIGHT_BETWEEN_PARENT_AND_CHILDREN;


	// margin
	extern const qreal LEFT_MARGIN;
	extern const qreal RIGHT_MARGIN;
	extern const qreal TOP_MARGIN;
	extern const qreal BOTTOM_MARGIN;
	
}