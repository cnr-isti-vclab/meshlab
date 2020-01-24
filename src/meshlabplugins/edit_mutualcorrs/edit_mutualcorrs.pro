include (../../shared.pri)

HEADERS += \
    alignset.h \
    levmarmethods.h \
    mutual.h \
    parameters.h \
    shutils.h \
    solver.h \
    edit_mutualcorrs.h \
    edit_mutualcorrsDialog.h \
    edit_mutualcorrs_factory.h

SOURCES += \
    alignset.cpp \
    edit_mutualcorrs.cpp \
    edit_mutualcorrsDialog.cpp \
    edit_mutualcorrs_factory.cpp \
    solver.cpp \
    levmarmethods.cpp \
    mutual.cpp \
    parameters.cpp

RESOURCES += \
    edit_mutualcorrs.qrc

FORMS += \
    edit_mutualcorrsDialog.ui

TARGET = edit_mutualcorrs

INCLUDEPATH *= \
    $${MESHLAB_EXTERNAL_DIRECTORY}/levmar-2.3 \
    $${MESHLAB_EXTERNAL_DIRECTORY}/newuoa/include

win32-msvc:LIBS += $${MESHLAB_DISTRIB_DIRECTORY}/lib/win32-msvc/levmar.lib
win32-g++:LIBS += -L$${MESHLAB_DISTRIB_DIRECTORY}/lib/win32-gcc -llevmar
macx:LIBS+= $${MESHLAB_DISTRIB_DIRECTORY}/lib/macx/liblevmar.a
linux-g++:LIBS += -llevmar
