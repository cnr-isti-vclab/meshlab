include (../../shared.pri)
HEADERS += edit_vase.h
HEADERS += edit_vase_factory.h
HEADERS += OLD.h
HEADERS += fieldinterpolator.h
HEADERS += volume.h
HEADERS += vasewidget.h
HEADERS += gridaccell.h
HEADERS += vase_utils.h
HEADERS += balloon.h
HEADERS += myheap.h
HEADERS += ../filter_plymc/plymc.h

SOURCES += edit_vase.cpp
SOURCES += edit_vase_factory.cpp
SOURCES += fieldinterpolator.cpp
SOURCES += volume.cpp
SOURCES += vasewidget.cpp
SOURCES += gridaccell.cpp
SOURCES += vase_utils.cpp
SOURCES += balloon.cpp

#--- SUPPORT FOR TRACKBALL
VCGDIR = ../../../../vcglib
HEADERS += $$VCGDIR/wrap/gui/trackball.h
HEADERS += $$VCGDIR/wrap/gui/trackmode.h
SOURCES += $$VCGDIR/wrap/gui/trackball.cpp
SOURCES += $$VCGDIR/wrap/gui/trackmode.cpp

# EIGEN + CHOLMOD STUFF
INCLUDEPATH += /sw/include/eigen3
INCLUDEPATH += /sw/include/suitesparse
LIBS += -L/sw/lib -lamd -lcamd -lccolamd -lcholmod -lcolamd -lcxsparse -lblas

TARGET = edit_vase
QT += opengl
RESOURCES = meshlab.qrc
FORMS = vasewidget.ui
