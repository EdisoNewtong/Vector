
TEMPLATE = subdirs

SUBDIRS = rapidxml \
          rapidjson \
          lua \
          SuperTreeEditor
          

CONFIG += ordered

SuperTreeEditor.subdir  = SuperTreeEditor
SuperTreeEditor.depends = rapidxml rapidjson lua

rapidxml.subdir = parser/rapidxml
rapidjson.subdir = parser/rapidjson
lua.subdir = parser/lua
