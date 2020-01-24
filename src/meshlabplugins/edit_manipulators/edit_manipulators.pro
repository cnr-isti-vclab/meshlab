include (../../shared.pri)

HEADERS += \
    edit_manipulators_factory.h \
    edit_manipulators.h

SOURCES += \
    edit_manipulators_factory.cpp \
    edit_manipulators.cpp \
    $$VCGDIR/wrap/gui/trackball.cpp \
    $$VCGDIR/wrap/gui/trackmode.cpp

RESOURCES += \
    edit_manipulators.qrc

TARGET = edit_manipulators

