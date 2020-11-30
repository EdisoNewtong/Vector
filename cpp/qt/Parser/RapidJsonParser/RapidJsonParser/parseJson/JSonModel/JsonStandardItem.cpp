#include "JsonStandardItem.h"


JSonStandardItem::JSonStandardItem(const QString& text, rapidjson::Value*  jsonValue)
	: QStandardItem(text)
	, m_pJsonValue(jsonValue)
{
}

// virtual 
JSonStandardItem::~JSonStandardItem()
{
}


rapidjson::Value* JSonStandardItem::getJsonValue()
{
	return m_pJsonValue;
}
