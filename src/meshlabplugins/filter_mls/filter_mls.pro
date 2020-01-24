include(../../shared.pri)

HEADERS += \
    apss.h \
    mlsmarchingcube.h \
    priorityqueue.h \
    balltree.h \
    mlsplugin.h \
    rimls.h \
    implicits.h \
    mlssurface.h \
    smallcomponentselection.h

SOURCES += \
    apss.cpp \
    balltree.cpp \
    mlsplugin.cpp \
    rimls.cpp \
    apss.tpp \
    mlssurface.tpp \
    rimls.tpp

TARGET = filter_mls

