
TEMPLATE = subdirs

SUBDIRS = XMLParser \
          XMLDisplay

CONFIG += ordered

XMLDisplay.subdir = XMLDisplay
XMLDisplay.depends = XMLParser
XMLParser.subdir = XMLParser
