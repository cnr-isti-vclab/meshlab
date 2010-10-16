include (../../shared.pri)
# THIS IS TEMPORARY
# WARNING:
# in order to properly compile this plugin, you need to download and install the qtsoap package
# http://qt.nokia.com/products/appdev/add-on-products/catalog/4/Utilities/qtsoap/
# and modify the line below according to your installation directory
include(C:\Qt\qtsoap-2.7_1-opensource\src\qtsoap.pri)

HEADERS       += filter_photosynth.h \
    synthData.h
SOURCES       += filter_photosynth.cpp \ 
    downloader.cpp
TARGET        = filter_photosynth
TEMPLATE      = lib
CONFIG       += plugin
