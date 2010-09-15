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

#--- EIGEN + CHOLMOD STUFF
# Note that the current version of Eigen is buggy, consequently, 
# the one that ships with meshlab is buggy as well!!
# The define below change the include directives to use the 
# development version of Eigen, where the bug has been fixed
DEFINES += USE_EIGEN_LOCAL
DEFINES += REMOVE_DEGENERATE_FACES

#--- DEBUG MODE
# Toggles many allocations
DEFINES += DEBUG

INCLUDEPATH += /sw/include/eigen3
INCLUDEPATH += /sw/include/suitesparse
LIBS += -L/sw/lib -lamd -lcamd -lccolamd -lcholmod -lcolamd -lcxsparse -lblas

TARGET = edit_vase
QT += opengl
RESOURCES = meshlab.qrc
FORMS = vasewidget.ui
