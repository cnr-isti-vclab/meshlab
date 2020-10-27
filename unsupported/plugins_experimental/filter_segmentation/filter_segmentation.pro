include (../../shared.pri)

HEADERS += meshsegmentation.h \
           Cluster.h \
           Segmenter.h \
           HC_Segmenter.h \
           RG_Segmenter.h \
           Utils.h      

               
SOURCES += meshsegmentation.cpp \
           Segmenter.cpp \
           HC_Segmenter.cpp \
           RG_Segmenter.cpp \
           Utils.cpp

TARGET   = filter_segmentation

