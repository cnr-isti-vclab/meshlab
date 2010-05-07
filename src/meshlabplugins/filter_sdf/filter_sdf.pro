include (../../shared.pri)
HEADERS += filterinterface.h
HEADERS += mysampling.h

# Original plugin
HEADERS += filter_sdf.h

#--- Choose only one of the following two
# the "+" version does much more than computing
# the SDF by applying a verification step.
SOURCES += filter_sdf.cpp
# SOURCES += filter_sdf+.cpp

TARGET = filter_sdf
