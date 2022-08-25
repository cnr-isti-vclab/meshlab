include (../../shared.pri)

# Lib name
TARGET = edit_straightener

# Lib sources
SOURCES += $$VCGDIR/wrap/gui/trackball.cpp \
			$$VCGDIR/wrap/gui/trackmode.cpp \
			$$VCGDIR/wrap/gui/coordinateframe.cpp \
			$$VCGDIR/wrap/gui/activecoordinateframe.cpp \
			$$VCGDIR/wrap/gui/rubberband.cpp

# Input
HEADERS  += edit_straightener_factory.h \
			editstraightener.h \
			editstraightenerdialog.h

SOURCES  += edit_straightener_factory.cpp \
			editstraightener.cpp \
			editstraightenerdialog.cpp
			
QT += opengl

FORMS += editstraightener.ui

RESOURCES += editstraightener.qrc
