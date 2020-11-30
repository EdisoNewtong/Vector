
TEMPLATE = subdirs

SUBDIRS = XMLParser \
          XMLDisplay

CONFIG += orderd

XMLDisplay.subdir = XMLDisplay
XMLDisplay.depends = XMLParser
XMLParser.subdir = XMLParser
