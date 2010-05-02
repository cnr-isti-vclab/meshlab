include (../../shared.pri)
HEADERS += filter_sdf.h

HEADERS += filterinterface.h
HEADERS += calculatesdf.h
HEADERS += rayintersect.h
HEADERS += tnt/tnt_array1d.h
HEADERS += tnt/tnt_array2d.h
HEADERS += tnt/tnt_array3d.h

SOURCES += rayintersect.cpp
SOURCES += calculatesdf.cpp
SOURCES += filter_sdf.cpp

OTHER_FILES += readme.txt

TARGET = filter_sdf
