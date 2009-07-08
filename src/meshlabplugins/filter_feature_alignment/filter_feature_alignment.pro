include (../../shared.pri)
ANNDIR = ../../external/ann_1.1.1
INCLUDEPATH += $$ANNDIR/include
LIBS += -L$$ANNDIR/lib \
    -lANN
QT += opengl
QT += xml
HEADERS = ../../meshlab/interfaces.h \
    filter_feature_alignment.h \
    feature_alignment.h \
    feature_rgb.h \
    feature_msc.h
SOURCES = filter_feature_alignment.cpp \
    ../../meshlab/filterparameter.cpp \
    ../../meshlabplugins/edit_pickpoints/pickedPoints.cpp \
    $$GLEWCODE
TARGET = filter_feature_alignment
CONFIG += opengl
