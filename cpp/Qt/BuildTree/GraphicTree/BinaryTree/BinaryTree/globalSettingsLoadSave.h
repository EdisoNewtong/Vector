#pragma once

#include <QVector>
#include <QByteArray>

#include "rapidxml.hpp" // for xml generte use


class GSettingLoadSave
{
public:
	static bool loadFromConfigFile(const QString& cfgFileName, QString* pErrorStr);
	static bool saveToConfigFile(const QString& cfgFileName, QString* pErrorStr);

protected:
    static QVector<QByteArray> s_XmlStringList;

    static QString s_strRenderCfg;
    static QString s_strGeo;
};
