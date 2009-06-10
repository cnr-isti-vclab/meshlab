include (../../shared.pri)
QT += opengl
QT += xml
HEADERS = filter_feature_alignment.h \
    feature_alignment.h \
    feature_rgb.h \
    feature_msc.h
SOURCES = filter_feature_alignment.cpp \
    feature_alignment.cpp \
    ../../meshlab/filterparameter.cpp \
    ../../meshlabplugins/edit_pickpoints/pickedPoints.cpp \
    $$GLEWCODE
TARGET = filter_feature_alignment
CONFIG += opengl
