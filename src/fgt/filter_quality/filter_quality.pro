include (../../shared.pri)

HEADERS       = ../edit_quality/common/const_types.h \
		../edit_quality/common/transferfunction.h \
		../edit_quality/common/util.h \
		../edit_quality/common/meshmethods.h \
		filterqualitymapper.h \
		../../meshlab/filterparameter.h 

SOURCES       = ../edit_quality/common/transferfunction.cpp \
		../edit_quality/common/util.cpp \		
		../edit_quality/common/meshmethods.cpp \
		filterqualitymapper.cpp \
		../../meshlab/filterparameter.cpp

TARGET        = filter_quality


