include (../../shared.pri)

HEADERS       = common/const_types.h \
		edit_quality_factory.h \
		qualitymapper.h \
		qualitymapperdialog.h \
		common/transferfunction.h \
		common/util.h \
		common/meshmethods.h\
		handle.h \
		eqhandle.h \
		tfhandle.h

SOURCES       = edit_quality_factory.cpp \
		qualitymapper.cpp\
		qualitymapperdialog.cpp\
		common/transferfunction.cpp\
		common/util.cpp \
		common/meshmethods.cpp\
		handle.cpp\
		eqhandle.cpp \
		tfhandle.cpp 

TARGET        = edit_quality

RESOURCES = qualitymapper.qrc

FORMS		  = qualitymapperdialog.ui

QT           += opengl
