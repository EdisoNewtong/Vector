
TEMPLATE = subdirs

SUBDIRS = SuperTreeEditor \
          rapidxml \
          rapidjson
          

CONFIG += orderd

SuperTreeEditor.subdir  = SuperTreeEditor
SuperTreeEditor.depends = rapidxml rapidjson

rapidxml.subdir = parser/rapidxml
rapidjson.subdir = parser/rapidjson
