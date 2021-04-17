include (../../shared.pri)

HEADERS += \
    meshfilter_M.h

SOURCES += \
    meshfilter_M.cpp
	
RESOURCES += \
    filter_meshing_M.qrc \

TARGET = filter_meshing_M

win32-msvc:QMAKE_CXXFLAGS = /bigobj
