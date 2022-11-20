
TEMPLATE = subdirs

SUBDIRS = rapidxml \
          rapidjson \
          lua \
          Tree2FileOnly
          

CONFIG += ordered

Tree2FileOnly.subdir  = Tree2FileOnly
Tree2FileOnly.depends = rapidxml rapidjson lua

rapidxml.subdir = parser/rapidxml
rapidjson.subdir = parser/rapidjson
lua.subdir = parser/lua
