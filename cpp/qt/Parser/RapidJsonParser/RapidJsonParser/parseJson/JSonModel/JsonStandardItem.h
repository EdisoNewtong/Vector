#ifndef JSON_STANDARD_ITEM_H
#define JSON_STANDARD_ITEM_H

#include "rapidjson/document.h"
#include <QStandardItem>

class JSonStandardItem : public QStandardItem
{
public:
	JSonStandardItem(const QString& text, rapidjson::Value*  jsonValue);
	virtual ~JSonStandardItem();

	rapidjson::Value* getJsonValue();
protected:
	rapidjson::Value*  m_pJsonValue;
};

#endif
