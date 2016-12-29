include (../../shared.pri)


HEADERS       = += ./alignset.h \
    ./levmarmethods.h \
    ./mutual.h \
    ./parameters.h \
    ./shutils.h \
    ./solver.h \
    ./edit_mutualcorrs.h \
    ./edit_mutualcorrsDialog.h \
    ./edit_mutualcorrs_factory.h

SOURCES += ./alignset.cpp \
    ./edit_mutualcorrs.cpp \
    ./edit_mutualcorrsDialog.cpp \
    ./edit_mutualcorrs_factory.cpp \
    ./solver.cpp \
    ./levmarmethods.cpp \
    ./mutual.cpp \
    ./parameters.cpp

TARGET        = edit_mutualcorrs

RESOURCES     = edit_mutualcorrs.qrc

FORMS         = edit_mutualcorrsDialog.ui

win32-msvc2013:  LIBS += ../../external/lib/win32-msvc2013/levmar.lib
win32-msvc2015:  LIBS += ../../external/lib/win32-msvc2015/levmar.lib
win32-g++:LIBS += -L../../external -llevmar
linux-g++:LIBS += -L../../external/lib/linux-g++ -llevmar
linux-g++-32:LIBS += -L../../external/lib/linux-g++-32 -llevmar
linux-g++-64:LIBS += -L../../external/lib/linux-g++-64 -llevmar
macx:LIBS            += $$MACLIBDIR/liblevmar.a

