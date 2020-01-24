include (../../shared.pri)

HEADERS += \
    filter_fractal.h \
    fractal_utils.h \
    craters_utils.h \
    filter_functors.h

SOURCES += \
    filter_fractal.cpp

RESOURCES += \
    ff_res.qrc

OTHER_FILES += \
    ff_fractal_description.txt \
    ff_craters_description.txt

TARGET = filter_fractal
