include(../ext_common.pri)
TEMPLATE = lib
DEFINES += _UNICODE
TARGET = muparser
CONFIG += staticlib
DEPENDPATH += src/
INCLUDEPATH += include/
macx:QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-format -Wno-pointer-sign -Wno-deprecated-declarations\
  -Wno-deprecated-register -Wno-delete-non-virtual-dtor -Wno-logical-not-parentheses -Wno-switch -Wno-unknown-pragmas
macx:QMAKE_CXXFLAGS_WARN_ON = 


# Input
SOURCES += \
    src/muParser.cpp \
    src/muParserBase.cpp \
    src/muParserBytecode.cpp \
    src/muParserCallback.cpp \
    src/muParserDLL.cpp \
    src/muParserError.cpp \
    src/muParserInt.cpp \
    src/muParserTest.cpp \
    src/muParserTokenReader.cpp
