include (../../shared.pri)

# Lib name
TARGET = editstraightener

# Lib sources
SOURCES += $$VCGDIR/wrap/gui/trackball.cpp
SOURCES += $$VCGDIR/wrap/gui/trackmode.cpp 
SOURCES += $$VCGDIR/wrap/gui/coordinateframe.cpp 
SOURCES += $$VCGDIR/wrap/gui/activecoordinateframe.cpp 
SOURCES += $$VCGDIR/wrap/gui/rubberband.cpp 

# Compile glew
SOURCES += $$GLEWCODE
QT += opengl
# Input
HEADERS += editstraightener.h
HEADERS += editstraightenerdialog.h

SOURCES += editstraightener.cpp
SOURCES += editstraightenerdialog.cpp

FORMS += editstraightener.ui

RESOURCES += editstraightener.qrc
