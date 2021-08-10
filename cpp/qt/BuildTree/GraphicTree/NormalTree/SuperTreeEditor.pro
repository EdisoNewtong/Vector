
TEMPLATE = subdirs

SUBDIRS = SuperTreeEditor \
          XmlParser \
          rapidjson
          

CONFIG += orderd

SuperTreeEditor.subdir  = SuperTreeEditor
SuperTreeEditor.depends = XmlParser rapidjson

XMLParser.subdir = XMLParser
rapidjson.subdir = rapidjson
