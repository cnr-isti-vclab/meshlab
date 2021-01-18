#-------------------------------------------------
#
# Project created by QtCreator 2010-10-15T12:45:28
#
#-------------------------------------------------

include(../../../external/qtsoap-2.7_1/src/qtsoap.pri)

QT       += core script

QT       -= gui

TARGET = SoapTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

HEADERS += \
    handler.h
