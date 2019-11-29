include (../../shared.pri)

HEADERS     += filter_multiscale_align.h \
               multiscale_align.h \
               struct.h \
               utils.h \
               generic_align.h 
               

SOURCES     += filter_multiscale_align.cpp \
               multiscale_align.cpp \
               generic_align.cpp 
               		
TARGET       = filter_multiscale_align

## Dependencies
INCLUDEPATH += lib
!CONFIG(system_eigen3): INCLUDEPATH *= $$VCGDIR/eigenlib   #needed by Grenaille
CONFIG(system_eigen3):  INCLUDEPATH *= $$EIGENDIR   #needed by Grenaille


unix: QMAKE_CXXFLAGS += -DQ_OS_LINUX

# OpenMP
LIBS += -fopenmp 
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -fopenmp
#QMAKE_CXXFLAGS_RELEASE += -g
#QMAKE_CXXFLAGS_RELEASE -= -O2
