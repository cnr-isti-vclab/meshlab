include (../../shared.pri)
ANNDIR = ../../external/ann_1.1.1
INCLUDEPATH += $$ANNDIR/include
LIBS += -L$$ANNDIR/lib \
    -lANN
QT += opengl
QT += xml
HEADERS =
		../filter_mls/apss.h \
    ../filter_mls/priorityqueue.h \
    ../filter_mls/balltree.h \
    ../filter_mls/implicits.h \
    ../filter_mls/mlssurface.h \
    ../filter_mls/kdtree.h \
    ../filter_mls/mlsutils.h \
    filter_feature_alignment.h \
    feature_alignment.h \
    feature_rgb.h \
    feature_msc.h
SOURCES = filter_feature_alignment.cpp \
    ../filter_mls/apss.cpp \
    ../filter_mls/balltree.cpp \
    ../filter_mls/apss.tpp \
    ../filter_mls/kdtree.cpp \
    ../filter_mls/mlssurface.tpp \
    ../edit_pickpoints/pickedPoints.cpp 
		
		TARGET = filter_feature_alignment
CONFIG += opengl
