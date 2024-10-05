
TEMPLATE = subdirs

SUBDIRS = rapidjson \
	      parseJson

CONFIG += ordered
parseJson.subdir = parseJson
parseJson.depends = rapidjson
rapidjson.subdir = rapidjson

# SUBDIRS += my_executable my_library
# my_executable.subdir = app
# my_executable.depends = my_library
# my_library.subdir = lib
