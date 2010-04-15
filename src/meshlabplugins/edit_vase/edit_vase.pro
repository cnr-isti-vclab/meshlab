include (../../shared.pri)

HEADERS += edit_vase.h
HEADERS += fieldinterpolator.h
HEADERS += volume.h
HEADERS += vasewidget.h
HEADERS += gridaccell.h
HEADERS += vase_utils.h
HEADERS += balloon.h
HEADERS += myheap.h
HEADERS += OLD.h


SOURCES += edit_vase.cpp
SOURCES += fieldinterpolator.cpp
SOURCES += volume.cpp
SOURCES += vasewidget.cpp
SOURCES += gridaccell.cpp
SOURCES += vase_utils.cpp
SOURCES += balloon.cpp

# EIGEN + CHOLMOD STUFF
INCLUDEPATH += /sw/include/eigen3 /sw/include/suitesparse
LIBS += -L/sw/lib -lamd -lcamd -lccolamd -lcholmod -lcolamd -lcxsparse -lblas
# INCLUDEPATH += /opt/local/include/eigen3 /opt/local/include/ufsparse
# LIBS += -L/opt/local/lib -lamd -lcamd -lccolamd -lcholmod -lcolamd -lcsparse -lcxsparse -lmetis -lblas

TARGET = edit_vase
QT += opengl
RESOURCES = meshlab.qrc
FORMS = vasewidget.ui
