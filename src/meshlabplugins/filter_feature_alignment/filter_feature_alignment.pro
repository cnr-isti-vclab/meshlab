include (../../shared.pri)
ANNDIR = ../../external/ann_1.1.1
INCLUDEPATH += $$ANNDIR/include
LIBS += -L$$ANNDIR/lib \
    -lANN
QT += opengl
QT += xml
HEADERS =   ../../meshlab/interfaces.h \
            ../../meshlab/meshmodel.h \
            apss.h \
                mlsmarchingcube.h \
                priorityqueue.h \
                balltree.h \
                rimls.h \
                implicits.h \
                mlssurface.h \
                smallcomponentselection.h \
                kdtree.h \
                mlsutils.h \
            filter_feature_alignment.h \
            feature_alignment.h \
            feature_rgb.h \
            feature_msc.h
SOURCES =   filter_feature_alignment.cpp \
            apss.cpp \
                balltree.cpp \
                rimls.cpp \
                apss.tpp \
                kdtree.cpp \
                mlssurface.tpp \
                rimls.tpp \
            ../../meshlab/filterparameter.cpp \
            ../../meshlab/meshmodel.cpp \
            ../edit_pickpoints/pickedPoints.cpp \
            $$GLEWCODE
TARGET = filter_feature_alignment
CONFIG += opengl
