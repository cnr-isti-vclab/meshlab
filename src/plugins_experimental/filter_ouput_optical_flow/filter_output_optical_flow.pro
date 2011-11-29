include (../../shared.pri)


SOURCES = filter_output_optical_flow.cpp \
          DominancyClassifier.cpp \
          floatbuffer.cpp \
          utils.cpp \
          GPU/*.cpp

HEADERS = filter_output_optical_flow.h \
          DominancyClassifier.h \
          floatbuffer.h \
          OOCRaster.h \
          utils.h \
          GPU/*.h
          
RESOURCES += filter_output_optical_flow.qrc

TARGET = filter_output_optical_flow
