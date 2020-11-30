
TEMPLATE = lib

CONFIG += staticlib c++11

SOURCES += rapidxml_main.cpp

HEADERS += rapidxml.hpp \
           rapidxml_iterators.hpp \
           rapidxml_print.hpp \
           rapidxml_utils.hpp

TARGET = XmlParser

