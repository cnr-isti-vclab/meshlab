include (../../shared.pri)

QTSOAPDIR = ../../external/qtsoap-2.7_1/src

INCLUDEPATH += $$QTSOAPDIR

HEADERS       += filter_photosynth.h \
                 synthData.h \
                 $$QTSOAPDIR/qtsoap.h

SOURCES       += filter_photosynth.cpp \
                 downloader.cpp \
                 $$QTSOAPDIR/qtsoap.cpp

TARGET         = filter_photosynth

QT += network
